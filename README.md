# klogging

An easy-to-use C/C++ logging library

## Build and Install

```bash
make
sudo make install
sudo ldconfig
```

## Usage Example

--- *myprog.c* ---

```c
#include <klogging.h>

int main(int argc, char *argv[])
{
        KLOG_SET(argc, argv);

        KLOGA("I'm KLOGA");
        KLOGF("I'm KLOGF");
        KLOGE("I'm KLOGE");
        KLOGW("I'm KLOGW");
        KLOGI("I'm KLOGI");
        KLOGD("I'm KLOGD");
        KLOGV("I'm KLOGV");

        return 0;
}
```

Compile *myprog.c* with klogging library

```bash
gcc myprog.c -lklogging -o myprog
```

Run myprog

```
$ ./myprog

$ ./myprog KLOG_ENABLE_OPTIONS=0x0001
06-03 22:19:26.549072 A | I'm KLOGA (myprog.c:7:main)

$ ./myprog KLOG_ENABLE_OPTIONS=0x0001 KLOG_SET_LEVEL=6
06-03 22:19:29.272033 A | I'm KLOGA (myprog.c:7:main)
06-03 22:19:29.272163 F | I'm KLOGF (myprog.c:8:main)
06-03 22:19:29.272186 E | I'm KLOGE (myprog.c:9:main)
06-03 22:19:29.272200 W | I'm KLOGW (myprog.c:10:main)
06-03 22:19:29.272213 I | I'm KLOGI (myprog.c:11:main)
06-03 22:19:29.272226 D | I'm KLOGD (myprog.c:12:main)
06-03 22:19:29.272239 V | I'm KLOGV (myprog.c:13:main)

$ ./myprog KLOG_ENABLE_OPTIONS=0x1001 KLOG_SET_LEVEL=6
A | I'm KLOGA (myprog.c:7:main)
F | I'm KLOGF (myprog.c:8:main)
E | I'm KLOGE (myprog.c:9:main)
W | I'm KLOGW (myprog.c:10:main)
I | I'm KLOGI (myprog.c:11:main)
D | I'm KLOGD (myprog.c:12:main)
V | I'm KLOGV (myprog.c:13:main)

$ ./myprog KLOG_ENABLE_OPTIONS=0x2001 KLOG_SET_LEVEL=6
06-03 22:19:35.668146 | I'm KLOGA (myprog.c:7:main)
06-03 22:19:35.668330 | I'm KLOGF (myprog.c:8:main)
06-03 22:19:35.668360 | I'm KLOGE (myprog.c:9:main)
06-03 22:19:35.668388 | I'm KLOGW (myprog.c:10:main)
06-03 22:19:35.668412 | I'm KLOGI (myprog.c:11:main)
06-03 22:19:35.668436 | I'm KLOGD (myprog.c:12:main)
06-03 22:19:35.668459 | I'm KLOGV (myprog.c:13:main)

$ ./myprog KLOG_ENABLE_OPTIONS=0x3001 KLOG_SET_LEVEL=6
I'm KLOGA (myprog.c:7:main)
I'm KLOGF (myprog.c:8:main)
I'm KLOGE (myprog.c:9:main)
I'm KLOGW (myprog.c:10:main)
I'm KLOGI (myprog.c:11:main)
I'm KLOGD (myprog.c:12:main)
I'm KLOGV (myprog.c:13:main)

$ ./myprog KLOG_ENABLE_OPTIONS=0x4001 KLOG_SET_LEVEL=6
06-03 22:19:41.520533 A | I'm KLOGA
06-03 22:19:41.520670 F | I'm KLOGF
06-03 22:19:41.520691 E | I'm KLOGE
06-03 22:19:41.520708 W | I'm KLOGW
06-03 22:19:41.520724 I | I'm KLOGI
06-03 22:19:41.520740 D | I'm KLOGD
06-03 22:19:41.520754 V | I'm KLOGV

$ ./myprog KLOG_ENABLE_OPTIONS=0x5001 KLOG_SET_LEVEL=6
A | I'm KLOGA
F | I'm KLOGF
E | I'm KLOGE
W | I'm KLOGW
I | I'm KLOGI
D | I'm KLOGD
V | I'm KLOGV

$ ./myprog KLOG_ENABLE_OPTIONS=0x6001 KLOG_SET_LEVEL=6
06-03 22:19:48.179275 | I'm KLOGA
06-03 22:19:48.179489 | I'm KLOGF
06-03 22:19:48.179519 | I'm KLOGE
06-03 22:19:48.179539 | I'm KLOGW
06-03 22:19:48.179557 | I'm KLOGI
06-03 22:19:48.179587 | I'm KLOGD
06-03 22:19:48.179599 | I'm KLOGV

$ ./myprog KLOG_ENABLE_OPTIONS=0x7001 KLOG_SET_LEVEL=6
I'm KLOGA
I'm KLOGF
I'm KLOGE
I'm KLOGW
I'm KLOGI
I'm KLOGD
I'm KLOGV
```
