#pragma once
#include "stdafx.h"
#include "Duali.h"

struct CARD_STATUS {

	uint8_t auto_polling;			// 0x01:disable, 0x00:enable
	uint8_t rf_status;				// 0x01:rf on, 0x00:rf off
	uint8_t card_type;				// 0x41: type A, 0x42 : type B, 0x4d : ISO14443 Part3(MiFare),	0x43 : FeilCa, 0x49 : ISO15693, 0xff : No card
	uint8_t max_bit_rates;			//
	uint8_t bit_rates_tx_Max;
	uint8_t bit_rates_tx_current;
	uint8_t bit_rates_rx_Max;
	uint8_t bit_rates_rx_current;
	uint8_t cascade_level;			// Type A:0x01,0x02,0x03 ///até aqui são 9
	uint8_t UID_10B[10];			// for cascade_level = 0x03 caso seja uid_sz = 4, completa o resto com FF 
}; 

Duali::~Duali() {
	deleteMe();
	return;
}

ReaderImp* Duali::testAndCreate(const std::string &ReaderName) {

	ReaderImp* newReader;

	//If the word "Duali" or "DUALi" exists, tests if it's contactelees or not
	if ((ReaderName.find("Duali") != std::string::npos) || (ReaderName.find("DUALi") != std::string::npos)) {

		//std::cout << "Found the manufacturer's name \"Duali\" in: " << ReaderName << std::endl;
		if ((ReaderName.find("Contactless") != std::string::npos)) {
			newReader = new Duali(ReaderName); //new Duali(ReaderName);
		}
		else { newReader = new PCSCreader(ReaderName); } //"we" can decide about what would be here, but later (for the contact duali)
		return newReader;
	}
	else {
		return NULL;
	}
}

void Duali::anticollision() {

	std::cout << "\n \n Init anti-collision sequence" << std::endl;

	//Disable card existence checking
	setCardExistenceCheck(false);

	//" RF->OFF "
	apdu_.reset();
	apdu_.set("FE11FEFE00");
	///Reader::printApdu(apdu_);
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);

	//" RF->ON "
	apdu_.reset();
	apdu_.set("FE10FEFE00");
	///Reader::printApdu(apdu_);
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);

	// REQA 
	apdu_.reset();
	apdu_.set("FE21FEFE00");
	///Reader::printApdu(apdu_);
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);

	//" Anticollision-Select "
	apdu_.reset();
	apdu_.set("FE3DFEFE0000");
	///Reader::printApdu(apdu_);
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);
	
	return;
}

bool Duali::rats() {

	apdu_.reset();
	apdu_.set("FE41FEFE").addData("E08050") .le(0x00);
	//" RATS "
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);
	if (ret_.get_uint8p(0x00)[0] != 0x02) return true;
	else return false;
}

bool Duali::freqCom() {
	apdu_.reset();
	apdu_.set("FE41FEFE04D011005000");
	// " Parada da frequência de comunicação do cartão "
	return CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);
}

/**
*	Note: need to fix the returns of the called functions (Returns false if some of the commands on the called functions fails)
*/
bool Duali::mode14443_4() {
	bool ret;
	anticollision();
	ret = rats();
	freqCom();
	return ret;
}

bool Duali::modeCompatibility()
{
	anticollision();
	return true;
}

void Duali::getStatusCard() {  //uint8_t * uid_, int & uid_sz

	char * uid;
	apdu_.reset();
	apdu_.set("FE17FEFE00");  //não pode ter le, nem igual a 0x00
	Reader::printApdu(apdu_);
	//Caso envie esse comando com o autopolling ligado, ele retorna o UID de 4Bytes

	std::cout << "\n Getting the UID from the card...";
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);
	//ACHAR UMA CONDIÇÃO DE RETORNO
	status = new CARD_STATUS();
	memcpy(status, ret_.get_uint8p(0x01), sizeof(CARD_STATUS)); //ret_.get_uint8p(0x01) offset de 0x01 graças ao retorno de status da leitora

	return;
}

