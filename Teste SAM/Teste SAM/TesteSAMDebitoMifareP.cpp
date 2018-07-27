//#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Reader.h"
#include "binAPDU.h"

bool check9000(ApduResp ret_) {//, uint16_t sw = 0x9000) {
							   //	if (ret_.getSW() == sw) return true; //for debug
							   //else return false;
	return (ret_.getSW() == 0x9000);
}

bool isEqual(ApduResp src, char* desired) {
	BinaryData retExpected;
	retExpected.set(desired);
	ApduResp buffer;
	buffer.set(src.getData(), src.getRespDataLen());  //DANGER
	bool equal = src.getInternalRespData().equal(retExpected); //for debug
	return equal;
}

ApduMsg apdu_;
ApduResp ret_;

bool initSAM(Reader * samReader_) {

	std::cout << "\n --------";
	std::cout << "\n Init: SAM initialization:";

	////prepares SAM for test mode
	ApduMsg apdu;

	apdu.reset();
	apdu.set("0002010000");
	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
	if (!check9000(ret_)) return false;

	const char * AuthOff = "568E17CD12F94A63";
	//"SAM test mode"
	BinaryData response(ret_.getInternalRespData());
	BinaryData key;
	key.set(AuthOff);
	response.DESEncrypt(key);
	apdu.reset();
	apdu.set("00020200").addData(response.getData(), 8); //.addData("CCBB");
	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
	if (!check9000(ret_)) return false;

	apdu.reset();
	apdu.set("0000000000");
	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
	if (!check9000(ret_)) return false;

	std::cout << "\n Fim da inicializacao do SAM";
	std::cout << "\n --------";
	return true;
} //initSAM()

//0x56, 0x8E, 0x17, 0xCD, 0x12, 0xF9, 0x4A, 0x63 

//bool initSAM(Reader * samReader_) {
//
//	std::cout << "\n --------";
//	std::cout << "\n Init: SAM initialization:";
//
//	//prepares SAM for test mode
//	ApduMsg apdu;
//
//	apdu.reset();
//	apdu.set("0000000000");
//	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
//	if (!check9000(ret_)) return false;
//
//	apdu.reset();
//	apdu.set("0001010000");
//	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
//	if (!check9000(ret_)) return false;
//
//	apdu.reset();
//	apdu.set("0002010000");
//	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
//	if (!check9000(ret_)) return false;
//
//	apdu.reset();
//	apdu.set("00020200082926C7AD0F801A04");
//	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
//	if (!check9000(ret_)) return false;
//
//	apdu.reset();
//	apdu.set("0000000000");
//	samReader_->CardTransmit(apdu, ret_, Reader::SAM);
//	if (!check9000(ret_)) return false;
//
//	std::cout << "\n Fim da inicializacao do SAM";
//	std::cout << "\n --------";
//	return true;
//} ///initSAM()
//

bool Follow_Auth_AES(Reader * samReader_, Reader * cardReader_, uint8_t sam_key, uint8_t fileID, char *mifare_key) {

	//defining that's about a MIFARE for 0x74
	uint8_t cardType = 0x62; //mifare = 0x62, cipurse = 0xA1

							 //getting uid
	uint8_t * uid;
	int uidsz;
	cardReader_->getUID(uidsz, &uid);

	initSAM(samReader_);

	cardReader_->modeCompatibility();
	//cardReader_->mode14443_4();

	//Define_media -> Data:   0x04 | UID[0:3]  | AID_LEN[1] | AID [0|8..16] | FILEID -> Em caso de não haver AID, não há AID_LEN, apenas 0x00 entre o UID e o fileID
	apdu_.reset();
	apdu_.ins(0x74).p1(0x62).p2(0x80 | sam_key).addData(uidsz).addData(uid, (uint16_t)uidsz).addData("00").addData(fileID);
	//apdu_.ins(0x74).p1(0x62).p2(0x80 | sam_key).addData(uid, (uint16_t)uidsz).addData("00").addData(fileID);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset(); //Comando de Debug: retorna a chave
	apdu_.set("00810100");
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();
	apdu_.set("FE41FEFE").addData(0x76).addData(mifare_key).addData("50"); //Get Challenge
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();//SAM answere challenge
	apdu_.ins(0x76).p1(0X00).p2(0X00).addData((ret_.getRespBin() + 2), 16);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();//Give the ans to the card (auth)
	apdu_.set("FE41FEFE").addData("72").addData((ret_.getRespBin()), 32).addData("50");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();//verify on SAM (update)
	apdu_.set("00780000").addData((ret_.getRespBin() + 2), 16);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	return true;
}

