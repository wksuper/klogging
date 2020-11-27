#!/bin/bash
set -v
./myprog KLOG_SET_OPTIONS=0x0101 KLOG_SET_LEVEL=5 KLOG_SET_LINEEND=" custom line end string\n" KLOG_SET_FILE=mylog.txt