//desfazer essa bagunça com o retorno e os parâmetros
bool Duali::getUID(int &uid_sz, uint8_t ** uid_) {

	if (!status) {
		getStatusCard();
	}
	*uid_ = status->UID_10B;
	char * la = Hex::binToHex(*uid_, 1);
	if (Hex::binToHex(*uid_, 1) == "FF") {
		//erro: o uid não está preenchido
		std::cout << "O UID não foi encontrado: impossível prosseguir com execução \n";
		getchar();
		exit(1);
	}
	la = Hex::binToHex(*uid_ + 9, 1);
	if (!strcmp(Hex::binToHex(*uid_ + 6, 1), "FF")) {
		uid_sz = 4;
	}else 
		if (!strcmp(Hex::binToHex(*uid_ + 9, 1), "FF")){
			uid_sz = 7;
		}else{	
				uid_sz = 10; 
		}

	if (*uid_) return true;
	else return false;
}

/**
*	status = true -> to enable card existence checking (apdu final = 00)
*	status = false -> to disable card existence checking (apdu final = FF)
*/
void Duali::setCardExistenceCheck(bool status) { 
	//Card existence checking on/off
	if (cardExistenceCheck == status) return; //it does not send an unneccessary command, so
	cardExistenceCheck = status;
	apdu_.reset();
	apdu_.cla(0xFE).ins(0x81).p1(0xFE).p2(0xFE);
	apdu_.addData((status) ? "00" : "FF");    //FF : disable, 00: enable card existence checking
	CardTransmit(apdu_, ret_, Reader::MIFARE_PLUS);
	if (status) return;
}//setCardExistenceCheck(bool status) 

void Duali::deleteMe() {
	std::cout << "\n deleteMe():";
	setCardExistenceCheck(true); //enable card existence checking
	return;
}

bool classifyCardExecuted = false;

const int Duali::getCardType() {
	if (!classifyCardExecuted) cardtype_ = classifyCard();
	std::cout << "\n \t Card Type:" << sCardType[cardtype_];
	return cardtype_;

	//return (classifyCardExecuted)? cardtype_ : cardtype_ = classifyCard(); para não printar
}


