

#ifndef __MD5_H
#define __MD5_H

#include <string.h>


//---------------------------------------------------------------------
typedef unsigned long UINT4;
typedef unsigned char *POINTER;

//----------------------------�ṹ�嶨��----------------------------------------
/* MD5 context. */
typedef struct {
    /* state (ABCD) */
    /*�ĸ�32bits�������ڴ�����ռ���õ�����ϢժҪ������Ϣ���ȡ�512bitsʱ��Ҳ
    ���ڴ��ÿ��512bits���м���*/
    UINT4 state[4];

    /* number of bits, modulo 2^64 (lsb first) */
    /*�洢ԭʼ��Ϣ��bits������,����������bits���Ϊ 2^64 bits��
    ��Ϊ2^64��һ��64λ�������ֵ*/
    UINT4 count[2];

    /* input buffer */
    /*����������Ϣ�Ļ�������512bits*/
    unsigned char buffer[64];
} MD5_CTX;

//----------------------------�ⲿ�����ӿ�--------------------------------------
extern void md5_init(MD5_CTX *);
extern void md5_update(MD5_CTX *, unsigned char *, unsigned int);
extern void md5_final(unsigned char [16], MD5_CTX *);
extern void md5_calculation(unsigned char * input, unsigned long blen, unsigned char * output);

#endif /* __MD5_H */

