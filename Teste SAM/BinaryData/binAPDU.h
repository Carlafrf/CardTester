//#pragma once
#ifndef UTIL_H
#define UTIL_H


#include <stdint.h>
#include <vector>
#include <iostream>
//#include "stdafx.h"
//#include <boost/shared_ptr.hpp>

class BinaryData {
public:
	BinaryData(const uint8_t* data, const uint16_t length);
	BinaryData(const char *hexdata);
	BinaryData(const BinaryData& other);
	BinaryData();
	virtual ~BinaryData();

	BinaryData& reset();
	BinaryData& clearData();
	BinaryData& zero();

	
	BinaryData& forceLength(uint16_t length);
	BinaryData& set(const uint8_t* data, const uint16_t length);
	BinaryData& set(const char *hexdata);
	BinaryData& set(const BinaryData& rhs);
	BinaryData& set(const BinaryData* const rhs);
	BinaryData& add(const char *hexdata);
	BinaryData& add(const uint8_t data);
	BinaryData& add(const uint8_t* data, const uint16_t length);
	BinaryData& add(const BinaryData& rhs);
	BinaryData& addVarIntU64(const uint64_t value);
	BinaryData& operator=(const BinaryData& rhs);
	BinaryData& operator+=(const BinaryData& rhs);
	const BinaryData operator+(const BinaryData& rhs)const;

	bool operator==(const BinaryData& rhs)const;
	bool equal(const BinaryData& rhs)const;
	bool operator!=(const BinaryData& rhs)const;
	bool operator==(const char rhs[])const;
	bool operator!=(const char rhs[])const;
	bool equal(const uint8_t* data, const uint16_t length)const;
	uint8_t  operator[](std::size_t idx) { return m_data[idx]; };
	const uint8_t  operator[](std::size_t idx) const { return m_data[idx]; };

	/**
	 Print the content of m_data and m_length
	 on the @param std::ostream &out
	*/
	void print(std::ostream &out) const;
	std::string    toHex() const;
	std::string&   toHex(std::string &str) const;
	char *         asCharString() const;

	uint16_t       getLength()const { return m_length; }
	const uint8_t* getData() const { return m_data; }
	uint8_t*       getData() { return m_data; }
	uint16_t       getBufferSize()const { return m_allocated; }
	uint8_t        get_uint8(uint16_t offset)const;
	uint8_t*	   get_uint8p(uint16_t offset)const;

///	uint16_t       get_beuint16(uint16_t offset)const;

///	uint64_t       get_beU64(uint16_t& offset)const;
///	uint64_t       get_VarIntU64(uint16_t offset)const;

	bool    DESEncrypt(const BinaryData& key);
	bool    DESDecrypt(const BinaryData& key);

public:
	static uint16_t addVarIntU64(const uint64_t value, uint8_t data[], uint16_t offset);
	static uint64_t get_VarIntU64(const uint8_t data[], uint16_t& offset, unsigned length);

protected:
	void initData(const uint8_t* data, const uint16_t length);
	void allocate(uint16_t length);
	uint8_t* m_data;
	uint16_t m_length;
	uint16_t m_allocated;
	friend class ApduMsg;
	friend class ApduResp;
};
std::ostream &operator<<(std::ostream &out, const BinaryData&f);






class ApduMsg {
public:
	enum apduType { Error = 0, Incomplete = 1, Normal = 2, Extended = 3 };
	static const uint16_t NormalLimit = 255;
	ApduMsg(const uint8_t* data, const uint16_t length);
	ApduMsg(const char *hexdata);
	ApduMsg(const BinaryData& rhs);
	ApduMsg(uint8_t cla, uint8_t ins, uint8_t P1, uint8_t P2);
	ApduMsg();