Reader::cardType Duali::classifyCard() {
	//divide by ATR
	//for (int i = 0; i < (CARD_FINAL - 1); i++) {
	//	std::cout << "   " << sCardType[i] << "\n";
	//}

	classifyCardExecuted = true;
	readerAutomaticMode = DEFAULT_MODE;
	int mediaType = DEFAULT_CARD;
	uint8_t ats_len = 0x00;
	/*
	if (atr[0] == 0x3B) {
		//So ATR (probably) follows the Duali's ATR format

		if ((atr[1] & 0xF0) == 0xF0) {
			if (atr[1] == 0xF0) {
				std::cout << "\n ATR não coopera para teste de midia: ATR tem formato genérico:\n";
				std::cout << "\t	Não é possível conhecer o modo de operação da leitora\n";
			}
			else {
				readerAutomaticMode = MODE_144434; //Será assim quando os teste de cartões 14443-4 estiverem definidos
			}
		}
		else {
			if ((atr[1] & 0xF0) == 0x80) {
				if (atr[1] > 0x84) {
					readerAutomaticMode = MODE_COMPATIBILITY;
				}
				else
					if (atr[1] < 0x84 && atr[1] > 0x81) {
						readerAutomaticMode = MODE_144434;
					}
			}
		}
	}
	else {
		readerAutomaticMode = MODE_144434;
		//THIS WAY, it can test the other modes. It does not depends of ATR anymore.
	}*/
	uint8_t sak;
	readerAutomaticMode = MODE_COMPATIBILITY;
	switch (readerAutomaticMode){
	case MODE_COMPATIBILITY:
		anticollision();
		sak = ret_.getStatusCode(); //get the first byte from the answere of the card (without reader's status code)
		if ((sak & 0x02) == 0x02) { //testa bit 2
			std::cout <<"\n Coisa desconhecida graças ao fato de não terem coragem de fazer uma imagem vetorial ou, ao menos, minimamente legível. Parabéns NXP!\n";
		}
		else { //init. tests -> SAK bit 2 != 1
			if ((sak & 0x08) == 0x08) { // bit 4 YES
				if ((sak & 0x10) == 0x10) { //bit 5 YES
					if ((sak & 0x01) == 0x01) { //bit 1 YES
						return MIFARE_CLASSIC; // MIFARE_CLASSIC_2K
					}
					else {//bit 1 NO
						if ((sak & 0x20) == 0x20) { //bit 6 YES
							return DEFAULT_CARD; // SMART_MX_MIFARE_4K
						}
						else {//bit 6 NO
							if (rats()) { //aplies RATS
								ats_len = 0x00; //like a reset
								ats_len = ret_.getStatusCode();
								if (ats_len <= 0x07) { //there is no historical bytes
									return MIFARE_CLASSIC;// MIFARE_CLASSIC_4K
								}
								else {//there are historical Bytes
									if (ats_len > 0x07) {
										char * card_hb = Hex::binToHex(ret_.get_uint8p((ats_len - 0x07) + 1), (0x07));
										char * plusS_hb = "C1052F2F0035C7";
										char * plusX_hb = "C1052F2F01BCD6";
										if (!strcmp(card_hb, plusS_hb)) {
											return MIFARE_PLUSS_SL1;  //MIFARE_PLUSS_SL1_4K
										}
										else {
											if (!strcmp(card_hb, plusX_hb)) {
												return MIFARE_PLUSX_SL1;  //MIFARE_PLUSX_SL1_4K
											}
											else {
												std::cout << "\n Teste inconclusivo: cod:02" << std::endl;
												return DEFAULT_CARD;
											}
										}
									}
									else {
										std::cout << "Teste inconclusivo: cod:03" << std::endl;
										//erro na comparação (?)
										return DEFAULT_CARD;
									}
								}
							}
							else { //error in RATS
								return MIFARE_CLASSIC;// MIFARE_CLASSIC_4K
							}
						}
					}
				}
				else {//bit 5 NO
					if ((sak & 0x01) == 0x01) { //bit 1 YES
						return MIFARE_CLASSIC;// MIFARE_CLASSIC_MINI
					}
					else {//bit 1 NO
						if ((sak & 0x20) == 0x20) { //bit 6 YES
							return MIFARE_CLASSIC;// SMART_MX_MIFARE_1K
						}
						else {//bit 6 NO
							if (rats()) { //aplies RATS
								ats_len = 0x00; //like a reset
								ats_len = ret_.getStatusCode();
								if (ats_len <= 0x07) { //there is no historical bytes
									return MIFARE_CLASSIC;// MIFARE_CLASSIC_1K
								}
								else { //error on aplication of RATS
									if (ats_len > 0x07) {
										char * card_hb = Hex::binToHex(ret_.get_uint8p((ats_len - 0x07) + 1), (0x07));
										char * plusS_hb = "C1052F2F0035C7";
										char * plusX_hb = "C1052F2F01BCD6";
										//inclusos depois da sugetão do wilton: 09/07/2018
										char * plusSE_hb = "C10521300077C1";
										
										if (!strcmp(card_hb, plusS_hb)) {
											return MIFARE_PLUSS_SL1;  //MIFARE_PLUSS_SL1_2K
										}
										else {
											if (!strcmp(card_hb, plusX_hb)) {
												return MIFARE_PLUSX_SL1;  //MIFARE_PLUSX_SL1_2K
											}
											else {
												if (!strcmp(card_hb, plusSE_hb)) {
													return MIFARE_PLUSSE_SL1;  //INCLUSO POR MINHA CONTA
												}
												else {
													std::cout << "Teste inconclusivo: cod:00" << std::endl;
													return DEFAULT_CARD;
												}
											}
										}
									}
									else {
										std::cout << "Teste inconclusivo: cod:01" << std::endl;
										//erro na comparação(?)
										sCardType[0];
										return DEFAULT_CARD;
									}
								}
							}
							else { //error in RATS
								return MIFARE_CLASSIC;// MIFARE_CLASSIC_1K
							}
						}
					}
				}
			}
			else {// bit 4 NO
				if ((sak & 0x10) == 0x10) { // bit 5 YES
					if ((sak & 0x01) == 0x01) { // bit 1 YES
						return MIFARE_PLUSX_SL2;// MIFARE_PLUSX_SL2_4K;
					}
					else { // bit 1 NO
						return MIFARE_PLUSX_SL2;// MIFARE_PLUSX_SL2_2K;
					}

				}
				else { // bit 5 NO
					if ((sak & 0x20) == 0x20) { // bit 6 YES
						if (rats()) { //aplies RATS
							ats_len = 0x00; //like a reset
							ats_len = ret_.getStatusCode();
							if (ats_len <= 0x07) { //there is no historical bytes
								//get Version
								/*************************/
								return MIFARE_DESFIRE; //Or no NXP MIFARE!
							}
							else {//there are historical Bytes
								if (ats_len > 0x07) {
									char * card_hb_p1 = Hex::binToHex(ret_.get_uint8p((ats_len - 0x07) + 0x01), (0x02)); // offset +1 (reader's status code) 
									char * card_hb_p2 = Hex::binToHex(ret_.get_uint8p((ats_len - 0x07) + 0x05), (0x03)); // offset +5 ( + 1 reader's status code) (+4 the first 4 bytes usuless on hb)
									char * plusS_hb_p1 = "C105";
									char * plusS_hb_p2 = "0035C7";
									char * plusX_hb_p1 = "C105";
									char * plusX_hb_p2 = "01BCD6";
									if ((!strcmp(card_hb_p1, plusS_hb_p1)) && (!strcmp(card_hb_p2, plusS_hb_p2))) {//
										/*************************/
										/*************************/
										/*************************/



										//if(deselect virtual card){
										//return MIFARE_PLUSS_SL3;
										//}
										//else{
										std::cout << "\n MIFARE_PLUSS_SL0 or MIFARE_PLUSS_SL3 - no \"deselect virtual card\" implemented ";
										return MIFARE_PLUSS_SL0;
										//}
									}
									else {
										if ((!strcmp(card_hb_p1, plusX_hb_p1)) && (!strcmp(card_hb_p2, plusX_hb_p2))) {
									
											/*************************/
											/*************************/
											/*************************/
											//if(deselect virtual card){
											//return MIFARE_PLUSX_SL3;
											//}
											//else{
											std::cout << "\n\t MIFARE PLUSX SL0 or SL3 - MIFARE PLUS EV1 SL0 or SL3 \n\t no \"deselect virtual card\" implemented \n";
											return MIFARE_PLUSX_SL0;
											//}
										}
										else {
											std::cout << "\n Teste inconclusivo: cod:06" << std::endl;
											return DEFAULT_CARD;
										}
									}
								}
							}
						}
						else {
							std::cout << "Teste inconclusivo: cod:04" << std::endl;
							//erro na aplicação do RATS
							return DEFAULT_CARD;
						}
					}
					else { // bit 6 NO
						if ((sak & 0x01) == 0x01) { // bit 1 YES
							return DEFAULT_CARD; //TagNPlay ??????
						}
						else { // bit 1 NO
							//o que acontece aqui?
							//diferenciar entre ULC? seria interessante? - nem usamos isso 

							//MIFARE_ULC???
							//MIFARE_UL???
							return DEFAULT_CARD;
						}
					}
				}
			}
		}//terminam os testes do diagrama

		break;
	case MODE_144434:
		return DEFAULT_ISO144434;
		break;

	case DEFAULT_MODE:
	default:
		return DEFAULT_CARD;
		break;
	}
	return DEFAULT_CARD;
}


