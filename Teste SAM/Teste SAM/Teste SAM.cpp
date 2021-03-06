// Teste SAM.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "Reader.h"
#include "TesteSAMDebitoMifareP.h"
#include "Cipurse.h"

int main()
{
	enum cards {SAM1, SAM2, SAM3}; //here, list the cards that you want to operate with

	Reader * section;
	section->initSectionReaders();
	std::vector<Reader*> readers = section->getReaderList();
	if (section->getNumberOfReaders() < 2) { //Here, stabilish a min number of cards you have to connect to operate
		std::cout << "\n There isn't enought readers for this operation" << std::endl;
		std::cout << "\n \t finishing program..." << std::endl;
		getchar();
		return 0;
	}
	if (!((readers[SAM1]->ConnectCard(10000)) && (readers[SAM2]->ConnectCard(10000)))){ 
		std::cout << "\t There in not enought connections." << std::endl;
		section->endSection();
		getchar();
		return 0;
	}

	initSAM(section->getReader(SAM1));
	initSAM(section->getReader(SAM2));

	section->endSection();
	getchar();
    return 0;
}












//TESTE READ SIMPLES
/**********************************************************/
//Comando assinado
//AuthSL3_block(samReader, cardReader, 0x04);
//Read_block(samReader, cardReader, "0900", "01", "30");
//Read_block(samReader, cardReader, "0900", "01", "31");
//Read_block(samReader, cardReader, "0900", "01", "32");
//Read_block(samReader, cardReader, "0900", "01", "33");
//Comando sem assinatura
//Read_block(samReader, cardReader, "0900", "01", "34");
//Read_block(samReader, cardReader, "0900", "01", "35");
//Read_block(samReader, cardReader, "0900", "01", "36");
//Read_block(samReader, cardReader, "0900", "01", "37");
/**********************************************************/

//TESTE READ Multiplos Blocos
/**********************************************************/
//AuthSL3_block(samReader, cardReader, 0x04);
//Read_block(samReader, cardReader, "0800", "03", "30");
//Read_block(samReader, cardReader, "0800", "03", "31");
//Read_block(samReader, cardReader, "0800", "03", "32");
//Read_block(samReader, cardReader, "0800", "03", "33");
//Comando sem assinatura
//Read_block(samReader, cardReader, "0800", "03", "34");
//Read_block(samReader, cardReader, "0800", "03", "35");
//Read_block(samReader, cardReader, "0800", "03", "36");
//Read_block(samReader, cardReader, "0800", "03", "37");
/**********************************************************/

//TESTE WRITE SIMPLES
/**********************************************************/
//AuthSL3_block(samReader, cardReader, 0x05);
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE", "A3"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "CACACACACACACACACACACACACACACACA", "A2"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "22002200220022002200220022002200", "A1"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "11001100110011001100110011001100", "A0"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32"); //CA02CA04CA06CA08CA10CA12CA14CA16
/**********************************************************/

//TESTE WRITE Multiplos Blocos
/**********************************************************/
//AuthSL3_block(samReader, cardReader, 0x05);
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "FEFEFEFEFEFEFEFEFEFEFEFEFEFEFEFE", "A3"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "CACACACACACACACACACACACACACACACA", "A2"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "22002200220022002200220022002200", "A1"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32");
//Write_block(samReader, cardReader, "0900", "11001100110011001100110011001100", "A0"); //00112233445566778899001122334455
//Read_block(samReader, cardReader, "0900", "01", "32"); //CA02CA04CA06CA08CA10CA12CA14CA16
/**********************************************************/

//TESTE WRITE ON MFP CONFIGURATION BLOCK
/**********************************************************/
//Try_AuthSL3(samReader, cardReader, 0x00, 0x00, 0x00);
//Write_block(samReader, cardReader, "00B0", "050F0000000000000000000000000000", "A0"); 
/**********************************************************/

//Follow_Auth_AES(samReader, cardReader, 0x05, 0x01, "0490");
//First_Auth_AES(samReader, cardReader, 0x05, 0x03, "0390");
//Try_AuthSL3(samReader, cardReader, 0x81, 0x00, 0x01); //0x9001
//Try_AuthSL3(samReader, cardReader, 0x85, 0x01, 0x04); //0x9004




//CIPURSE TESTS
/***************************************************/
/***************************************************/
/***************************************************/
/***************************************************/

//AuthW_Transport_Key(samReader, cardReader);
//List_EFs_MF(samReader, cardReader);
//Auth_CipurseADF(samReader, cardReader, 0x03);
//format_all(samReader, cardReader);

//Create_ADF4B(samReader, cardReader);