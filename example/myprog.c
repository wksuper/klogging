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