Reader::SCstatus Duali::checkStatusMsg(ApduResp ret, int card, uint8_t reference){
	uint16_t offset;
	//para a primeira posição:
	offset = 0x00;
	char str[513];

	if (card == Reader::MIFARE_PLUS) {// MIFARE_PLUS == 2

		if (ret.get_uint8(offset) != 0x00) {//deduzindo que sempre se queira que a o comando ao menos chegue ao cartão ou bem sucedido na comunicação com a leitora
			std::cout << "\n\n\t  Comunicacao mal sucedida com o cartao" << std::endl;
			std::cout << "		   Reader Status Code:" << Hex::binToHex(str, ret.get_uint8p(offset), (uint16_t)(1)) << std::endl;
			return Reader::COMUNICATION_ERROR;
		}
		else
			if (reference != NULL && ret.get_uint8(++offset) != reference) {
				std::cout << "\n\n\t  Retorno inseperado do Mifare" << std::endl;
				std::cout << "	   Card Status Code:" << ret.get_uint8p(offset) << std::endl;
				return Reader::MIFARE_P_ERROR;
			}
			else
				return Reader::SUCCESS;

	}
	else
		if (card == Reader::SAM) { //classificação na qual o SAM cairia
			if (ret.getSW1() != (uint8_t)0x90) {
				std::cout << "Comunicação mal sucedida com o cartão" << std::endl;
				std::cout << "		   SAM Status Code:" << ret.get_uint8(offset) << std::endl;
				return Reader::SW_ERROR;
			}
			else
				return Reader::SUCCESS;
		}
		else
			std::cout << "NOT PREPARED FOR THIS MEDIA" << std::endl;
	return Reader::DEFAULT_ERROR;
}