	ApduMsg& reset();
	ApduMsg& cla(uint8_t CLA);
	ApduMsg& ins(uint8_t INS);
	ApduMsg& p1(uint8_t P1);
	ApduMsg& p2(uint8_t P2);
	ApduMsg& le(uint16_t LE);
	uint8_t  cla()const;
	uint8_t  ins()const;
	uint8_t  p1()const;
	uint8_t  p2()const;
	uint16_t lc()const;
	uint16_t le()const;
	uint16_t hasle()const;

	ApduMsg& set(const char *hexdata);
	ApduMsg& set(const BinaryData& rhs);
	ApduMsg& resetData();
	ApduMsg& addData(const uint8_t data);
	ApduMsg& addData(const char *hexdata);
	ApduMsg& addData(const uint8_t* data, const uint16_t length);
	ApduMsg& addData(const BinaryData& rhs);

	const uint8_t *   getApduData()const;
	uint16_t		  getApduDataLength()const { return m_dataLength; }
	const uint8_t *   getApduBin()const { return m_bin.m_data; };
	uint16_t		  getApduBinLen()const { return m_bin.m_length; }
	const BinaryData& getInternalData()const { return m_bin; }
	bool			  isExtended()const { return m_type == Extended; }
	std::string		  toHex() const;

private:
	BinaryData	  m_bin;
	uint16_t	  m_dataLength;
	uint16_t	  m_le;
	bool          m_hasLe;
	enum apduType m_type;
	ApduMsg&	  init();
	ApduMsg&	  adjustSize();
};



class ApduResp {
public:
	ApduResp(const uint8_t* data, const uint16_t length);
	ApduResp();
	ApduResp& reset();
	ApduResp& set(const char *hexdata);
	ApduResp& set(const BinaryData& rhs);
	ApduResp& set(const uint8_t* data, const uint16_t length);

	uint16_t getSW()const { return m_status; }
	uint8_t  getSW1()const { return (uint8_t)((m_status & 0xFF00) >> 8); }
	uint8_t  getSW2()const { return (uint8_t)(m_status & 0x00FF) ; }
	const uint8_t *		  getData()const { return m_bin.getData(); }
	uint8_t *		  getRespBin()const { return m_bin.m_data; }; //Get the entire binary data
	uint16_t		  getRespBinLen()const { return m_bin.m_length; } //Get the entire binary data lenght
	uint16_t		  getRespDataLen()const { return (m_bin.m_length); } //Get the data lenght except SW 1 and 2
	uint8_t           get_uint8(uint16_t offset)const; 
	uint8_t *		  get_uint8p(uint16_t offset)const;
	const BinaryData& getInternalRespData()const { return m_bin; }

	const uint8_t* getDataOut()const { return (m_bin.getData() + 1); }//For mifare operation on duali (just the internal data without status)
	uint8_t  getStatusCode()const { return get_uint8p(0x01)[0]; } //The first byte. Indicates the status of the transmission.
	//STATUS(Response) Code Definition(uint8_t)

	std::string ResptoHex() const;

private:
	BinaryData m_bin;
	uint16_t  m_status;
	ApduResp&  init();
};





class Hex
{
public:
	static uint8_t* hexToBin(const char * const str, uint16_t* const size);
	static char*	binToHex(const uint8_t* bin, const uint16_t size);
	static uint8_t* hexToBin(uint8_t* const bin, const char * const str, uint16_t* const size);
	static char*    binToHex(char * const str, const uint8_t* bin, const uint16_t size);
	static char*    uint64ToHexBigEndian(char * const str, const uint64_t value, const uint8_t hexSize);
	static char*    uint64ToHexLittleEndian(char * const str, const uint64_t value, const uint8_t hexSize);
	static bool     cmpHexToBin(const char * const str, uint8_t* const bin, const uint16_t  size);

	/* compara uma sequencia binaria com um string em hexa. Se fo utilizado o caracter X na sequencia hexa o mesmo é ignorado */
	static uint8_t* hexPartToBin(const char * const str, int charlength);
	static uint8_t* hexPartToBin(uint8_t* const bin, const char * const str, int charLength);
};




#endif // UTIL_H

