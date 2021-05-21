#!/bin/bash

# 每队测试次数
TIME=3
# 测试代码文件夹
OPPPATH=../optimization/2018_Bin
# 一局结束的标志
OurCode=../mport.sh
# 结果输出的文件
TEMP_FILE_PATH=temp.log
# 存放比分的文件
TARGET_FILE=SCORE.log
# 存放录像文件夹
LOG_FOLDER_PATH=~/soccerLog
# 生成的录像文件
LOG_FILE_PATH=./sparkmonitor.log
# 每局最长等待时间： 有时rcssserver会突然卡死，尚未找到解决原因
TIME_THRESHOLD=350
# Rcssserver被强制关闭时，往TEMP_FILE_PATH和TARGET_FILE中写的占位符
EMPTY="--------"

usage()
{
  (echo "Usage: $0 [options]"
   echo "Available options:"
   echo "  --help                       prints this"
   echo "  --path                       opp's code path")1>&2
}

while [ $# -gt 0 ]
do
  case $1 in

    --help)
      usage
      exit 0
      ;;

    -p|--path)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      OPPPATH="${2}"
      shift 1
      ;;
  esac
  shift 1
done


export SPARK_SERVERPORT=$[$RANDOM + 1025] #3200
export SPARK_AGENTPORT=$[$RANDOM + 1025] #3100

echo -n "It is: "
date
echo -n "and I am on: "
hostname
echo "Agent port: $SPARK_AGENTPORT, Monitor port: $SPARK_SERVERPORT"


#To view task while it runs uncomment the following line
/home/apollo3d/RoboViz-dev/bin/linux-amd64/roboviz.sh --serverPort=$SPARK_SERVERPORT &
#/home/apollo3d/RoboViz/bin/linux-amd64/roboviz.sh --serverPort=$SPARK_SERVERPORT &
#/home/luyvlei/RoboViz/bin/linux-amd64/roboviz.sh --serverPort=$SPARK_SERVERPORT &


# 清空比较文本
>$TARGET_FILE
>$TEMP_FILE_PATH
if [ ! -d $LOG_FOLDER_PATH ]; then
  mkdir -p $LOG_FOLDER_PATH
fi


dir=$(ls -l $OPPPATH |awk '/^d/ {print $NF}')
for fold in $dir
do
  for (( i = 1; i <= $TIME; i++ ));
    do   
    # 己方优先左场
    rcssserver3d --agent-port $SPARK_AGENTPORT --server-port $SPARK_SERVERPORT &
    PID=$!
    sleep 3 # 防止上人太快

    timeVar=0

    # 己方上人
    $OurCode --mport $SPARK_SERVERPORT --port $SPARK_AGENTPORT &
    sleep 15
    $OPPPATH/$fold/start.sh --port $SPARK_AGENTPORT &
    # 对方上人
	  sleep 15
    echo "=-----=上人完毕=-----="

    # sleep 350
    cat $TARGET_FILE > $TEMP_FILE_PATH
    while [[ $(diff $TEMP_FILE_PATH $TARGET_FILE) -eq 0 ]]; do
      echo "比赛进行中..."
      sleep 10
      let timeVar+=10

      if [[ timeVar -eq TIME_THRESHOLD ]]; then
      	echo "时间到"
      	break
      fi
    done

    if [[ timeVar -eq TIME_THRESHOLD ]]; then
    	#statements
    	echo "=-----=rcssserver3d卡死，强制关闭=-----="
    	kill -s 9 $PID
		echo $EMPTY >> $TEMP_FILE_PATH
		echo $EMPTY >> $TARGET_FILE
    else
    	echo "=-----=半场比完=-----="
    	kill -s 2 $PID
	    if [ -f $LOG_FILE_PATH ];then
       		mv $LOG_FILE_PATH $LOG_FOLDER_PATH/$fold-$i-1.log
    	fi
    	sleep 2
    fi

    echo "Killing Simulator"
    sleep 10

    # ----------------------------左右半场顺序切换------------------------

    rcssserver3d --agent-port $SPARK_AGENTPORT --server-port $SPARK_SERVERPORT &
    PID=$!
    sleep 3 # 防止上人太快

    timeVar=0
    
    # 交换次序
    $OPPPATH/$fold/start.sh --port $SPARK_AGENTPORT &    
    sleep 15
    $OurCode --mport $SPARK_SERVERPORT --port $SPARK_AGENTPORT &
	  sleep 15

    echo "=-----=上人完毕=-----="

    # sleep 350

    cat $TARGET_FILE > $TEMP_FILE_PATH
    while [[ $(diff $TEMP_FILE_PATH $TARGET_FILE) -eq 0 ]]; do
      echo "比赛进行中..."
      sleep 10

      let timeVar+=10
      if [[ timeVar -eq TIME_THRESHOLD ]]; then
      	break
      fi
    done

    if [[ timeVar -eq TIME_THRESHOLD ]]; then
    	#statements
    	echo "=-----=rcssserver3d卡死，强制关闭=-----="
    	kill -s 9 $PID
		echo $EMPTY >> $TEMP_FILE_PATH
		echo $EMPTY >> $TARGET_FILE
    else
    	echo "=-----=整场比完=-----="
    	kill -s 2 $PID
	    if [ -f $LOG_FILE_PATH ];then
       		mv $LOG_FILE_PATH $LOG_FOLDER_PATH/$fold-$i-2.log
    	fi
    	sleep 2
    fi

    echo "Killing Simulator"
    sleep 10 # 防止rcssserver冲突

    done  

done 



sleep 2
echo "Finished"

# 删除临时文件
rm $TEMP_FILE_PATH
