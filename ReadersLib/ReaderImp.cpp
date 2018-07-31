
#include "stdafx.h"
#include "Reader.h"
#include "PCSCgen.h"
#include "Duali.h"
#include <iostream>
#include <windows.h>
#include <winscard.h>

//Test to initialize each reader according with the type
typedef ReaderImp* (*TEST_FUNC)(const std::string &ReaderName);

#define READER_TYPE_N (2)

static const TEST_FUNC testList[READER_TYPE_N] = {
	Duali::testAndCreate,
	PCSCreader::testAndCreate
};

ReaderImp* ReaderTypeByName2(const std::string & ReaderName) {
	ReaderImp* newReader;
	for (int i = 0; i < READER_TYPE_N; i++) {
		if ((newReader = (*testList[i])(ReaderName)) != NULL)
			return newReader;
	}
	//in the case that the reader has not been initialized, an error occurred 
	return NULL;
}//ReaderTypeByName2()

//Stabilish the context and initialize the readers
int n_readers;

std::vector<Reader*> readersList;

using namespace std;

READERS_GROUP_DATA *  globalCtx_ = NULL;
READERS_GROUP_DATA *  ReaderImp::ctx_ = NULL;

std::ofstream out;

/**
*	Establish the PCSC context and lists the Smart card readers.
*	In the case it is empty, the function fills the instance of
*	the struct READERS_GROUP_DATA, globalCtx_.
*/
static bool ListReaders() {

	if (globalCtx_) return true; // initialization already done 
	READERS_GROUP_DATA con;
	con.hContext_g = NULL;
	con.mszReaders_g = 0;

	DWORD rt = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &con.hContext_g);
	if (rt != SCARD_S_SUCCESS) {
		std::cout << "\n>> ReaderImp::ListReaders(): " << std::endl;
		std::cout << "ERROR on stabilish the context for PCSC " << std::endl;
		std::cout << "   press enter to finish..." << std::endl;

		getchar();
		exit(EXIT_FAILURE);
	}

	DWORD dwReaders = SCARD_AUTOALLOCATE; //32bit integer
	globalCtx_ = new READERS_GROUP_DATA();
	LPCSTR mszGroups = NULL;
	rt = SCardListReadersA(con.hContext_g, mszGroups, (LPSTR)& con.mszReaders_g, &dwReaders);
	if (rt != SCARD_S_SUCCESS) {

		//std::cout << ">> ReaderImp::ListReaders():" << std::endl;
		std::cout << "\n \tERROR: Cannot list the readers." << std::endl;
		std::cout << "\t Check if the readers are connected" << std::endl;
		std::cout << "\t     press enter to finish..." << std::endl;

		*globalCtx_ = con;

		getchar();
		exit(EXIT_FAILURE);
	}
	else {
		if (con.mszReaders_g > 0) {
			*globalCtx_ = con;
			return true;
		}
		else {
			return false;
		}
	}
}//ListReaders() 

 /**
 *	Calls ListReaders() and then, initialize the readers vector.
 *	Then, it calls the static member of ReaderImp, ReaderTypeByName,
 *	so, it can initialise te instances on the vector properly.
 */
static bool initReaders() {

	if (!ListReaders()) { 
		std::cout << ">> ReaderImp::initReaders():" << std::endl;
		std::cout << "There is no reader to be listed \n";
		std::cout << "   press enter to finish..." << std::endl;

		getchar();
		exit(EXIT_FAILURE);
	}

	ReaderImp* reader;
	n_readers = 0;

	LPSTR pReader;
	std::string readerName;
	pReader = globalCtx_->mszReaders_g;

	while ('\0' != *pReader) {
		//Detaching the names
		readerName.assign((const char *)pReader);
		std::cout << "Reader:" << readerName << std::endl;
		reader = ReaderTypeByName2(readerName); //Here it instantiates an object acording to the reader type (MCR, Duali, PCSCgen...)
		readersList.push_back(reader);
		// Advance to the next value.
		pReader = pReader + strlen((const char*)pReader) + 1;
		n_readers++;
	}
	return true;
}//initReaders();

static Reader * getReader(unsigned id); //to implement

void ReaderImp::initSectionReaders() {
	initReaders();
	ReaderImp::ctx_ = globalCtx_;
	//if any error ocurred it would be interrupted before
	return;
}//initSectionReaders

/*
*	In the case the section has not been initialized "initSectionReaders()"
*	it returns as there wasnt readers (return 0)
*/
unsigned ReaderImp::getNumberOfReaders() {
	return globalCtx_ ? (unsigned)n_readers : 0;
}//getNumberOfReaders()

std::vector<Reader*> ReaderImp::getReaderList() {

	if (n_readers) {
		return readersList;
	}
	else {
		std::cout << ">>>ReaderImp::getReaderList():" << std::endl;
		std::cout << "ERROR: There aren't any readers." << std::endl;
		std::cout << "    press enter to finish..." << std::endl;
		getchar();
		//IMPLEMENT AN ERROR HERE
		exit(1);
	}
}//getReaderList()

void ReaderImp::printApdu(const ApduMsg apdu) {

	if (!out.is_open()) {
		out.open("Test_firstAuth1.txt");
		out << "\n \t  Teste de Autenticação compatibilidade MIFARE classic \n";
	}
	char str[513];
	const uint8_t* apduMsg = apdu.getApduBin();

	std::cout << "\n  <--";
	out << "\n  <--";

	for (int z = 0; z < (apdu.getApduBinLen()); z++) {
		std::cout << " " << Hex::binToHex(str, apduMsg + z, (uint16_t)(1));
		out << " " << Hex::binToHex(str, apduMsg + z, (uint16_t)(1));
		if (!((z + 1) % 16)) {
			std::cout << "\n" << "     ";
			out << "\n" << "     ";
		}
	}
}


//ReaderImp* ReaderImp::ReaderTypeByName(const std::string & ReaderName) {
//
//	ReaderImp* newReader;
//	//If the word "Duali" or "DUALi" exists, tests if it's contactelees or not
//	if((ReaderName.find("Duali") != std::string::npos) || (ReaderName.find("DUALi") != std::string::npos)) {
//		//std::cout << "Found the manufacturer's name \"Duali\" in: " << ReaderName << std::endl;
//		if ((ReaderName.find("Contactless") != std::string::npos)) {
//			newReader = new PCSCreader(ReaderName); //new Duali(ReaderName);
//		}
//		else { newReader = new PCSCreader(ReaderName); }
//	}
//	else { newReader = new PCSCreader(ReaderName);	}
//	return newReader;
//}//ReaderTypeByName(std::string ReaderName)

ReaderImp::~ReaderImp() {
	return;
}

void ReaderImp::endSection() {
	for (int i = 0; i < ReaderImp::getNumberOfReaders(); i++) {
		std::cout << "\n Deleting:" << getReaderList()[i]->name();
		delete(ReaderImp::getReaderList()[i]);
	}
}