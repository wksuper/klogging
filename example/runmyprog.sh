#!/bin/bash
set -v
./myprog KLOG_ENABLE_OPTIONS=0x0001 KLOG_SET_LEVEL=6 KLOG_SET_FILE=mylog.txt
