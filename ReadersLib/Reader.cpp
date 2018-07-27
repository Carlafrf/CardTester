#include "stdafx.h"
#include "Reader.h"
#include "ReaderImp.h"

#include <iostream>
#include <windows.h>
#include <vector>
#include <winscard.h>


static Reader * getReader(unsigned id);

unsigned Reader::getNumberOfReaders() {
	return ReaderImp::getNumberOfReaders();
}//getNumberOfReaders()

void Reader::initSectionReaders() {
	return ReaderImp::initSectionReaders();
}//initSectionReaders()


std::vector<Reader*> Reader::getReaderList() {
	return ReaderImp::getReaderList();
}//getReaderList()

void Reader::printApdu(ApduMsg apdu) {
	ReaderImp::printApdu(apdu);
}

//void Reader::printAndCheck(ApduResp ret, midiaType card, uint8_t reference) {
//int Reader::printAndCheck(ApduResp ret, int card, uint8_t reference) {
//	return ReaderImp::printAndCheck(ret, card, reference);
//}

Reader::Reader()
{

}