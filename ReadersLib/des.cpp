#include "stdafx.h"
#include "des.h"
#include <cstdlib>
#include <string.h>
//#include <stdio.h>
#if (WINDOWS)
	#include <alloc.h>
#endif

//---------------------------------------------------------------------------
DES::UINT2 DES::BYTE_BIT[8] = {
  0200, 0100, 040, 020, 010, 04, 02, 01
};
//---------------------------------------------------------------------------
uint8_t DES::PC1[56] = {
  56, 48, 40, 32, 24, 16,  8,      0, 57, 49, 41, 33, 25, 17,
   9,  1, 58, 50, 42, 34, 26,     18, 10,  2, 59, 51, 43, 35,
  62, 54, 46, 38, 30, 22, 14,      6, 61, 53, 45, 37, 29, 21,
  13,  5, 60, 52, 44, 36, 28,     20, 12,  4, 27, 19, 11,  3
};
//---------------------------------------------------------------------------
uint8_t DES::TOTAL_ROTATIONS[16] = {
  1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28
};
//---------------------------------------------------------------------------
uint8_t DES::PC2[48] = {
  13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9,
  22, 18, 11,  3, 25,  7, 15,  6, 26, 19, 12,  1,
  40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
  43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
};
//---------------------------------------------------------------------------
DES::UINT4 DES::BIG_BYTE[24] = {
  0x800000L, 0x400000L, 0x200000L, 0x100000L,
  0x80000L,  0x40000L,  0x20000L,  0x10000L,
  0x8000L,   0x4000L,   0x2000L,   0x1000L,
  0x800L,    0x400L,    0x200L,    0x100L,
  0x80L,     0x40L,     0x20L,     0x10L,
  0x8L,      0x4L,      0x2L,      0x1L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP1[64] = {
  0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
  0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
  0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
  0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
  0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
  0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
  0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
  0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
  0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
  0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
  0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
  0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
  0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
  0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
  0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
  0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP2[64] = {
  0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
  0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
  0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
  0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
  0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
  0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
  0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
  0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
  0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
  0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
  0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
  0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
  0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
  0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
  0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
  0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP3[64] = {
  0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
  0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
  0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
  0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
  0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
  0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
  0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
  0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
  0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
  0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
  0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
  0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
  0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
  0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
  0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
  0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP4[64] = {
  0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
  0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
  0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
  0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
  0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
  0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
  0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
  0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
  0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
  0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
  0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
  0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
  0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
  0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
  0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
  0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP5[64] = {
  0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
  0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
  0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
  0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
  0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
  0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
  0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
  0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
  0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
  0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
  0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
  0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
  0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
  0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
  0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
  0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP6[64] = {
  0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
  0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
  0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
  0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
  0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
  0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
  0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
  0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
  0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
  0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
  0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
  0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
  0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
  0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
  0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
  0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP7[64] = {
  0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
  0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
  0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
  0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
  0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
  0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
  0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
  0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
  0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
  0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
  0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
  0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
  0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
  0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
  0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
  0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L
};
//---------------------------------------------------------------------------
DES::UINT4 DES::SP8[64] = {
  0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
  0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
  0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
  0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
  0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
  0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
  0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
  0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
  0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
  0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
  0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
  0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
  0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
  0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
  0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
  0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L
};
//---------------------------------------------------------------------------
/*..............................................................................
Fun�o      :	DESCreate
Descricao 	:	Aloca memoria para execucao de criptografia
Entradas		:  key, chave a ser carregada
			   flag, 0=Decripta, 1=Encripta
Retorno  	:	Ponteiro para as subchaves criadas em memoria
===>  Revisoes
05/03/2001  1.00  JASJ  Versao Inicial
..............................................................................*/
uint32_t * DES::DESCreate(const uint8_t *key,int flag)
{
   uint32_t * pkeys;

   // Aloca memoria para armazenar o key sched
   pkeys = (uint32_t *) malloc(128);
   // Efetua a carga de chaves
   DESKey_(pkeys,(uint8_t*) key,flag);
   return pkeys;
}
//---------------------------------------------------------------------------
/*..............................................................................
Fun�o      :	DESLoadKey
Descricao 	:	Carrega chave de criptografia para memoria
Entradas		:  pkeys, ponteiro para a area de memoria das subchaves

			   flag, 0=Decripta, 1=Encripta
Retorno  	:	Ponteiro para as subchaves criadas em memoria
===>  Revisoes
05/03/2001  1.00  JASJ  Versao Inicial
..............................................................................*/
void DES::DESLoadKey(uint32_t *pkeys,const uint8_t *key,int flag)
{
   DESKey_(pkeys,(uint8_t*) key,flag);
}
//---------------------------------------------------------------------------
void DES::DESDestroy(uint32_t *pkeys)
{
   // Libera memoria alocada
   free(pkeys);
   pkeys = 0x0000;
}
//---------------------------------------------------------------------------
void DES::DESCrypt(const uint8_t* inblock, uint8_t* outblock,DESKEYS subkeys)
{
   uint32_t outblk[2];

   Pack_(outblk,(uint8_t*) inblock);
   DESFunction_(outblk,subkeys);
   Unpack_(outblock,outblk);
}
//---------------------------------------------------------------------------
DES::DES(uint8_t * const cipher,const uint8_t *ascblock,const uint8_t * const asckey,uint8_t mode)
{

   if (mode==ENCRYPT)
   {
	  DESKEYS crypt = DESCreate(asckey, mode);
	  DESCrypt(ascblock,cipher,crypt);
	  DESDestroy(crypt);
   }
   else
   {
	  DESKEYS decrypt = DESCreate(asckey, mode);
	  DESCrypt(ascblock,cipher,decrypt);
	  DESDestroy(decrypt);
   }
   return ;
}
//---------------------------------------------------------------------------
void DES::TripleDES(uint8_t *cipher,uint8_t *block, uint8_t *key1, uint8_t* key2, uint8_t mode)
{
   if (mode==ENCRYPT)
   {
	  DES(cipher,block,key1,ENCRYPT);
	  DES(cipher,cipher,key2,DECRYPT);
	  DES(cipher,cipher,key1,ENCRYPT);
   }
   else
   {
	  DES(cipher,block,key1,DECRYPT);
	  DES(cipher,cipher,key2,ENCRYPT);
// Alterado por Fabr�io Rezende em 12/12/2006 18:18 h
    //      DES(cipher,block,key1,DECRYPT);
 	  DES(cipher,cipher,key1,DECRYPT);
   }

  return;
}
//---------------------------------------------------------------------------
void DES::Pack_(UINT4 *into, uint8_t *outof)
{
   Pack(into,outof);
   return;
}
//---------------------------------------------------------------------------
void DES::Unpack_(uint8_t *into, UINT4 *outof)
{
   Unpack(into,outof);
   return;
}
//---------------------------------------------------------------------------
void DES::DESKey_(UINT4 *subkeys, uint8_t *key, int encrypt)
{
   DESKey(subkeys,key,encrypt);
   return;
}
//---------------------------------------------------------------------------
void DES::DESFunction_(UINT4 *block, UINT4 *subkeys)
{
   DESFunction(block,subkeys);
   return;
}
//---------------------------------------------------------------------------
void DES::Pack (UINT4 *into,uint8_t *outof)
{
  *into    = (*outof++ & 0xffL) << 24;
  *into   |= (*outof++ & 0xffL) << 16;
  *into   |= (*outof++ & 0xffL) << 8;
  *into++ |= (*outof++ & 0xffL);
  *into    = (*outof++ & 0xffL) << 24;
  *into   |= (*outof++ & 0xffL) << 16;
  *into   |= (*outof++ & 0xffL) << 8;
  *into   |= (*outof   & 0xffL);
}
//---------------------------------------------------------------------------
void DES::Unpack (uint8_t *into,UINT4 *outof)
{
  //*into++ = (uint8_t)((*outof >> 24) & 0xffL);
  //*into++ = (uint8_t)((*outof >> 24) & 0xffL);
  //*into++ = (uint8_t)((*outof >> 16) & 0xffL);
  //*into++ = (uint8_t)((*outof >>  8) & 0xffL);
  //*into++ = (uint8_t)( *outof++      & 0xffL);
  //*into++ = (uint8_t)((*outof >> 24) & 0xffL);
  //*into++ = (uint8_t)((*outof >> 16) & 0xffL);
  //*into++ = (uint8_t)((*outof >>  8) & 0xffL);
  //*into   = (uint8_t)( *outof        & 0xffL);
  into[0] = (uint8_t)((*outof >> 24) & 0xffL);
  into[1] = (uint8_t)((*outof >> 16) & 0xffL);
  into[2] = (uint8_t)((*outof >>  8) & 0xffL);
  into[3] = (uint8_t)( *outof++      & 0xffL);
  into[4] = (uint8_t)((*outof >> 24) & 0xffL);
  into[5] = (uint8_t)((*outof >> 16) & 0xffL);
  into[6] = (uint8_t)((*outof >>  8) & 0xffL);
  into[7] = (uint8_t)( *outof        & 0xffL);
}
//---------------------------------------------------------------------------
void DES::DESKey (UINT4 subkeys[32],uint8_t key[8],int encrypt)
{
  UINT4 kn[32];
  int i, j, l, m, n;
  uint8_t pc1m[56], pcr[56];

  for (j = 0; j < 56; j++) {
	l = PC1[j];
	m = l & 07;
	pc1m[j] = (uint8_t)((key[l >> 3] & BYTE_BIT[m]) ? 1 : 0);
  }
  for (i = 0; i < 16; i++) {
	m = i << 1;
	n = m + 1;
	kn[m] = kn[n] = 0L;
	for (j = 0; j < 28; j++) {
	  l = j + TOTAL_ROTATIONS[i];
	  if (l < 28)
		pcr[j] = pc1m[l];
	  else
		pcr[j] = pc1m[l - 28];
	}
	for (j = 28; j < 56; j++) {
	  l = j + TOTAL_ROTATIONS[i];
	  if (l < 56)
		pcr[j] = pc1m[l];
	  else
		pcr[j] = pc1m[l - 28];
	}
	for (j = 0; j < 24; j++) {
	  if (pcr[PC2[j]])
		kn[m] |= BIG_BYTE[j];
	  if (pcr[PC2[j+24]])
		kn[n] |= BIG_BYTE[j];
	}
  }
  CookKey (subkeys, kn, encrypt);

  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)pc1m, 0, sizeof (pc1m));
  R_memset ((POINTER)pcr, 0, sizeof (pcr));
  R_memset ((POINTER)kn, 0, sizeof (kn));
}
//---------------------------------------------------------------------------
void DES::CookKey (UINT4 *subkeys,UINT4 *kn,int encrypt)
{
  UINT4 *cooked, *raw0, *raw1;
  int increment;
  unsigned int i;

  raw1 = kn;
  cooked = encrypt ? subkeys : &subkeys[30];
  increment = encrypt ? 1 : -3;

  for (i = 0; i < 16; i++, raw1++) {
	raw0 = raw1++;
	*cooked    = (*raw0 & 0x00fc0000L) << 6;
	*cooked   |= (*raw0 & 0x00000fc0L) << 10;
	*cooked   |= (*raw1 & 0x00fc0000L) >> 10;
	*cooked++ |= (*raw1 & 0x00000fc0L) >> 6;
	*cooked    = (*raw0 & 0x0003f000L) << 12;
	*cooked   |= (*raw0 & 0x0000003fL) << 16;
	*cooked   |= (*raw1 & 0x0003f000L) >> 4;
	*cooked   |= (*raw1 & 0x0000003fL);
	cooked += increment;
  }
}
//---------------------------------------------------------------------------
void DES::DESFunction (UINT4 *block,UINT4 *subkeys)
{
  register UINT4 fval, work, right, left;
  register int round;

  left = block[0];
  right = block[1];
  work = ((left >> 4) ^ right) & 0x0f0f0f0fL;
  right ^= work;
  left ^= (work << 4);
  work = ((left >> 16) ^ right) & 0x0000ffffL;
  right ^= work;
  left ^= (work << 16);
  work = ((right >> 2) ^ left) & 0x33333333L;
  left ^= work;
  right ^= (work << 2);
  work = ((right >> 8) ^ left) & 0x00ff00ffL;
  left ^= work;
  right ^= (work << 8);
  right = ((right << 1) | ((right >> 31) & 1L)) & 0xffffffffL;
  work = (left ^ right) & 0xaaaaaaaaL;
  left ^= work;
  right ^= work;
  left = ((left << 1) | ((left >> 31) & 1L)) & 0xffffffffL;

  for (round = 0; round < 8; round++) {
	work  = (right << 28) | (right >> 4);
	work ^= *subkeys++;
	fval  = SP7[ work        & 0x3fL];
	fval |= SP5[(work >>  8) & 0x3fL];
	fval |= SP3[(work >> 16) & 0x3fL];
	fval |= SP1[(work >> 24) & 0x3fL];
	work  = right ^ *subkeys++;
	fval |= SP8[ work        & 0x3fL];
	fval |= SP6[(work >>  8) & 0x3fL];
	fval |= SP4[(work >> 16) & 0x3fL];
	fval |= SP2[(work >> 24) & 0x3fL];
	left ^= fval;
	work  = (left << 28) | (left >> 4);
	work ^= *subkeys++;
	fval  = SP7[ work        & 0x3fL];
	fval |= SP5[(work >>  8) & 0x3fL];
	fval |= SP3[(work >> 16) & 0x3fL];
	fval |= SP1[(work >> 24) & 0x3fL];
	work  = left ^ *subkeys++;
	fval |= SP8[ work        & 0x3fL];
	fval |= SP6[(work >>  8) & 0x3fL];
	fval |= SP4[(work >> 16) & 0x3fL];
	fval |= SP2[(work >> 24) & 0x3fL];
	right ^= fval;
  }

  right = (right << 31) | (right >> 1);
  work = (left ^ right) & 0xaaaaaaaaL;
  left ^= work;
  right ^= work;
  left = (left << 31) | (left >> 1);
  work = ((left >> 8) ^ right) & 0x00ff00ffL;
  right ^= work;
  left ^= (work << 8);
  work = ((left >> 2) ^ right) & 0x33333333L;
  right ^= work;
  left ^= (work << 2);
  work = ((right >> 16) ^ left) & 0x0000ffffL;
  left ^= work;
  right ^= (work << 16);
  work = ((right >> 4) ^ left) & 0x0f0f0f0fL;
  left ^= work;
  right ^= (work << 4);
  *block++ = right;
  *block = left;
}
//---------------------------------------------------------------------------
void DES::R_memset (POINTER output,int value,unsigned int len)
{
  if (len)
	memset (output, value, len);
}
//---------------------------------------------------------------------------
