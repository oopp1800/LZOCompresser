/* LZO1X-1 Compresser.c
 * 
 * ʹ�� LZO1X-1 �㷨��ѹ������
 *
 * �޸��Դ��� simple.c��LZO ���ʾ�����룩
 * LZO�ٷ��⣺http://www.oberhumer.com/opensource/lzo/
 *
 * ���� GNU Э��
 *
 * Brain Bag
 * <me@bbag.info>
 *
 */


/* 
 * Ϊ��ʹ�� LZO1X �㷨������ lzo1x.h
 */

#include <lzo/lzoconf.h>
#include <lzo/lzo1x.h>

/* portability layer */
static const char *progname = NULL;
#define WANT_LZO_MALLOC 1
#define WANT_XMALLOC 1
#include "examples/portab.h"

void usage() {
	printf("Usage:\n");
	printf("\tLZO1X-1 Compresser.exe inputFileName OutputFileName\n");
}

lzo_uint getFileSize(FILE* fp) {
	lzo_uint size;

	if (fp == NULL) return 0; // ���ļ�
	
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	rewind(fp);
	return size;
}

int __lzo_cdecl_main main(int argc, char *argv[])
{
	int r;
	lzo_bytep in;
	lzo_bytep out;
	lzo_voidp wrkmem;
	lzo_uint in_len;
	lzo_uint out_len;
	//lzo_uint new_len;
	FILE* in_fp = NULL;
	FILE* out_fp = NULL;

	/*
	* Ҫ������������ļ�������ļ�
	*/
	if (argc != 3) {
		usage();
		return 0;
	}

	printf("\nLZO real-time data compression library (v%s, %s).\n",
		lzo_version_string(), lzo_version_date());
	printf("Copyright (C) 1996-2017 Markus Franz Xaver Johannes Oberhumer\nAll Rights Reserved.\n\n");
	printf("�޸��ߣ�Brain Bag.\n\n");

	/*
	* Step 1: ��ʼ�� LZO ��
	*/
	if (lzo_init() != LZO_E_OK)
	{
		printf("internal error - lzo_init() failed !!!\n");
		printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
		return 4;
	}

	/*
	 * Step 2: ��ȡ�ļ�
	 */
	if ((in_fp = fopen(argv[1], "rb")) == NULL) {
		printf("�����ļ�����");
		return 0;
	}
	if ((out_fp = fopen(argv[2], "wb")) == NULL) {
		printf("����ļ�����");
		return 0;
	}
	

	/*
	* Step 3: �����ڴ�
	*/
	in_len = getFileSize(in_fp);
	out_len = in_len + in_len / 16 + 64 + 3; // ��֤ѹ�������ݷ������ӵ���������ܹ���

	if (out_len > ULONG_MAX) {
		printf("�ļ�̫���ˣ�");
		return 0;
	}


	in = (lzo_bytep)xmalloc(in_len);
	out = (lzo_bytep)xmalloc(out_len);
	wrkmem = (lzo_voidp)xmalloc(LZO1X_1_MEM_COMPRESS);
	if (in == NULL || out == NULL || wrkmem == NULL)
	{
		printf("out of memory\n");
		return 3;
	}

	if (fread(in, in_len, 1, in_fp) != 1) {
		printf("��ȡ����");
		return 0;
	}

	/*
	* Step 4: compress from 'in' to 'out' with LZO1X-1
	*/
	r = lzo1x_1_compress(in, in_len, out, &out_len, wrkmem);
	if (r == LZO_E_OK)
		printf("compressed %lu bytes into %lu bytes\n",
		(unsigned long)in_len, (unsigned long)out_len);
	else
	{
		/* this should NEVER happen */
		printf("internal error - compression failed: %d\n", r);
		return 2;
	}
	/* check for an incompressible block */
	if (out_len >= in_len)
	{
		printf("This block contains incompressible data.\n");
		return 0;
	}

	if (fwrite(out, out_len, 1, out_fp) != 1) {
		printf("д�����");
		return 0;
	}

	if (fclose(in_fp) != 0 || fclose(out_fp) != 0) {
		printf("�ر��ļ�����");
		return 0;
	}

	lzo_free(wrkmem);
	lzo_free(out);
	lzo_free(in);
	printf("Simple compression test passed.\n");
	return 0;
}