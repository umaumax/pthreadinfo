#include <dlfcn.h>
#include <execinfo.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#ifndef DISABLE_UNWIND
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif

#ifndef DISABLE_UNWIND
#ifdef __APPLE__
const char *unw_strerror(int err_code) {
  if (err_code == -UNW_EUNSPEC) {
    return "An unspecified error occurred.";
  } else if (err_code == -UNW_ENOINFO) {  // stripped
    return "Libunwind was unable to determine the name of the procedure.";
  } else if (err_code == -UNW_ENOMEM) {
    return "The procedure name is too long to fit in the buffer provided. A "
           "truncated version of the name has been returned.";
  }
  return "Unknown error code";
}
#endif
#endif

pid_t gettid() {
#if __APPLE__
  uint64_t tid64 = 0;
  pthread_threadid_np(nullptr, &tid64);
  return static_cast<pid_t>(tid64);
  // deprecated way
  // return syscall(SYS_thread_selfid);
#else
  return syscall(SYS_gettid);
#endif
}

pid_t get_pthread_tid(pthread_t *pthread) {
  uint64_t tid64 = 0;
  pthread_threadid_np(*pthread, &tid64);
  return static_cast<pid_t>(tid64);
}

class BackTracer {
 public:
  BackTracer(){};
  virtual ~BackTracer(){};

  virtual std::vector<std::string> GetBackTrace() = 0;
  virtual std::string GetBackTraceString()        = 0;
};

class GlibcBackTracer : public BackTracer {
 public:
  GlibcBackTracer() {}
  ~GlibcBackTracer() {}

  std::vector<std::string> GetBackTrace() override {
    int trace_size = 64;
    void *trace[trace_size];
    std::size_t size = backtrace(trace, trace_size);
    char **symbols   = backtrace_symbols(trace, size);
    std::vector<std::string> result(symbols, symbols + size);
    free(symbols);
    return result;
  }

  std::string GetBackTraceString() override {
    std::stringstream ss;
    std::vector<std::string> backtrace = GetBackTrace();
    int count                          = 0;
    for (std::size_t i = 2; i < backtrace.size(); i++) {
      ss << "[" << count << "] from " << backtrace[i].c_str() << std::endl;
      count++;
    }
    return ss.str();
  }
};

#ifndef DISABLE_UNWIND
class UnwindBackTracer : public BackTracer {
 public:
  UnwindBackTracer() {}
  ~UnwindBackTracer() {}

  std::vector<std::string> GetBackTrace() override {
    unw_cursor_t cursor;
    unw_context_t context;
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    std::vector<std::string> result;
    char func_name[512];
    for (int frame = 0; unw_step(&cursor) > 0; frame++) {
      unw_word_t offset, pc;
      unw_get_reg(&cursor, UNW_REG_IP, &pc);
      func_name[0] = 0;
      int ret =
          unw_get_proc_name(&cursor, func_name, sizeof(func_name), &offset);
      if (ret < 0) {
        if (ret == -UNW_EUNSPEC) {
          // An unspecified error occurred.
          // std::cerr << "unw_get_proc_name() UNW_EUNSPEC: " << unw_strerror(ret)
          // << std::endl;
        } else if (ret == -UNW_ENOINFO) {  // stripped
          // Libunwind was unable to determine the name of the procedure.
          // std::cerr << "unw_get_proc_name() UNW_ENOINFO: " << unw_strerror(ret)
          // << std::endl;
        } else if (ret == -UNW_ENOMEM) {
          // The procedure name is too long to fit in the buffer provided. A truncated version of the name has been returned.
          // std::cerr << "unw_get_proc_name() UNW_ENOMEM: " << unw_strerror(ret)
          // << std::endl;
        } else {
          // std::cerr << "unw_get_proc_name() unknown erro code: " << ret << " "
          // << unw_strerror(ret) << std::endl;
        }
        // continue;
      }

      void *frame_addr = reinterpret_cast<void *>(pc);
      Dl_info info;
      dladdr(frame_addr, &info);
      {
        std::stringstream ss;
        ss << info.dli_fname << "(" << func_name << "+0x" << offset << ") ["
           << frame_addr << "]";
        result.emplace_back(ss.str());
      }
    }
    return result;
  }

  std::string GetBackTraceString() override {
    std::stringstream ss;
    std::vector<std::string> backtrace = GetBackTrace();
    int count                          = 0;
    for (std::size_t i = 1; i < backtrace.size(); i++) {
      ss << "[" << count << "] from " << backtrace[i].c_str() << std::endl;
      count++;
    }
    return ss.str();
  }
};
#endif

class BackTracerDB {
 public:
  BackTracerDB() {}
  ~BackTracerDB() {}

