#pragma once
#include "ReaderImp.h"

#define RECBUFFER (258)

class PCSCreader : public  ReaderImp {

public:

	PCSCreader(const std::string &ReaderName){
		sname_ = ReaderName;
		return;
};

	static ReaderImp* testAndCreate(const std::string &ReaderName);

	//bool cardPresent() { return card_presence; }
	bool ConnectCard(unsigned long timeout=1000);
	bool getATR();
	bool CardTransmit(const  ApduMsg & apdu, ApduResp & ret, int media);

	//virtual void deleteMe() { return; };
	
	//const int getCardType() { return ;	};
	virtual bool mode14443_4() { return false; };
	virtual bool modeCompatibility() { return false; };
 	virtual bool getUID(int & uid_sz, uint8_t ** uid_) { return uid_; };
	virtual SCstatus checkStatusMsg(ApduResp ret, int card, uint8_t reference) { return SUCCESS; };
	virtual const int getCardType() {
		std::cout << "\n Cannot classify card: The program does not recognize Reader's specific commands.\n";
		return Reader::DEFAULT_CARD; 
	};
	virtual void setCardExistenceCheck(bool status) { return; };


protected:
	
	//const int type = CONTACT;
	SCARDHANDLE  hCard_;
	SCARD_IO_REQUEST pioSendPci_;
	//bool card_;
	uint8_t * uid_;
	uint8_t uid_sz;
	uint8_t * atr;
	uint8_t * status;
	ApduMsg apdu_;
	ApduResp ret_;
private:
	int printAndCheck(const ApduResp ret, int card, uint8_t reference = NULL);

};