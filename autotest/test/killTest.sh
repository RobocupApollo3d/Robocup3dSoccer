#!/bin/bash
ps -ef |grep rcssserver |awk '{print $2}'|xargs kill -9
ps -ef |grep TEST.sh |awk '{print $2}'|xargs kill -9