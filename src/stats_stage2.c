#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "tracing.h"
#include "calibration.h"

int main(int argc, char* argv[])
{
	FILE* fin;
	record_header strRec;
	char acInput[60] = "";
	int s32i;
	int s32ret;
	uint64_t* pu64rec;
	double d64total;
	double d64temp;
	double d64max;

	strcpy(acInput, argv[1]);
	fin = fopen(acInput, "rb");
	if(NULL == fin)
	{
		fprintf(stderr, "Open %s Error!\n", acInput);
		exit(EXIT_FAILURE);
	}

	s32ret = fread(&strRec, sizeof(record_header), 1, fin);
	if(1 != s32ret)
	{
		fprintf(stderr, "fread() Error!\n");
		exit(-1);
	}

	pu64rec = (uint64_t*)malloc(sizeof(uint64_t) * strRec.s32cnt);
	if(NULL == pu64rec)
	{
		fprintf(stderr, "malloc Error!\n");
		exit(EXIT_FAILURE);
	}

	s32ret = fread(pu64rec, sizeof(uint64_t), strRec.s32cnt, fin);
	if(strRec.s32cnt != s32ret)
	{
		fprintf(stderr, "fread() Error!\n");
		exit(-1);
	}

	d64total = 0.0;
	d64max  = 0.0;
	for(s32i = 0; s32i < strRec.s32cnt; s32i++)
	{
		d64temp = ((double)(pu64rec[s32i]) / KHZ);
		d64total += d64temp;
		if(d64temp >= d64max)
		{
			d64max = d64temp;
		}
	}
	d64total /= strRec.s32cnt;

	printf("%d,%lf,%lf\n", strRec.s32Qcnt, d64total, d64max);

	free(pu64rec);
	fclose(fin);
	return 0;
}
