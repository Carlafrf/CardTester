#pragma once
#include "stdafx.h"
#include "PCSCgen.h"

#include <windows.h>
#include <winscard.h>

ReaderImp* PCSCreader::testAndCreate(const std::string &ReaderName) {
	//Here we can put some test about some names, like GEMALTO
	ReaderImp* newReader;
	newReader = new PCSCreader(ReaderName);
	newReader->sname_ = ReaderName;
	return newReader;
}

/*
* Wait for card to be inserted, create connection, gets ATR
* Timeout is in miliseconds
*/
bool PCSCreader::ConnectCard(unsigned long timeout) {
	DWORD rv;
	DWORD dwActiveProtocol;
	//std::cout << "nome: " << sname_ << std::endl;
	//size_t n_readers = Reader::getNumberOfReaders();
	LPSCARD_READERSTATEA lpState = new SCARD_READERSTATEA;// [n_readers];

	memset(lpState, 0, sizeof(SCARD_READERSTATEA));
	lpState->szReader = sname_.c_str();
	timeout *= 20000; //experimental
	DWORD dwTimeout = 1000; //The maximum amount of time, in milliseconds, to wait for an action
	int loopsDone = 0;
	float totalLoops= ((int)timeout / (int)dwTimeout);
	bool look_for_card = true;

	while (look_for_card) {

		rv = SCardGetStatusChangeA(ctx_->hContext_g,	//context
			dwTimeout,			//timeout
			lpState,			//indication of reader's state - it contains only one reader: the object's reader
			(DWORD)1);			//number of readers to watch, the size of lpState 

		switch (rv)
		{
		case SCARD_S_SUCCESS:
		case SCARD_E_TIMEOUT:
			if ((lpState->dwEventState & SCARD_STATE_PRESENT) && (!card_presence)) {

				rv = SCardConnectA(ctx_->hContext_g,
					(LPCSTR)sname_.c_str(), //ctx_->mszReaders_g, 
					SCARD_SHARE_EXCLUSIVE, //SCARD_SHARE_SHARED,
					SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
					&hCard_,
					&dwActiveProtocol);

				if (rv == SCARD_E_NOT_READY) {
					std::cout << "\n Fail: reader is not ready to connect (SCARD_E_NOT_READY) " << std::endl;
					return false;
				}
				else
					if (rv != SCARD_S_SUCCESS) {
						std::cout << "\n Fail : Not connected to the card. Status " << rv << std::endl;
						std::cout << "     press enter to finish..." << std::endl;
						getchar();
						//exit(1);
						return false;
					}

				switch (dwActiveProtocol) {
				case SCARD_PROTOCOL_T0:
					pioSendPci_ = *SCARD_PCI_T0;
					break;
				case SCARD_PROTOCOL_T1:
					pioSendPci_ = *SCARD_PCI_T1;
					break;
				}
				card_presence = true;
				look_for_card = false; //the search is over!
			}
			else {
				if (!loopsDone) {
					std::cout << "\n\t Please, insert card on " << sname_.c_str()  << "..." << std::endl;
				}
				loopsDone++;
				if (loopsDone >= totalLoops) {
					look_for_card = false;
					std::cout << "\n Fail : Timeout! There is no card to connect..." << std::endl;
					//exit(1);
					return false;
				}
				else {
					look_for_card = true; //the search is not over!
				}
				//card_presence = false; //se entrou aqui, já é false
			}
			break;
		default:
			std::cout << "\n \t The reader's status changed for non treated reasons" << std::endl;
			std::cout << "\t SCardGetStatusChangeA returned " << rv << std::endl;
			return false;

			break;
		}
	}
	std::cout << "\n Conected to card"  << std::endl;
	connected = true;
	//já pega o ATR (mas sem tratamento de erro)

	char str[513];
	LPBYTE   pbAttr = NULL;
	DWORD    cByte = SCARD_AUTOALLOCATE;
	SCardGetAttrib(hCard_, SCARD_ATTR_ATR_STRING, (LPBYTE)&pbAttr, &cByte);
	std::cout << "\nATR:  " << Hex::binToHex(str, (uint8_t*)pbAttr, (uint16_t)cByte) << std::endl;
	atr = (uint8_t*)pbAttr;

	//getCardType();
	return true;

}//ConnectCard()

