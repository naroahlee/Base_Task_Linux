#!/bin/bash

rm ./*.trace
period=100
for((i=0;i<91;i++))
{
	./calib_stage1 ${period} 101 95 ${period}
	let "period = $period + 10"
	sleep 1
}

