#!/bin/bash
set -v

./myprog KLOG_SET_OPTIONS=0x1 KLOG_SET_LEVEL=0 KLOG_SET_FILE=mylog_off.txt

./myprog KLOG_SET_OPTIONS=0x1 KLOG_SET_LEVEL=1 KLOG_SET_FILE=mylog_error.txt

./myprog KLOG_SET_OPTIONS=0x1 KLOG_SET_LEVEL=2 KLOG_SET_FILE=mylog_warning.txt

./myprog KLOG_SET_OPTIONS=0x1 KLOG_SET_LEVEL=3 KLOG_SET_FILE=mylog_info.txt

./myprog KLOG_SET_OPTIONS=0x1 KLOG_SET_LEVEL=4 KLOG_SET_FILE=mylog_debug.txt

./myprog KLOG_SET_OPTIONS=0x1 KLOG_SET_LEVEL=5 KLOG_SET_FILE=mylog_verbose.txt

./myprog KLOG_SET_OPTIONS=0x701 KLOG_SET_LEVEL=5 KLOG_SET_FILE=mylog_filename_linenum_functionname_verbose.txt