bool PCSCreader::CardTransmit(const  ApduMsg & apdu, ApduResp & ret, int media) {

	if (!card_presence) {
		std::cout << "\n There is no card avaiable for connection " << std::endl;
		return 0;
	}
	char str[513];
	BYTE pbRecv[RECBUFFER];
	DWORD dwRecv = RECBUFFER;

	ret_.reset();
	LONG lReturn;

	apdu_ = apdu;

	lReturn = SCardTransmit(hCard_, &pioSendPci_, apdu.getApduBin(), apdu.getApduBinLen(), NULL, pbRecv, &dwRecv);
	if (lReturn != SCARD_S_SUCCESS)
	{
		std::cout << "\n No Success, Return of transmit: " << Hex::binToHex(str, (uint8_t*)(&lReturn), sizeof(lReturn)) << std::endl;
		std::cout << "----------//-----------------// \n";
		out << "\n No Success, Return of transmit: " << Hex::binToHex(str, (uint8_t*)(&lReturn), sizeof(lReturn)) << std::endl;
		Sleep(1000);
		return false;
	}
	ret_.set(pbRecv, (uint16_t)dwRecv);
	ret = ret_;

	printApdu(apdu);
	if (printAndCheck(ret, media) == Reader::SUCCESS) return true;
	else return false;

}//CardTransmit()

bool PCSCreader::getATR() {

	char	 str[513];
	LPBYTE   pbAttr = NULL;
	DWORD    cByte = SCARD_AUTOALLOCATE;
	LONG     lReturn = SCardGetAttrib(ctx_->hContext_g, SCARD_ATTR_ATR_STRING, (LPBYTE)&pbAttr, &cByte);
	if (lReturn == SCARD_S_SUCCESS) {
		std::cout << "\nATR:  " << Hex::binToHex(str, (uint8_t*)pbAttr, (uint16_t)cByte) << std::endl;
		atr = (uint8_t*)pbAttr;
	}
	else {
		if (ERROR_NOT_SUPPORTED == lReturn)
			std::cout << "Failed SCardGetAttrib Value not supported\n \n";
		else
		{
			// Some other error occurred.
			printf("Failed SCardGetAttrib - %x\n", lReturn);
		}
	}
	return true;
}

int PCSCreader::printAndCheck(const ApduResp ret, int card, uint8_t reference) {
	//in the case there is not an .txt to write 
	if (!out.is_open()) {
		out.open("Test_firstAuth1.txt");
		out << "\n \t  Teste de Autenticação compatibilidade MIFARE classic \n";
	}

	char str[513];

	std::cout << "\n  -->";
	out << "\n  -->";

	for (uint16_t offset = 0; offset < (ret.getRespBinLen()); offset++)
	{
		std::cout << " " << Hex::binToHex(str, ret.get_uint8p(offset), (uint16_t)(1)); //Hex::binToHex(str, response + z, (uint16_t)(1));
		out << " " << Hex::binToHex(str, ret.get_uint8p(offset), (uint16_t)(1));
		if (!((offset + 1) % 16))
		{
			std::cout << "\n" << "     ";
			out << "\n" << "     ";
		}
	}
	std::cout << "\n";
	if((ret.getRespBinLen()) > 0) std::cout << " status:" << Hex::binToHex(str, ret.get_uint8p(0), (uint16_t)(1));
	else std::cout << "          " << std::endl;
	uint16_t tt = ret.getSW();
	std::cout << "\t SW: 0x" << std::hex << ret.getSW(); 
	//std::cout << "\t SW1: 0x" << std::hex << static_cast<int>(ret.getSW1()) << " SW2: 0x" << static_cast<int>(ret.getSW2()); //Aqui vai printar se deu sucesso ou não de acordo com o retorno de checkStatusMsg

	std::cout << "\n ------- \n"; 

	int i = checkStatusMsg(ret, card, reference);
	
	if (i == Reader::SUCCESS) 
	{
		//??
	}
	return i;
}
