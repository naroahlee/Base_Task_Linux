#!/bin/bash

rm ./*.dat
Qcnt=1000
for((i=0;i<91;i++))
{
	TaskFile=`printf "Task%05d.dat" ${Qcnt}`
	./calib_stage2 2 ${Qcnt} 95 ${TaskFile}
	let "Qcnt = ${Qcnt} + 100"
}