bool First_Auth_AES(Reader * samReader_, Reader * cardReader_, uint8_t sam_key, uint8_t fileID, char *mifare_key) {

	//defining that's about a MIFARE for 0x74
	uint8_t cardType = 0x62; //mifare = 0x62, cipurse = 0xA1

							 //getting uid
	uint8_t * uid;
	int uidsz;
	cardReader_->getUID(uidsz, &uid);

	initSAM(samReader_);

	//cardReader_->modeCompatibility();
	cardReader_->mode14443_4();

	//Define_media -> Data:   0x04 | UID[0:3]  | AID_LEN[1] | AID [0|8..16] | FILEID -> Em caso de não haver AID, não há AID_LEN, apenas 0x00 entre o UID e o fileID
	apdu_.reset();
	apdu_.ins(0x74).p1(0x62).p2(0x80 | sam_key).addData(uidsz).addData(uid, (uint16_t)uidsz).addData("00").addData(fileID);
	//apdu_.ins(0x74).p1(0x62).p2(0x80 | sam_key).addData(uid, (uint16_t)uidsz).addData("00").addData(fileID); //antigo
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset(); //Comando de Debug: retorna a chave
	apdu_.set("00810100");
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();
	apdu_.set("FE41FEFE").addData("0270").addData(mifare_key).addData("0050");// .addData("50"); //Get Challenge
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();//SAM answere challenge
	apdu_.ins(0x76).p1(0X00).p2(0X00).addData((ret_.getRespBin() + 3), 16);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();//Give the ans to the card (auth)
	apdu_.set("FE41FEFE").addData("0372").addData((ret_.getRespBin()), 32).addData("50");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	apdu_.reset();//verify on SAM (update)
	apdu_.set("00780000").addData((ret_.getRespBin()), 37);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	return true;
}

bool Try_AuthSL3(Reader * samReader_, Reader * cardReader_, uint8_t sam_key, uint8_t fileid, uint8_t key) {

	//diversify Key
	uint8_t cardType = 0x62;//mifare = 0x62, cipurse = 0xA1
							//apdu_.reset();
	uint8_t * uid;
	int uidsz;
	cardReader_->getUID(uidsz, &uid);

	initSAM(samReader_);

	//cardReader_->mode14443_4();

	//Define_media -> Data:   0x04 | UID[0:3]  | AID_LEN[1] | AID [0|8..16] | FILEID0000
	apdu_.reset();
	apdu_.ins(0x74).p1(0x62).p2(0x80 | sam_key).addData(uidsz).addData(uid, (uint16_t)uidsz).addData("00").addData(fileid); //.addData("000001");
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	
	apdu_.reset();
	apdu_.set("00810100");
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.reset();
	apdu_.set("FE63FEFE").addData("70").addData(key).addData("9000");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.reset();
	apdu_.ins(0x76).p1(0X00).p2(0X00).addData((ret_.getRespBin() + 2), 16);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);

	apdu_.reset();
	apdu_.set("FE63FEFE").addData("72").addData((ret_.getRespBin()), 32);// .addData("50");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.reset();
	apdu_.set("00780000").addData((ret_.getRespBin() + 2), 32);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

}

bool AuthSL3_block(Reader * samReader_, Reader * cardReader_, uint8_t fileid) {
	
	uint8_t * uid;
	int uidsz;
	uint8_t sam_key_ = fileid % 2;
	cardReader_->getUID(uidsz, &uid);

	apdu_.reset();
	apdu_.ins(0x74).p1(0x62).p2(0x80 | sam_key_).addData(uidsz).addData(uid, (uint16_t)uidsz).addData("00").addData(fileid);
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.reset(); //Comando de Debug: retorna a chave
	apdu_.set("00810100");
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	/*apdu_.reset();
	apdu_.set("FE41FEFE").addData("0270").addData(fileid).addData(0x40).addData("0050");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false; */
	
	apdu_.reset();
	apdu_.set("FE63FEFE").addData("70").addData(fileid).addData(0x40).addData("00");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.reset();//SAM answere challenge
	apdu_.ins(0x76).p1(0X80).p2(0X00).addData("7373737373737373737373737373").addData(fileid).addData("40").addData((ret_.getRespBin() + 2), 16); //+3 for 0x41;
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	//apdu_.reset();//Give the ans to the card (auth)
	//apdu_.set("FE41FEFE").addData("0372").addData((ret_.getRespBin()), 32).addData("50");
	//cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//if (!(check9000(ret_)))
	//	return false;


	apdu_.reset();//Give the ans to the card (auth)
	apdu_.set("FE63FEFE").addData("72").addData((ret_.getRespBin()), 32);
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.reset();//verify on SAM (update)
	apdu_.set("00780000").addData((ret_.getRespBin()) + 2, 32);// + 3 for 0x41;
	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	std::cout << "\n\n  Sector 0x";

	uint8_t sector;
	if (sam_key_) sector = (fileid - 1) / 2; 
	else sector = fileid / 2;
	
	std::cout << Hex::binToHex(&sector, 1)  <<  " authenticated \n";
	return true;
}