  void Add(pid_t tid, std::string backtrace_info) {
    backtrace_map.insert(std::make_pair(tid, backtrace_info));
  }

  void Dump(std::ostream &oss) {
    for (auto &v : backtrace_map) {
      // const pid_t &tid                  = v.first;
      const std::string &backtrace_info = v.second;
      // oss << tid << std::endl;
      oss << backtrace_info << std::endl;
    }
  }

 private:
  std::map<pid_t, std::string> backtrace_map;
};

class BackTracerConfig {
 public:
  BackTracerConfig() {
    char *env;
    env = getenv("BACKTRACER_STDERR");
    if (env != nullptr) {
      std::string env_stderr = std::string(env);
      if (env_stderr == "1") {
        stderr_flag_ = true;
      }
    }
    env = getenv("BACKTRACER_OUTFILE");
    if (env != nullptr) {
      output_filepath_ = std::string(env);
    }
  }
  ~BackTracerConfig() {}

  void SetStdErrFlag(bool v) { stderr_flag_ = v; }
  bool GetStdErrFlag() { return stderr_flag_; }

  void SetOutputFilepath(std::string v) { output_filepath_ = v; }
  std::string &GetOutputFilepath() { return output_filepath_; }

 private:
  bool stderr_flag_            = false;
  std::string output_filepath_ = "threadinfo.log";
};

namespace {
#ifndef DISABLE_UNWIND
UnwindBackTracer unwind_back_tracer;
BackTracer *back_tracer_ptr = &unwind_back_tracer;
#else
GlibcBackTracer glibc_back_tracer;
BackTracer *back_tracer_ptr = &glibc_back_tracer;
#endif
BackTracerConfig back_tracer_config;
BackTracerDB back_tracer_db;
}  // namespace

extern "C" {
#ifdef __linux__
int pthread_create(pthread_t *__restrict __newthread,
                   const pthread_attr_t *__restrict __attr,
                   void *(*__start_routine)(void *),
                   void *__restrict __arg) __THROWNL __nonnull((1, 3));
int pthread_create(pthread_t *__restrict __newthread,
                   const pthread_attr_t *__restrict __attr,
                   void *(*__start_routine)(void *),
                   void *__restrict __arg) __THROWNL {
#elif __APPLE__
int pthread_create(pthread_t *__restrict __newthread,
                   const pthread_attr_t *__restrict __attr,
                   void *(*__start_routine)(void *), void *__restrict __arg);
int pthread_create(pthread_t *__restrict __newthread,
                   const pthread_attr_t *__restrict __attr,
                   void *(*__start_routine)(void *), void *__restrict __arg) {
#else
#error "Non supported OS"
#endif
  using libc_pthread_create_pointer_type = decltype(&pthread_create);
  static libc_pthread_create_pointer_type libc_pthread_create = nullptr;
  if (libc_pthread_create == nullptr) {
    libc_pthread_create =
        (libc_pthread_create_pointer_type)dlsym(RTLD_NEXT, "pthread_create");
    assert(libc_pthread_create && "failed dlsym pthread_create");
  }
  int ret = libc_pthread_create(__newthread, __attr, __start_routine, __arg);
  if (ret == 0) {
    pid_t current_tid            = gettid();
    pid_t new_tid                = get_pthread_tid(__newthread);
    std::string backtrace_result = back_tracer_ptr->GetBackTraceString();
    if (back_tracer_config.GetStdErrFlag()) {
      std::cerr << new_tid << " created by " << current_tid << std::endl;
      std::cerr << backtrace_result << std::endl;
    }
    auto micro_since_epoch =
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    std::stringstream info_ss;
    info_ss << "[" << micro_since_epoch / 1000000 << "."
            << micro_since_epoch % 1000000 << "]" << std::endl
            << new_tid << " created by " << current_tid << std::endl
            << backtrace_result;
    back_tracer_db.Add(new_tid, info_ss.str());
    if (!back_tracer_config.GetOutputFilepath().empty()) {
      std::ofstream output_db_file(back_tracer_config.GetOutputFilepath());
      back_tracer_db.Dump(output_db_file);
    }
#if 0
// NOTE: below API is not useful because of short name limitation and using nonportable function
#ifdef __linux__
    std::string function_name = "dummy name of thread";
    const int pthread_name_buffer_length = 16;
    const int pthread_name_max_length    = pthread_name_buffer_length - 1;
    int ret                              = pthread_setname_np(
        *__newthread, function_name.substr(0, pthread_name_max_length).c_str());
    if (ret != 0) {
      // ignore error
      // std::cerr << "pthread_setname_np() failed:" << strerror(errno)
      // << std::endl;
    }
#endif
#endif
  }
  return ret;
}
}
