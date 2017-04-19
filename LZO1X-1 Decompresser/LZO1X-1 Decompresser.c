/* LZO1X-1 Decompresser.c
 * 
 * ʹ�� LZO1X-1 �㷨�Ľ�ѹ����
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
	printf("\tLZO1X-1 Decompresser.exe inputFileName OutputFileName\n");
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
	lzo_bytep neww; // new �ǹؼ���
	lzo_bytep out;
	lzo_voidp wrkmem;
	lzo_uint out_len;
	lzo_uint new_len;
	FILE* out_fp = NULL;
	FILE* new_fp = NULL;

	/*
	* Ҫ�����������ļ����ؽ��ļ�
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
	if ((out_fp = fopen(argv[1], "rb")) == NULL) {
		printf("����ļ�����");
		return 0;
	}
	if ((new_fp = fopen(argv[2], "wb")) == NULL) {
		printf("�ؽ��ļ�����");
		return 0;
	}
	

	/*
	* Step 3: �����ڴ�
	*/
	out_len = getFileSize(out_fp);
	new_len = 8L * 1024L * 1024L;// TODO: ��ѹ���ļ���С

	if (out_len > ULONG_MAX) {
		printf("�ļ�̫���ˣ�");
		return 0;
	}


	out = (lzo_bytep)xmalloc(out_len);
	neww = (lzo_bytep)xmalloc(new_len);
	wrkmem = (lzo_voidp)xmalloc(LZO1X_1_MEM_COMPRESS);
	if (neww == NULL || out == NULL || wrkmem == NULL)
	{
		printf("out of memory\n");
		return 3;
	}

	if (fread(out, out_len, 1, out_fp) != 1) {
		printf("��ȡ����");
		return 0;
	}

	/*
	* Step 4: decompress from 'out' to 'new' with LZO1X-1
	*/
	r = lzo1x_decompress(out, out_len, neww, &new_len, NULL);
	if (r == LZO_E_OK)
		printf("decompressed %lu bytes back into %lu bytes\n",
		(unsigned long)out_len, (unsigned long)new_len);
	else
	{
		/* this should NEVER happen */
		printf("internal error - decompression failed: %d\n", r);
		return 1;
	}

	if (fwrite(neww, new_len, 1, new_fp) != 1) {
		printf("д�����");
		return 0;
	}

	if (fclose(new_fp) != 0 || fclose(out_fp) != 0) {
		printf("�ر��ļ�����");
		return 0;
	}

	lzo_free(wrkmem);
	lzo_free(out);
	lzo_free(neww);
	printf("Simple compression test passed.\n");
	return 0;
}