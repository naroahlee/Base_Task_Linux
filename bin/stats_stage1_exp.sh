#!/bin/bash

period=100
for((i=0;i<91;i++))
{
	TaskFile=`printf "Task%04d.trace" ${period}`
	./stats_stage1 ${TaskFile}
	let "period = $period + 10"
}

