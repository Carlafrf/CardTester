#pragma once
#include "PCSCgen.h"

/**
*  Besides there is the contact reader, this class consider just the 
*  contacteless reader.
*/
extern struct CARD_STATUS;


class Duali  : public  PCSCreader {
public:

	Duali(const std::string &ReaderName):PCSCreader(ReaderName){
		//std::cout << "\n instanciando uma Duali \n \n ";
		sname_ = ReaderName;
		return;
	}
	~Duali();
	static ReaderImp* testAndCreate(const std::string &ReaderName);
	bool mode14443_4();
	bool modeCompatibility();
	//bool cardPresent() { return card_presence; }
	const int getCardType();
	void setCardExistenceCheck(bool status);
	bool getUID(int &uid_sz, uint8_t ** uid_);
	Reader::SCstatus checkStatusMsg(ApduResp ret, int card, uint8_t reference = NULL);
	//int getreaderAutomaticMode() {return readerAutomaticMode; };
	//bool CardTransmit(const  ApduMsg & apdu, ApduResp & ret); //implementar aqui uma função que encapsula nos formatos 0x41 e 0x63
protected:
	const int type = CONTACTLESS;
	CARD_STATUS * status;

private:
	int readerAutomaticMode;

	bool cardExistenceCheck = true;
	cardType classifyCard();
	void deleteMe();
	void getStatusCard();
	void anticollision();
	bool rats(); //goes to 14443
	bool freqCom();
};
