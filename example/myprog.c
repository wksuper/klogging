#include <klogging.h>

static void PrintSomething(void)
{
        KCONSOLE("I'm KCONSOLE\n");
        KLOGE("I'm KLOGE\n");
        KLOGW("I'm KLOGW\n");
        KLOGI("I'm KLOGI\n");
        KLOGD("I'm KLOGD\n");
        KLOGV("I'm KLOGV\n");
}

int main(int argc, char *argv[])
{
        printf("klogging version %s\n\n", KVERSION());

        printf("Setting KLOGGING_TO_STDOUT:\n");
        KLOG_SET_OPTIONS(KLOGGING_TO_STDOUT);
        printf("Setting done\n\n");

        printf("Testing KLOGGING_LEVEL_OFF:\n");
        KLOG_SET_LEVEL(KLOGGING_LEVEL_OFF);
        PrintSomething();
        printf("Testing done\n\n");

        printf("Testing KLOGGING_LEVEL_ERROR:\n");
        KLOG_SET_LEVEL(KLOGGING_LEVEL_ERROR);
        PrintSomething();
        printf("Testing done\n\n");

        printf("Testing KLOGGING_LEVEL_WARNING:\n");
        KLOG_SET_LEVEL(KLOGGING_LEVEL_WARNING);
        PrintSomething();
        printf("Testing done\n\n");

        printf("Testing KLOGGING_LEVEL_INFO:\n");
        KLOG_SET_LEVEL(KLOGGING_LEVEL_INFO);
        PrintSomething();
        printf("Testing done\n\n");

        printf("Testing KLOGGING_LEVEL_DEBUG:\n");
        KLOG_SET_LEVEL(KLOGGING_LEVEL_DEBUG);
        PrintSomething();
        printf("Testing done\n\n");

        printf("Testing KLOGGING_LEVEL_VERBOSE:\n");
        KLOG_SET_LEVEL(KLOGGING_LEVEL_VERBOSE);
        PrintSomething();
        printf("Testing done\n\n");

        printf("Setting KLOGGING_PRINT_FILE_NAME:\n");
        KLOG_SET_OPTIONS(KLOG_GET_OPTIONS() | KLOGGING_PRINT_FILE_NAME);
        printf("Setting done\n");
        printf("Testing:\n");
        PrintSomething();
        printf("Testing done\n\n");

        printf("Setting KLOGGING_PRINT_LINE_NUM:\n");
        KLOG_SET_OPTIONS(KLOG_GET_OPTIONS() | KLOGGING_PRINT_LINE_NUM);
        printf("Setting done\n");
        printf("Testing:\n");
        PrintSomething();
        printf("Testing done\n\n");

        printf("Setting KLOGGING_PRINT_FUNCTION_NAME:\n");
        KLOG_SET_OPTIONS(KLOG_GET_OPTIONS() | KLOGGING_PRINT_FUNCTION_NAME);
        printf("Setting done\n");
        printf("Testing:\n");
        PrintSomething();
        printf("Testing done\n\n");

        printf("Setting KLOGGING_NO_TIMESTAMP:\n");
        KLOG_SET_OPTIONS(KLOG_GET_OPTIONS() | KLOGGING_NO_TIMESTAMP);
        printf("Setting done\n");
        printf("Testing:\n");
        PrintSomething();
        printf("Testing done\n\n");

        return 0;
}
