#pragma once
#ifndef DES_H
#define DES_H


#define ENCRYPT 1
#define DECRYPT 0
#define ENCRYPTION 1
#define DECRYPTION 0
#define BLOCKSIZE 8

#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif


#include <stdint.h>

//---------------------------------------------------------------------------
class DES
{
	public:

		DES(uint8_t * const cipher,const uint8_t *const ascblock,const uint8_t * const asckey, uint8_t mode);

		//typedef unsigned long *DESKEYS;
		typedef uint32_t* DESKEYS;
		/* UINT2 defines a two byte word */
		typedef uint16_t UINT2;
		//typedef uint16_t unsigned short int UINT2;

		/* UINT4 defines a four byte word */
		//typedef unsigned long int UINT4;
		typedef uint32_t UINT4;

		/* POINTER defines a generic pointer type */
		//typedef unsigned char *POINTER;
		typedef uint8_t* POINTER;

		static UINT2 BYTE_BIT[8];
		static uint8_t  PC1[56];
		static uint8_t  TOTAL_ROTATIONS[16];
		static uint8_t  PC2[48];
		static UINT4 BIG_BYTE[24];

		static UINT4 SP1[64];
		static UINT4 SP2[64];
		static UINT4 SP3[64];
		static UINT4 SP4[64];
		static UINT4 SP5[64];
		static UINT4 SP6[64];
		static UINT4 SP7[64];
		static UINT4 SP8[64];

		static DESKEYS DESCreate(const uint8_t *key,int flag);
		static void DESDestroy(DESKEYS pkeys);
		static void DESCrypt(const uint8_t* inblock, uint8_t* outblock,DESKEYS subkeys);
		static void DESLoadKey(DESKEYS pkeys,const uint8_t *key,int flag);
		static void TripleDES(uint8_t *cipher,uint8_t *block, uint8_t *key1, uint8_t *key2, uint8_t mode);

		static void Pack_(UINT4 *into, uint8_t *outof);
		static void Unpack_(uint8_t *into, UINT4 *outof);
		static void DESKey_(UINT4 *subkeys, uint8_t *key, int encrypt);
		static void DESFunction_(UINT4 *block, UINT4 *subkeys);

		static void Pack (UINT4 *into,uint8_t *outof);
		static void Unpack (uint8_t *into,UINT4 *outof);
		static void DESKey (UINT4 subkeys[32],uint8_t key[8],int encrypt);
		static void CookKey (UINT4 *subkeys,UINT4 *kn,int encrypt);
		static void DESFunction (UINT4 *block,UINT4 *subkeys);

		static void Unpack PROTO_LIST ((uint8_t *, UINT4 *));
		static void Pack PROTO_LIST ((UINT4 *, uint8_t *));
		static void DESKey PROTO_LIST ((UINT4 *, uint8_t *, int));
		static void CookKey PROTO_LIST ((UINT4 *, UINT4 *, int));
		static void DESFunction PROTO_LIST ((UINT4 *, UINT4 *));
		static void R_memset (POINTER output, int value,unsigned int len);
};
//---------------------------------------------------------------------------
#endif
