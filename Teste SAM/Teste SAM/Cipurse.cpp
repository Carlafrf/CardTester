#pragma once
#include "Reader.h"
#include "binAPDU.h"

#include "TesteSAMDebitoMifareP.h"

ApduMsg apdu;
ApduResp ret;
	uint8_t * uid;
	int uidsz;

bool AuthW_Transport_Key(Reader * samReader_, Reader * cardReader_) {

	//diversify Key
	uint8_t cardType = 0xA1; //mifare = 0x62, cipurse = 0xA1
	
	cardReader_->getUID(uidsz, &uid);

	apdu.reset();

	//select
	apdu.reset();
	apdu.ins(0xA4).addData("3F00");
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	//define media
	apdu.reset();
	apdu.set("0074A100").addData(uid, (uint16_t)uidsz);
	if (uidsz == 0x04) { apdu.addData("000000"); }
	apdu.addData("3F0000000000000000");
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	//get challenge
	apdu.reset();
	apdu.set("0084000016");
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	char* key = "73737373737373737373737373737373";

	apdu.reset();
	apdu.set("00768000").addData(key).addData((ret.getRespBin()), 22);
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset();
	apdu.set("00820001").addData((ret.getRespBin()), 38).le(0x10);
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset();
	apdu.set("00780000").addData((ret.getRespBin()), 16);
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	return true;
}

bool List_EFs_MF(Reader * samReader_, Reader * cardReader_) {

	if(!AuthW_Transport_Key(samReader_, cardReader_)) return false;

	apdu.reset();
	apdu.ins(0xA4).addData("2FF1"); //seleciona FILE com informações do ADF
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset();
	apdu.ins(0xB0).le(0xFF);//Lê arquivo com informações do ADF
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	if ((ret.getRespBinLen()) < 3) {
		std::cout << "No files under this directory";
		return 0;
	}
	char str[513];

	std::cout << " Files Under the Master File: \n";
	for (uint16_t i = 0; i < (ret.getRespBinLen()); i++) { /// (uint16_t)0x02
		std::cout << "\n    File/ADF:" << Hex::binToHex(str, ret.get_uint8p(i), (uint16_t)(2)) << "\n";
		i += 2;
		std::cout << "\t    SFID:" << Hex::binToHex(str, ret.get_uint8p(i), (uint16_t)(1));
		std::cout << "      FD:" << Hex::binToHex(str, ret.get_uint8p(++i), (uint16_t)(1)) << "\n";
	}

	return true;

}

bool Auth_CipurseADF(Reader * samReader_, Reader * cardReader_, uint8_t authKey, char* fileID) {

	//diversify Key
	uint8_t cardType = 0xA1; //mifare = 0x62, cipurse = 0xA1
	uint8_t * uid;
	int uidsz;
	initSAM(samReader_);

	cardReader_->getUID(uidsz, &uid);
	
	apdu.reset(); //selecting the master file
	apdu.ins(0xA4).addData("3F00");
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	apdu.reset();
	apdu.ins(0xA4).addData(fileID);
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	//Os 2 possíveis AIDs
	char * aid = "636973FFFFC10000FF";//cartão2
	//char * aid = "646973FFFFC10000FF";//cartão1
	apdu.reset();

	//apdu.ins(0x74).p1(0xA1).p2(0x03).addData(uid, (uint16_t)uidsz).addData(aid);// comando antigo
	apdu.ins(0x74).p1(0xA1).p2(authKey).addData(uidsz).addData(uid, (uint16_t)uidsz).addData("09").addData(aid);// .addData("03"); //comando novo
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	
	apdu.set("00810100");
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
		
	apdu.reset();
	apdu.set("0084000016");
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset();
	apdu.set("00760000").addData((ret.getRespBin()), 22);
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset();
	apdu.set("00820003").addData((ret.getRespBin()), 38).le(0x10);
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	apdu.reset();
	apdu.set("00780000").addData((ret.getRespBin()), 16);
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
}

bool format_all(Reader * samReader_, Reader * cardReader_) {

	//AuthW_Transport_Key(samReader_, cardReader_);

	std::cout << "\nFormating all...";
	apdu.reset();
	apdu.cla(0X80).ins(0xFC).addData("436F6E6669724D"); //Cmd data: default
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	return true;

}//Format All

