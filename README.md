# pthreadinfo

This tool generates pthread backtrace infomation automatically.

## how to build
``` bash
# for linux
g++ -std=c++11 main.cpp -Wall -ldl -lunwind -lpthread -shared -fPIC -o libpthreadinfo.so
# for Mac OS X (no need to add -lunwind to use unwind)
g++ -std=c++11 main.cpp -Wall -ldl -lpthread -shared -fPIC -o libpthreadinfo.so
```

* default setting is using `libuwind`
  * if you want to disable `libuwind` add `-DDISABLE_UNWIND` (glibc backrace is enabled)

## how to use
``` bash
# for linux
LD_PRELOAD=./libpthreadinfo.so ./thread-app

# for Mac OS X
DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=./libpthreadinfo.so ./thread-app
```

with sudo, without ASLR(Linux)
``` bash
sudo env LD_PRELOAD=./libpthreadinfo.so setarch $(uname -m) -R ./thread-app
```

* if you want to display thread info to stderr, set `BACKTRACER_STDERR=1` environment variable
* `BACKTRACER_OUTFILE=threadinfo.log`: filepath to output each thread backtrace info

## examples
output log format
```
[$CURRENT_TIME(second)]
$CREATED_TID created by $CREATOR_TID
$BACKTRACE
```

### glibc backtrace at Mac OS X
e.g.
```
[1624110834.316724]
203904329 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632bec _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE24__emplace_back_slow_pathIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 164
[8] from 10  thread-app                          0x0000000104631e94 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 120
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630d8c main + 248
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.318797]
203904330 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632bec _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE24__emplace_back_slow_pathIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 164
[8] from 10  thread-app                          0x0000000104631e94 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 120
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630d8c main + 248
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.319460]
203904331 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632bec _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE24__emplace_back_slow_pathIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 164
[8] from 10  thread-app                          0x0000000104631e94 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 120
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630d8c main + 248
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.320271]
203904332 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632af4 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE22__construct_one_at_endIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 112
[8] from 10  thread-app                          0x0000000104631e74 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 88
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630d8c main + 248
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.321250]
203904333 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632bec _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE24__emplace_back_slow_pathIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 164
[8] from 10  thread-app                          0x0000000104631e94 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 120
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630fe4 main + 848
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.322053]
203904334 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632bec _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE24__emplace_back_slow_pathIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 164
[8] from 10  thread-app                          0x0000000104631e94 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 120
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630fe4 main + 848
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.322568]
203904335 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632bec _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE24__emplace_back_slow_pathIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 164
[8] from 10  thread-app                          0x0000000104631e94 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 120
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630fe4 main + 848
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4

[1624110834.323145]
203904336 created by 203904316
[0] from 2   libpthreadinfo.so                   0x00000001046a5748 pthread_create + 276
[1] from 3   thread-app                          0x00000001046331c0 _ZNSt3__1L22__libcpp_thread_createEPP17_opaque_pthread_tPFPvS3_ES3_ + 48
[2] from 4   thread-app                          0x0000000104632fa4 _ZNSt3__16threadC2IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 252
[3] from 5   thread-app                          0x0000000104632e94 _ZNSt3__16threadC1IZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_JEvEEOT_DpOT0_ + 40
[4] from 6   thread-app                          0x0000000104632e60 _ZNSt3__19allocatorINS_6threadEE9constructIS1_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvPT_DpOT0_ + 64
[5] from 7   thread-app                          0x0000000104632e14 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE11__constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvNS_17integral_constantIbLb1EEERS3_PT_DpOT0_ + 76
[6] from 8   thread-app                          0x0000000104632d04 _ZNSt3__116allocator_traitsINS_9allocatorINS_6threadEEEE9constructIS2_JZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvRS3_PT_DpOT0_ + 76
[7] from 9   thread-app                          0x0000000104632af4 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE22__construct_one_at_endIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 112
[8] from 10  thread-app                          0x0000000104631e74 _ZNSt3__16vectorINS_6threadENS_9allocatorIS1_EEE12emplace_backIJZN10ThreadPoolC1EiNS_8functionIFviEEEEUlvE_EEEvDpOT_ + 88
[9] from 11  thread-app                          0x0000000104631c90 _ZN10ThreadPoolC2EiNSt3__18functionIFviEEE + 240
[10] from 12  thread-app                          0x000000010463122c _ZN10ThreadPoolC1EiNSt3__18functionIFviEEE + 40
[11] from 13  thread-app                          0x0000000104630fe4 main + 848
[12] from 14  libdyld.dylib                       0x0000000196abd450 start + 4
```
