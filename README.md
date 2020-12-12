# klogging
An easy-to-use C/C++ logging library

Build and install
```
$ make
$ sudo make install
$ sudo ldconfig
```
Usage example:

myprog.c
```
#include <klogging.h>

int main(int argc, char *argv[])
{
        KLOG_SET(argc, argv);

        KCONSOLE("I'm KCONSOLE");
        KLOGE("I'm KLOGE");
        KLOGW("I'm KLOGW");
        KLOGI("I'm KLOGI");
        KLOGD("I'm KLOGD");
        KLOGV("I'm KLOGV");

        return 0;
}
```
Compile myprog with klogging library
```
$ gcc myprog.c -lklogging -o myprog
```
Run myprog
```
$ ./myprog
I'm KCONSOLE

$ ./myprog KLOG_SET_OPTIONS=0x0001
12-13 02:39:01.758600 C | I'm KCONSOLE (myprog.c:9:main)

$ ./myprog KLOG_SET_OPTIONS=0x0001 KLOG_SET_LEVEL=5
12-13 02:39:06.503092 C | I'm KCONSOLE (myprog.c:9:main)
12-13 02:39:06.503251 E | I'm KLOGE (myprog.c:10:main)
12-13 02:39:06.503288 W | I'm KLOGW (myprog.c:11:main)
12-13 02:39:06.503316 I | I'm KLOGI (myprog.c:12:main)
12-13 02:39:06.503344 D | I'm KLOGD (myprog.c:13:main)
12-13 02:39:06.503371 V | I'm KLOGV (myprog.c:14:main)

$ ./myprog KLOG_SET_OPTIONS=0x1001 KLOG_SET_LEVEL=5
C | I'm KCONSOLE (myprog.c:9:main)
E | I'm KLOGE (myprog.c:10:main)
W | I'm KLOGW (myprog.c:11:main)
I | I'm KLOGI (myprog.c:12:main)
D | I'm KLOGD (myprog.c:13:main)
V | I'm KLOGV (myprog.c:14:main)

$ ./myprog KLOG_SET_OPTIONS=0x2001 KLOG_SET_LEVEL=5
12-13 02:40:03.942040 | I'm KCONSOLE (myprog.c:9:main)
12-13 02:40:03.942094 | I'm KLOGE (myprog.c:10:main)
12-13 02:40:03.942102 | I'm KLOGW (myprog.c:11:main)
12-13 02:40:03.942115 | I'm KLOGI (myprog.c:12:main)
12-13 02:40:03.942126 | I'm KLOGD (myprog.c:13:main)
12-13 02:40:03.942133 | I'm KLOGV (myprog.c:14:main)

$ ./myprog KLOG_SET_OPTIONS=0x3001 KLOG_SET_LEVEL=5
I'm KCONSOLE (myprog.c:9:main)
I'm KLOGE (myprog.c:10:main)
I'm KLOGW (myprog.c:11:main)
I'm KLOGI (myprog.c:12:main)
I'm KLOGD (myprog.c:13:main)
I'm KLOGV (myprog.c:14:main)

$ ./myprog KLOG_SET_OPTIONS=0x4001 KLOG_SET_LEVEL=5
12-13 02:40:12.488137 C | I'm KCONSOLE
12-13 02:40:12.488193 E | I'm KLOGE
12-13 02:40:12.488204 W | I'm KLOGW
12-13 02:40:12.488212 I | I'm KLOGI
12-13 02:40:12.488220 D | I'm KLOGD
12-13 02:40:12.488228 V | I'm KLOGV

$ ./myprog KLOG_SET_OPTIONS=0x5001 KLOG_SET_LEVEL=5
C | I'm KCONSOLE
E | I'm KLOGE
W | I'm KLOGW
I | I'm KLOGI
D | I'm KLOGD
V | I'm KLOGV

$ ./myprog KLOG_SET_OPTIONS=0x6001 KLOG_SET_LEVEL=5
12-13 02:40:24.851753 | I'm KCONSOLE
12-13 02:40:24.851805 | I'm KLOGE
12-13 02:40:24.851817 | I'm KLOGW
12-13 02:40:24.851825 | I'm KLOGI
12-13 02:40:24.851833 | I'm KLOGD
12-13 02:40:24.851839 | I'm KLOGV

$ ./myprog KLOG_SET_OPTIONS=0x7001 KLOG_SET_LEVEL=5
I'm KCONSOLE
I'm KLOGE
I'm KLOGW
I'm KLOGI
I'm KLOGD
I'm KLOGV
```
