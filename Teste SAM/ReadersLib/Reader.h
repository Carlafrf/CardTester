
#ifndef READER_H
#define READER_H
#pragma once
//#include "stdafx.h"
#include "binAPDU.h"

#include <vector>
#include <string>
#include <iostream>

//Descrição de como se deu o retorno da função para listar as leitoras

/**
*  Abstract class. Factory (?) 
*/
class Reader {
public:

	virtual ~Reader() { return; };

	static Reader * getReader(unsigned id);
	static unsigned getNumberOfReaders();
	static void initSectionReaders();
	static std::vector<Reader*> getReaderList();
	static void printApdu(ApduMsg apdu);
	//static int printAndCheck(ApduResp ret, int card, uint8_t reference = NULL);
//	
	virtual bool mode14443_4() = 0; //forceMode14443_4
	virtual bool modeCompatibility() = 0;
	virtual bool getATR() = 0;
	virtual bool CardTransmit(const ApduMsg & apdu, ApduResp & ret, int media) = 0;
	virtual bool cardPresent()=0;
	virtual bool ConnectCard( unsigned long timeout = 1000) = 0 ;
	virtual void setCardExistenceCheck(bool status) = 0;
	//disconnect
	//reconectcard 

	virtual const std::string & name()=0;
	virtual const int  getCardType() = 0;
	virtual bool getUID(int & uid_sz, uint8_t ** uid_) = 0;


	enum card_type { UNKNOWN, CONTACTLESS, CONTACT };
	enum midiaType { DEFAULT_MIDIA, MIFARE, MIFARE_PLUS, CIPURSE, SAM };
	enum SCstatus { DEFAULT_ERROR, SUCCESS, COMUNICATION_ERROR, MIFARE_P_ERROR, SW_ERROR };
	enum mode { DEFAULT_MODE, MODE_COMPATIBILITY, MODE_144434 };
	enum cardType { DEFAULT_CARD = 0, MIFARE_CLASSIC, MIFARE_PLUSX_SL0, MIFARE_PLUSX_SL1, MIFARE_PLUSX_SL2, MIFARE_PLUSX_SL3, MIFARE_PLUSS_SL0, MIFARE_PLUSS_SL1, MIFARE_PLUSS_SL3, MIFARE_PLUSSE_SL0, MIFARE_PLUSSE_SL1, MIFARE_PLUSSE_SL3, MIFARE_PLUS_EV1, MIFARE_DESFIRE, MIFARE_DESFIRE_EV1, MIFARE_DESFIRE_EV2, CIPURSE_4MOVE_4K, CIPURSE_4MOVE_2K, DEFAULT_ISO144434, CARD_FINAL };
	const char * sCardType[CARD_FINAL] = { "DEFAULT_CARD", "MIFARE_CLASSIC", "MIFARE_PLUSX_SL0", "MIFARE_PLUSX_SL1", "MIFARE_PLUSX_SL2", "MIFARE_PLUSX_SL3", "MIFARE_PLUSS_SL0", "MIFARE_PLUSS_SL1", "MIFARE_PLUSS_SL3", "MIFARE_PLUSSE_SL0", "MIFARE_PLUSSE_SL1", "MIFARE_PLUSSE_SL3", "MIFARE_PLUS_EV1", "MIFARE_DESFIRE", "MIFARE_DESFIRE_EV1", "MIFARE_DESFIRE_EV2", "CIPURSE_4MOVE_4K", "CIPURSE_4MOVE_2K", "DEFAULT_ISO144434" };
                                 	   //{ "DEFAULT_CARD", "MIFARE_CLASSIC", "MIFARE_PLUSX_SL0", "MIFARE_PLUSX_SL1", "MIFARE_PLUSX_SL2", "MIFARE_PLUSX_SL3", "MIFARE_PLUSS_SL0", "MIFARE_PLUSS_SL1", "MIFARE_PLUSS_SL3", "MIFARE_PLUSSE_SL0", "MIFARE_PLUSSE_SL1", "MIFARE_PLUSSE_SL3",  "MIFARE_PLUS_EV1", "MIFARE_DESFIRE", "MIFARE_DESFIRE_EV1", "MIFARE_DESFIRE_EV2", "CIPURSE_4MOVE_4K", "DEFAULT_ISO144434", "CIPURSE_4MOVE_2K" };
	
protected:
	Reader();	
};


#endif /*READER_H*/