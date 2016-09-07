#!/bin/bash

Qcnt=1000
for((i=0;i<91;i++))
{
	TaskFile=`printf "Task%05d.dat" ${Qcnt}`
	./stats_stage2 ${TaskFile}
	let "Qcnt = ${Qcnt} + 100"
}

