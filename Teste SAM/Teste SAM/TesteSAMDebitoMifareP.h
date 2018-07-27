#pragma once

#include "Reader.h"
#include "binAPDU.h"

bool check9000(ApduResp ret_);

bool isEqual(ApduResp src, char* desired);

bool initSAM(Reader * samReader_);

bool Follow_Auth_AES(Reader * samReader_, Reader * cardReader_, uint8_t sam_key, uint8_t fileID, char * mifare_key);

bool First_Auth_AES(Reader * samReader_, Reader * cardReader_, uint8_t sam_key, uint8_t fileID, char * mifare_key);

bool Try_AuthSL3(Reader * samReader_, Reader * cardReader_, uint8_t sam_key, uint8_t fileid, uint8_t key);

bool CsetUID(Reader * samReader_, Reader * cardReader_);

bool AuthSL3_block(Reader * samReader_, Reader * cardReader_, uint8_t fileid);

bool Read_block(Reader * samReader_, Reader * cardReader_, char* block, char* n_blocks, char* mode);

bool Write_block(Reader * samReader_, Reader * cardReader_, char* block, char* data, char* mode);

