

#ifndef __AES_H
#define __AES_H

#include <stdint.h>
#include <string.h>

/*
 * ����AES���ܽ��ܲο���վ��http://aes.online-domain-tools.com/
*/

#define KEY_COUNT 1 /* ѡ��AES128��AES196��AES256 */

#if KEY_COUNT == 1
    #define KEYBITS 128 //!< Use AES128.
#elif KEY_COUNT == 2
    #define KEYBITS 192 //!< Use AES196.
#elif KEY_COUNT == 3
    #define KEYBITS 256 //!< Use AES256.
#else
    #error Use 1, 2 or 3 keys!
#endif

#if KEYBITS == 128
    #define ROUNDS 10    //!< Number of rounds.
    #define KEYLENGTH 16 //!< Key length in number of bytes.
#elif KEYBITS == 192
    #define ROUNDS 12    //!< Number of rounds.
    #define KEYLENGTH 24 //!< Key length in number of bytes.
#elif KEYBITS == 256
    #define ROUNDS 14    //!< Number of rounds.
    #define KEYLENGTH 32 //!< Key length in number of bytes.
#else
    #error Key must be 128, 192 or 256 bits!
#endif


#define BPOLY       0x1b    //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE   16      //!< Block size in number of bytes.
#define EXPANDED_KEY_SIZE (BLOCKSIZE * (ROUNDS + 1))    //!< 176, 208 or 240 bytes.


//#define AES_KEY_HIDE    0x0A    //�����������룬��������aes_key�Ķ���-----���ƽ
#define AES_KEY_HIDE    0x00    //�����Ŀ��������KEY


extern unsigned char aes_encode(unsigned char *key, unsigned char *databuf,
                                unsigned int datalen); //���ݳ���ҪΪ16�ı���,��ԿΪ32Ϊ
extern unsigned char aes_decode(const unsigned char *key, unsigned char *databuf,
                                unsigned int datalen); //���ݳ���ҪΪ16�ı���,��ԿΪ32Ϊ

#endif /* __AES_H */

