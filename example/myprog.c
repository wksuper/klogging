#include <klogging.h>

int main(int argc, char *argv[])
{
        KLOG_SET(argc, argv);

        KCONSOLE("I'm KCONSOLE\n");
        KLOGE("I'm KLOGE\n");
        KLOGW("I'm KLOGW\n");
        KLOGI("I'm KLOGI\n");
        KLOGD("I'm KLOGD\n");
        KLOGV("I'm KLOGV\n");

        return 0;
}
