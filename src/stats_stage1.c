/* stats_stage1.c */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "tracing.h"
#include "calibration.h"

size_t getFileSize(FILE* fp)
{
    size_t s32size;
    fseek(fp, 0L, SEEK_END);
    s32size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return s32size;
}


int main(int argc, char* argv[])
{
	FILE* fin;
	char acInput[60] = "";
	int s32i;
	int s32ret;
	size_t s32totalsz;
	int s32cnt;
	event_record *pstrrec;
	double d64avg;
	uint64_t u64diff;
	uint64_t u64total;


	strcpy(acInput, argv[1]);
	fin = fopen(acInput, "rb");
	if(NULL == fin)
	{
		fprintf(stderr, "Open %s Error!\n", acInput);
		exit(EXIT_FAILURE);
	}

	s32totalsz = getFileSize(fin);
	s32cnt = s32totalsz / sizeof(event_record);

	/*printf("%d total", s32cnt); */

	pstrrec = (event_record *)malloc(s32totalsz);
	if(NULL == pstrrec)
	{
		fprintf(stderr, "malloc Error!\n");
		exit(EXIT_FAILURE);
	}

	s32ret = fread(pstrrec, sizeof(event_record), s32cnt, fin);
	if(s32cnt != s32ret)
	{
		fprintf(stderr, "fread() Error!\n");
		exit(-1);
	}

	u64total = 0;
	for(s32i = 0; s32i < s32cnt - 1; s32i++)
	{
		u64diff = pstrrec[s32i + 1].u64tsc
                - pstrrec[s32i].u64tsc;
		u64total += u64diff;
	}
	d64avg = (double)(u64total) / (s32cnt - 1);

	printf("%lf\n", d64avg);

	free(pstrrec);
	fclose(fin);
	return 0;
}