bool CsetUID(Reader * samReader_, Reader * cardReader_) {

	//apdu_.set("FD2F0000").addData("FFFFFFFFFFFF");//000000000000
	//cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//if (!(check9000(ret_)))
	//	return false;

	//apdu_.set("FD350000").addData("00");
	//cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//if (!(check9000(ret_)))
	//	return false;

	/*apdu_.set("FE41FEFE").addData("025000FF");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;*/

	apdu_.set("FE26FEFE");// .addData("025000FF");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
	return false;

	/*apdu_.set("FE41FEFE").addData("40FF");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
	*/
	apdu_.set("FE41FEFE").addData("4050");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
/*
	apdu_.set("FE47FEFE").addData("4350");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.set("FE41FEFE").addData("439B");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false; 
*/


/*	apdu_.set("FE47FEFE").addData("024350");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;

	apdu_.set("FE47FEFE").addData("034350");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
*/
	apdu_.set("FE47FEFE").addData("4350");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;
/*
	apdu_.set("FE47FEFE").addData("43");
	cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	if (!(check9000(ret_)))
		return false;*/

	//apdu_.set("47024050");// .addData("024050");
	//cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//if (!(check9000(ret_)))
	//	return false;
	//apdu_.reset();

	//apdu_.set("47034050");// .addData("034050");
	//cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//if (!(check9000(ret_)))
	//	return false;
	//apdu_.reset();
	//apdu_.set("474050");//.addData("4050");
	//cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//if (!(check9000(ret_)))
	//	return false;

}

bool Read_block(Reader * samReader_, Reader * cardReader_, char* block, char* n_blocks, char* mode) {

		char* apdu2prepare = new char[16];
		strcpy(apdu2prepare, mode);
		strcat(apdu2prepare, block);
		strcat(apdu2prepare, n_blocks);
		std::cout << "\n" << apdu2prepare << std::endl;

	//if (strcmp(mode, "35") <= 0) {

		apdu_.reset();
		apdu_.ins(0xB4).p2(0x00).addData(apdu2prepare).le(0x00); // smi = 0x40 for MACed messages
		samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
		if (!(check9000(ret_)))
			return false;

		apdu_.reset(); //Read Enc'ed, Response no mac, CMD Mac'ed
		apdu_.set("FE63FEFE").addData(ret_.get_uint8p(0), ret_.getRespBinLen());
		cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
		if (!(check9000(ret_)))
			return false;
	/*}
	else {
		apdu_.reset(); 
		apdu_.set("FE63FEFE").addData(apdu2prepare);
		cardReader_->CardTransmit(apdu_, ret_, Reader::SAM);
		if (!(check9000(ret_)))
			return false;
	}*/

	bool modeEnc = ((strcmp(mode, "34") == 0) || (strcmp(mode, "30") == 0));
	bool modeEncMac = ((strcmp(mode, "35") == 0) || (strcmp(mode, "31") == 0));

	//if (modeEnc || modeEncMac) {
	//	apdu_.reset(); //decodifica o que foi respondido
	//	apdu_.ins(0xB6).p1(0x00).p2(0x00);
	//	int offset;
	//	modeEnc ? (offset = 1) : (offset = 1);
	//	apdu_.addData(ret_.get_uint8p(1), ret_.getRespBinLen() - offset).le(0x00);
	//	samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
	//	if (!(check9000(ret_)))
	//		return false;
	//}

		apdu_.reset(); //decodifica o que foi respondido
		apdu_.ins(0xB6).p1(0x00).p2(0x00);
		apdu_.addData(ret_.get_uint8p(1), ret_.getRespBinLen() - 1).le(0x00);
		samReader_->CardTransmit(apdu_, ret_, Reader::SAM);
		if (!(check9000(ret_)))
			return false;

	return true;
}

bool Write_block(Reader * samReader_, Reader * cardReader_, char* block, char* data, char* mode) {
	ApduMsg apdu;
	ApduResp ret;


	//vou convencionar como sizeof(data) = 16 bytes = 32 char
	char* apdu2prepare = new char[38];
	strcpy(apdu2prepare, mode); //A3
	strcat(apdu2prepare, block); //0009
	strcat(apdu2prepare, data); //CED0FADA CED0FADA CED0FADA CED0FADA 
	std::cout << "\n" << apdu2prepare << std::endl;

	apdu.reset();
	apdu.ins(0xB4).p2(0x00).addData(apdu2prepare).le(0x00); // smi = 0x40 for MACed messages
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset(); //Read Enc'ed, Response no mac, CMD Mac'ed
	apdu.set("FE63FEFE").addData(ret.get_uint8p(0), ret.getRespBinLen());
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	if (((strcmp(mode, "A1") == 0) || (strcmp(mode, "A3") == 0))) {
		apdu.reset(); //decodifica o que foi respondido
		apdu.ins(0xB6).p1(0x00).p2(0x00);
		apdu.addData(ret.get_uint8p(1), ret.getRespBinLen() - 1).le(0x00);
		samReader_->CardTransmit(apdu, ret, Reader::SAM);
		if (!(check9000(ret)))
			return false;
	}
	return true;
}