bool Create_ADF(Reader * samReader_, Reader * cardReader_)
{
	if (!AuthW_Transport_Key(samReader_, cardReader_)) return false;
	//diversify key to create ADF
	std::cout << "\n \t \t Criacao do ADF para 4K " << std::endl;
	char * aid_adf = "636973FFFFC10000FF";
	//char * aid_adf = "646973FFFFC10000FF";

	apdu.reset();
	uint16_t adf_id_1 = 0xE1;
	uint16_t adf_id_2 = 0x02;
	apdu.ins(0xB8).p1(0x23).p2(0x84); //P1: indicating "CREATE_FILE" P2: SMI
	apdu.addData("0300E000006C").addData("3840");
	/*********************************************************************************************\
	*  03 =  CIPURSE™V2.R2.0																	 *
	*  00 E0 00 00 6C = (CLA INS P1 P2 Lc)														 *
	*  FileDescriptor and Type: 38H for application DF (ADF), 40H indicating that is a cipurse S *
	\*********************************************************************************************/
	apdu.addData(adf_id_1).addData(adf_id_2);
	//Pode haver um problema aqui com o "Encryption Key No [1] (00H, other values are interpreted as error)	(Not present for version indicator 03H)"
	apdu.addData("030303").addData("0000").addData("00FFFFFF");
	apdu.addData("021009021009021009");
	/***************************************\
	*	03 03 03 = (#nEFs SFIDs keys#n)     *
	*	00 00 = SMR	 |	00 FF FF FF = ART   *
	*	02 10 09 02 10 09 02 10 09 = KeySet *
	\***************************************/
	apdu.addData("620B8409").addData(aid_adf).addData("C805FFFFFFFFFF");
	apdu.addData(uid, (uint16_t)uidsz).addData(aid_adf).addData("0001").addData("0101"); // Chave 01 DO *******SAM*******
	apdu.addData(uid, (uint16_t)uidsz).addData(aid_adf).addData("0002").addData("0101"); // Chave 02
	apdu.addData(uid, (uint16_t)uidsz).addData(aid_adf).addData("0003").addData("0101"); // Chave 03 
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	apdu.reset();// create ADF	
	apdu.cla(0x04).ins(0xE0).p1(0x00).p2(0x00).addData(0x84);
	apdu.addData(ret.get_uint8p((uint16_t)0), ret.getRespDataLen()).le(0x00);
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	apdu.reset();//verify
	apdu.ins(0xB6).p1(0x03).p2(0x00).addData(ret.get_uint8p((uint16_t)0), ret.getRespBinLen()).addData(ret.getSW1()).addData(ret.getSW2()); // p2 = key #n  <<=================================================
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	return 0;
}

bool Create_ADF4B(Reader * samReader_, Reader * cardReader_)
{

	if (!AuthW_Transport_Key(samReader_, cardReader_)) return false;
	//diversify key to create ADF
	std::cout << "\n \t \t Criacao do ADF para 4K " << std::endl;
	char * aid_adf = "636973FFFFC10000FF";
	//char * aid_adf = "646973FFFFC10000FF";

	apdu.reset();
	uint16_t adf_id_1 = 0xE1;
	uint16_t adf_id_2 = 0x02;
	apdu.ins(0xB8).p1(0x23).p2(0x84); //P1: indicating "CREATE_FILE" P2: SMI
	apdu.addData("0300E000006C").addData("3840");
	/*********************************************************************************************\
	*  03 =  CIPURSE™V2.R2.0																	 *
	*  00 E0 00 00 6C = (CLA INS P1 P2 Lc)														 *
	*  FileDescriptor and Type: 38H for application DF (ADF), 40H indicating that is a cipurse S *
	\*********************************************************************************************/
	apdu.addData(adf_id_1).addData(adf_id_2);
	//Pode haver um problema aqui com o "Encryption Key No [1] (00H, other values are interpreted as error)	(Not present for version indicator 03H)"
	apdu.addData("030303").addData("0000").addData("00FFFFFF");
	apdu.addData("021009021009021009");
	/***************************************\
	*	03 03 03 = (#nEFs SFIDs keys#n)     *
	*	00 00 = SMR	 |	00 FF FF FF = ART   *
	*	02 10 09 02 10 09 02 10 09 = KeySet *
	\***************************************/
	apdu.addData("620B8409").addData(aid_adf).addData("C805FFFFFFFFFF");
	apdu.addData(uid, (uint16_t)uidsz).addData("000000").addData(aid_adf).addData("0001").addData("0101"); // Chave 01 DO *******SAM*******
	apdu.addData(uid, (uint16_t)uidsz).addData("000000").addData(aid_adf).addData("0002").addData("0101"); // Chave 02
	apdu.addData(uid, (uint16_t)uidsz).addData("000000").addData(aid_adf).addData("0003").addData("0101"); // Chave 03 
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	apdu.reset();// create ADF	
	apdu.cla(0x04).ins(0xE0).p1(0x00).p2(0x00).addData(0x84);
	apdu.addData(ret.get_uint8p((uint16_t)0), ret.getRespDataLen()).le(0x00);
	cardReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;
	apdu.reset();//verify
	apdu.ins(0xB6).p1(0x03).p2(0x00).addData(ret.get_uint8p((uint16_t)0), ret.getRespBinLen()).addData(ret.getSW1()).addData(ret.getSW2()); // p2 = key #n  <<=================================================
	samReader_->CardTransmit(apdu, ret, Reader::SAM);
	if (!(check9000(ret)))
		return false;

	return 0;
}




