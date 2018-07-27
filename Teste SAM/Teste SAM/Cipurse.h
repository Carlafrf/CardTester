#pragma once
#include "Reader.h"
#include "binAPDU.h"

#include "TesteSAMDebitoMifareP.h"


bool AuthW_Transport_Key(Reader * samReader_, Reader * cardReader_);
bool List_EFs_MF(Reader * samReader_, Reader * cardReader_);
bool Auth_CipurseADF(Reader * samReader_, Reader * cardReader_, uint8_t authKey, char* fileID = "E102");
bool format_all(Reader * samReader_, Reader * cardReader_);
bool Create_ADF(Reader * samReader_, Reader * cardReader_);
bool Create_ADF4B(Reader * samReader_, Reader * cardReader_);