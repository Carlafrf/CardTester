#pragma once 
#include "Reader.h" 
#include <winscard.h>

#include <fstream>


typedef struct READERS_GROUP_DATA_
{
	SCARDCONTEXT hContext_g = NULL;
	LPSTR mszReaders_g;
}READERS_GROUP_DATA;

extern std::ofstream out;

/**
*  Class that represents the implementation of the class Reader and isolate that. 
*  ReaderImp is responsable for organize the reader types each one by their names 
*  (on the Vector of readers).
*/
class ReaderImp : public  Reader { 
public:
	ReaderImp()
		: card_presence(false), connected(false), cardtype_(DEFAULT_CARD){ return; }
	~ReaderImp();

	bool cardPresent() { return card_presence; }
	const std::string & name() { return sname_; } 
	bool CardTransmit(const  ApduMsg & apdu, ApduResp & ret, int media) = 0;
	bool getUID(int & uid_sz, uint8_t ** uid_) = 0;
	
	static void initSectionReaders();
	static unsigned getNumberOfReaders();
	static std::vector<Reader*> getReaderList();
	static void printApdu(ApduMsg apdu);
	//static int printAndCheck(ApduResp ret, int card, uint8_t reference = NULL);

	static READERS_GROUP_DATA *  ctx_;
	cardType cardtype_;
	bool connected;
	std::string sname_;



protected: 
	bool card_presence;
};