                                                                                                                                                                                                                                                                                                      
#include "stdafx.h"
#include "binAPDU.h"
#include "des.h"
//#include "Util/sha256-alg.h"
#include <string.h>
#include <stdlib.h>


#define NibbleToAsciiHex(nb) ((char)(((nb)>9)?(((nb)-10)%6)+'A':((nb)+'0')))
#define AsciiHexToNibble(ch) ((uint8_t)( ((ch)>='a') ? (((ch)-'a')%6)+10:(((ch)>='A')?(((ch)-'A')%6)+10:((ch)-'0')%10) ))
const static char tbl[] = "0123456789ABCDEF";

uint8_t* Hex::hexToBin(uint8_t* const bin, const char * const str, uint16_t* const size) {
	uint8_t*dptr;
	if ((!bin) || (!size) || (!str) || (!str[0])) return NULL;
	const char*nh, *nl;
	for (nh = str, nl = nh + 1, dptr = bin; (*nh) && (*nl); nl = (nh = nl + 1) + 1)
		*dptr++ = (AsciiHexToNibble(*nh) << 4) | AsciiHexToNibble(*nl);
	(*size) = dptr - bin;
	return bin;
}

uint8_t* Hex::hexPartToBin(uint8_t* const bin, const char * const str, int charLength) {
	uint8_t*dptr;
	if ((!bin) || (charLength<2) || (!str) || (!str[0])) return NULL;
	const char*nh, *nl;
	int size = charLength / 2;
	for (nh = str, nl = nh + 1, dptr = bin; (*nh) && (*nl) && (size); nl = (nh = nl + 1) + 1, size--)
		*dptr++ = (AsciiHexToNibble(*nh) << 4) | AsciiHexToNibble(*nl);
	if (size)memset(dptr, 0, size);
	return bin;
}

char* Hex::binToHex(char * const str, const uint8_t* bin, const uint16_t size) {
	if ((!size) || (!bin) || (!str)) return NULL;
	char *ptr = str;
	for (int i = 0; i<size; i++) {
		*ptr++ = tbl[(unsigned)((*bin) >> 4) & 0x000F];
		*ptr++ = tbl[(unsigned)((*bin++)) & 0x000F];
		//uint8_t idx=((*bin  )>>4)&0x0F;*ptr=tbl[idx]; ptr++;idx=(*bin)&0x0F; bin++;*ptr=tbl[idx];ptr++;
	}
	*ptr = 0;
	return str;
}

/* compara uma sequencia binaria com um string em hexa. Se fo utilizado o caracter X na sequencia hexa o mesmo é ignorado */
bool Hex::cmpHexToBin(const char * const str, uint8_t* const bin, const uint16_t  size) {
	if ((!str) || (!str[0])) { /* hex vazio */
		return size == 0; /* binario vazio */
	}
	else if (size == 0) return false;
	/* existe dados nos dois elementos */
	const char*nh, *nl;
	uint8_t*bptr, *endptr;
	for (nh = str, nl = nh + 1, bptr = bin, endptr = bin + size; (*nh) && (*nl) && (bptr<endptr); nl = (nh = nl + 1) + 1, bptr++) {
		if ((*nh != 'x') && (*nh != 'X')) if (AsciiHexToNibble(*nh) != (*bptr) >> 4) return false;
		if ((*nl != 'x') && (*nl != 'X')) if (AsciiHexToNibble(*nl) != ((*bptr) & 0x0f)) return false;
	}
	return (bptr >= endptr) && (!*nh);
}

char* Hex::uint64ToHexBigEndian(char * const str, const uint64_t value, const uint8_t hexSize) {
	if ((!hexSize) || (!str)) return NULL;
	char *ptr = str + (2 * hexSize);
	*ptr-- = 0;
	uint64_t v = value;
	for (int i = 0; i<hexSize; i++) {
		*ptr-- = tbl[v & 0x0F];
		v >>= 4;
		*ptr-- = tbl[v & 0x0F];
		v >>= 4;
	}
	return str;
}
char* Hex::uint64ToHexLittleEndian(char * const str, const uint64_t value, const uint8_t hexSize) {
	if ((!hexSize) || (!str)) return NULL;
	char*ptr = str;
	uint64_t v = value;
	for (int i = 0; i<hexSize; i++) {
		ptr[1] = tbl[v & 0x0F];
		v >>= 4;
		ptr[0] = tbl[v & 0x0F];
		v >>= 4;
		ptr += 2;
	}
	*ptr = 0;
	return str;
}

uint8_t* Hex::hexToBin(const char * const str, uint16_t* const size) {
	uint8_t* auxBuffer;
	if ((!size) || (!str) || (!str[0]) || ((auxBuffer = (uint8_t*)new uint8_t[(strlen(str) / 2)]) == NULL)) return NULL;
	return hexToBin(auxBuffer, str, size);
}

uint8_t* Hex::hexPartToBin(const char * const str, int charlength) {
	uint8_t* auxBuffer;
	if ((charlength<2) || (!str) || (!str[0]) || ((auxBuffer = (uint8_t*)new uint8_t[charlength / 2]) == NULL)) return NULL;
	return hexPartToBin(auxBuffer, str, charlength);
}
char* Hex::binToHex(const uint8_t* bin, const uint16_t size) {
	char *buffer;
	if ((!size) || (!bin) || ((buffer = (char*)new char[1 + 2 * size]) == NULL)) return NULL;
	return binToHex(buffer, bin, size);
}

/*==================================================================*/

// uint8_t BinaryData::checkSum(const uint8_t buf[], const uint16_t size) {
//	uint32_t sum = 0;
//	for (uint16_t i = 0; i<size;)sum += buf[i++];
//	return (~((int8_t)(sum & 0x000000FF))) + 1;
//}
//
//uint8_t BinaryData::checkSum()const {
//	return checkSum(m_data, m_length);
//}



BinaryData& BinaryData::reset() {   
	m_allocated = m_length = 0;
	delete[] m_data;
	m_data = NULL;
	return *this;
}
BinaryData& BinaryData::forceLength(const uint16_t length) {
	allocate(length);
	m_length = length;
	return *this;
}

BinaryData& BinaryData::zero() {
	memset(m_data, 0, m_allocated);
	return *this;
}

BinaryData& BinaryData::clearData() {
	memset(m_data, 0, m_allocated);
	m_length = 0;
	return *this;
}

/*
l> 8  && l <=16   16
l>16  && l <=32   32
l>32  && l <=64   64
l>64  && l <=128  128
l>128 && l <=256  256
*/
/** alocacao minima de 8 bytes **/
void BinaryData::allocate(uint16_t length) {
	if (length <= m_allocated) return;
	for (m_allocated = 8; m_allocated<length; m_allocated <<= 1);  //"m_allocated <<= 1"  =  set m_allocated to itself shifted by one bit to the right
	uint8_t* tmp = m_data;
	m_data = new uint8_t[m_allocated];
	if (tmp) {
		if (m_length)memcpy(m_data, tmp, m_length);
		delete[] tmp;
	}
}

BinaryData& BinaryData::add(const uint8_t* data, const uint16_t length) {
	if (length) {
		allocate(m_length + length);
		memcpy(m_data + m_length, data, length);
		m_length += length;
	}
	return *this;
}

BinaryData& BinaryData::add(const uint8_t data) { //eu parei aqui para saber como inicializa a classe
	allocate(m_length + 1);
	m_data[m_length] = data;
	m_length++;
	return *this;
}

BinaryData& BinaryData::set(const uint8_t* data, const uint16_t length) {
	m_length = 0;
	return add(data, length);
}
BinaryData& BinaryData::set(const BinaryData& rhs) {
	return set(rhs.m_data, rhs.m_length);
}
BinaryData& BinaryData::set(const BinaryData* const rhs) {
	return set(rhs->m_data, rhs->m_length);
}
BinaryData& BinaryData::add(const BinaryData& rhs) {
	return add(rhs.m_data, rhs.m_length);
}
BinaryData& BinaryData::set(const char *hexdata) {
	m_length = 0;
	return add(hexdata);
}

BinaryData& BinaryData::add(const char *hexdata) {
	if ((hexdata) && (*hexdata)) {
		allocate(m_length + (strlen(hexdata) / 2));
		uint16_t length;
		Hex::hexToBin(&(m_data[m_length]), hexdata, &length);
		m_length += length;
	}
	return *this;
}

BinaryData::BinaryData(const char * hexData)
	:m_data(NULL), m_length(0), m_allocated(0) {
	add(hexData);
}

BinaryData::BinaryData(const uint8_t* data, const uint16_t length)
	: m_data(NULL), m_length(0), m_allocated(0) {
	add(data, length);
}

BinaryData::BinaryData(const BinaryData& other)
	: m_data(NULL), m_length(0), m_allocated(0) {
	add(other.m_data, other.m_length);
}

//BinaryData::BinaryData(const uint16_t length)
//:m_data(NULL),m_length(0),m_allocated(0){
//    allocate(length);
//}

BinaryData::BinaryData()
	:m_data(NULL), m_length(0), m_allocated(0) {}

BinaryData& BinaryData::operator=(const BinaryData& rhs) {
	if (this != &rhs)
		set(rhs);
	return *this;
}

BinaryData::~BinaryData() {
	if (m_data != NULL) {
		delete[] m_data;
		m_data = NULL;
	}
}

void BinaryData::print(std::ostream &out) const
{
	char* hex = Hex::binToHex(m_data, m_length);
	if (!hex) return;
	out << hex;
	delete[] hex;
}

std::string& BinaryData::toHex(std::string &str) const
{
	char* hex = Hex::binToHex(m_data, m_length);
	if (hex) str = hex;
	delete[] hex;
	return str;
}

std::string BinaryData::toHex() const
{
	std::string aux;
	toHex(aux);
	return aux;
}

char * BinaryData::asCharString() const {
	char * buffer;
	if ((!m_length) ||
		((buffer = new char[m_length + 1]) == NULL))
		return NULL;
	memcpy(buffer, m_data, m_length);
	buffer[m_length] = 0;
	return buffer;
}

std::ostream &operator<<(std::ostream &out, const BinaryData&f) {
	f.print(out);
	return out;
}

BinaryData& BinaryData::operator+=(const BinaryData &rhs) {
	return add(rhs);
}
const BinaryData BinaryData::operator+(const BinaryData &rhs) const {
	return BinaryData(*this) += rhs;
}
bool BinaryData::operator==(const BinaryData &rhs) const {
	return (this->m_length == rhs.m_length) && (memcmp(this->m_data, rhs.m_data, this->m_length) == 0);
}
bool BinaryData::equal(const uint8_t* data, const uint16_t length)const {
	return (this->m_length == length) && (memcmp(this->m_data, data, this->m_length) == 0);
}
//begin @carla
bool BinaryData::equal(const BinaryData& rhs)const {
	return this->equal(rhs.getData(), rhs.getLength());
}
//end @carla

bool BinaryData::operator!=(const BinaryData &rhs) const {
	return !(*this == rhs);
}

bool BinaryData::operator==(const char rhs[])const {
	return Hex::cmpHexToBin(rhs, this->m_data, this->m_length);
}
bool BinaryData::operator!=(const char rhs[])const {
	return !(*this == rhs);
}

uint16_t BinaryData::addVarIntU64(const uint64_t value, uint8_t data[], uint16_t offset) {
	uint64_t v = value;
	do {
		data[offset] = (v & 0x000000000000007F);
		v >>= 7;
		if (v)
			data[offset] |= 0x80;
		offset++;
	} while (v);
	return offset;
}

uint64_t BinaryData::get_VarIntU64(const uint8_t data[], uint16_t& offset, unsigned length) {
	uint64_t value = 0;
	int pos = 0;
	uint8_t v;
	do {
		if (length>offset) {
			value |= (((uint64_t)((v = data[offset++]) & 0x7F)) << pos);
		}
		else {
			v = 0;
		}
		pos += 7;
	} while ((v & 0x80) && (pos<63));//0,7,14,21,28,35,42,49,56,63
	return value;
}
BinaryData& BinaryData::addVarIntU64(const uint64_t value) {
	uint8_t data[sizeof(uint64_t)];
	return add(data, addVarIntU64(value, data, 0));
}

uint8_t BinaryData::get_uint8(uint16_t offset)const {
	if (m_length>offset) return m_data[offset];
	return 0;
}
uint8_t* BinaryData::get_uint8p(uint16_t offset)const {
	if (m_length > offset) return m_data + offset;
	return 0;
}

bool    BinaryData::DESEncrypt(const BinaryData& key) {
	if (key.m_length != 8) return false;
	DES des(m_data, m_data, key.m_data, ENCRYPT);
	return true;
}

bool    BinaryData::DESDecrypt(const BinaryData& key) {
	if (key.m_length != 8) return false;
	DES des(m_data, m_data, key.m_data, DECRYPT);
	return true;
}
/*==================================================================*/


ApduMsg& ApduMsg::adjustSize() {
	if (m_type == Normal) {
		if (m_hasLe) {
			if (m_dataLength == 0) {
				if (m_bin.m_length == 5) {
					if ((m_dataLength = (m_bin.m_length - 5)) <= NormalLimit) {
						m_bin.m_data[4] = (uint8_t)m_dataLength;
						m_bin.add(m_le); 
						return  *this;
					}
				}
				else {
					m_dataLength = (m_bin.m_length - 6);
					memcpy(m_bin.m_data + 5, m_bin.m_data + 6, m_dataLength);
					m_bin.m_data[4] = m_dataLength;
					m_bin.m_data[m_dataLength + 5] = m_le;
				}
			}
			else { // dataLength>0
				unsigned pos = m_dataLength + 6; 
				memcpy(m_bin.m_data +(pos-1), m_bin.m_data + pos , m_bin.m_length - pos);
				m_dataLength = (m_bin.m_length - 6);
				m_bin.m_data[4] = m_dataLength;
				m_bin.m_data[m_dataLength + 5] = m_le;
			}
		}
		else {
			if (m_dataLength == 0) {
				if ((m_dataLength = (m_bin.m_length - 4)) <= NormalLimit) {
					m_bin.allocate(m_bin.m_length + 1);
					memcpy(m_bin.m_data + 5, m_bin.m_data + 4, m_dataLength);
					m_bin.m_data[4] = (uint8_t)m_dataLength;
					m_bin.m_length++; 
				    return  *this;
				}
			}
			else {
				if ((m_dataLength = (m_bin.m_length - 5)) <= NormalLimit) {
					m_bin.m_data[4] = (uint8_t)m_dataLength;
					return  *this;
				}
			}
		}
		uint16_t len = (m_hasLe ? 6 : 5);
	    
		if ((m_dataLength = (m_bin.m_length - len)) <= NormalLimit) {
			m_bin.m_data[4] = (uint8_t)m_dataLength;
			return  *this;
		}
		/*
		m_type = Extended;
		m_bin.forceLength(m_bin.m_length + 2);
		uint8_t* destptr = m_bin.m_data + (m_bin.m_length - 1);
		uint8_t* srcptr = m_bin.m_data + (m_bin.m_length - 3);
		uint8_t* endptr = m_bin.m_data + 4;
		//std::cout<<std::endl<<"Antes:"<<std::endl<<m_bin<<std::endl<<std::endl;
		while (srcptr != endptr) (*destptr--) = (*srcptr--);
		//std::cout<<std::endl<<"Depois:"<<std::endl<<m_bin<<std::endl<<std::endl;
		m_bin.m_data[4] = 0;*/
	}
	if (m_type == Extended) {
		m_bin.m_data[5] = (m_dataLength & 0xFF) << 8;
		m_bin.m_data[6] = (m_dataLength & 0xFF);
	}
	return *this;
}

ApduMsg& ApduMsg::
init() {
	m_le = 0;
	m_hasLe = false;
	/* cla ins p1 p2 lc */
	if (m_bin.m_length == 0) return reset();
	/** considera que a alocacao minima sao 8 bytes **/
	if (m_bin.m_length <= 4) {
		while (m_bin.m_length<4) { m_bin.m_data[m_bin.m_length++] = 0; }
		m_dataLength = 0;
		m_type = Normal;
		return *this;
	}
	if (m_bin.m_length == 5) {
		while (m_bin.m_length<5) { m_bin.m_data[m_bin.m_length++] = 0; }
		m_dataLength = 0;
		m_hasLe = true;
		m_le = m_bin.m_data[4];
		m_type = Normal;
		return *this;
	}
	/*  m_bin.m_length >5 */
	if (m_bin.m_data[4] == (m_dataLength = (m_bin.m_length - 5))) {
		m_type = Normal;
		return *this;
	}
	/*  m_bin.m_length >5 */
	if (m_bin.m_data[4] == (m_dataLength = (m_bin.m_length - 6))) {
		m_type = Normal;
		m_hasLe = true;
		m_le = m_bin.m_data[m_bin.m_length - 1];
		return *this;
	}
	m_dataLength = 0;
	m_bin.m_length = 0;
	return *this;
}


static uint8_t gl_init[] = { 0x00, 0x00, 0x00, 0x00 };


ApduMsg::ApduMsg(const uint8_t* data, const uint16_t length)
	:m_bin(data, length), m_dataLength(0), m_le(0), m_hasLe(false), m_type(Incomplete)
{
	init();
}
ApduMsg::ApduMsg(const char *hexdata)
	: m_bin(hexdata), m_dataLength(0), m_le(0), m_hasLe(false), m_type(Incomplete)
{
	init();
}
ApduMsg::ApduMsg(const BinaryData& rhs)
	: m_bin(rhs), m_dataLength(0), m_le(0), m_hasLe(false), m_type(Incomplete)
{
	init();
}
ApduMsg::ApduMsg(uint8_t CLA, uint8_t INS, uint8_t P1, uint8_t P2)
	: m_bin(gl_init, sizeof(gl_init)), m_dataLength(0), m_le(0), m_hasLe(false), m_type(Normal)
{
	cla(CLA); ins(INS); p1(P1); p2(P2);
}
ApduMsg::ApduMsg() :
	m_bin(gl_init, sizeof(gl_init)), m_dataLength(0), m_le(0), m_hasLe(false), m_type(Normal)
{}

ApduMsg& ApduMsg::reset() {
	m_bin.set(gl_init, sizeof(gl_init));
	m_dataLength = 0;
	m_le = 0;
	m_hasLe = false;
	m_type = Normal;
	return *this;
}

ApduMsg& ApduMsg::resetData() {
	if (m_hasLe) {
		m_bin.m_length = 5; 
		m_bin.m_data[4] = m_le;
	}
	else {
		m_bin.m_length = 4;
	}
	m_type = Normal;
	return adjustSize();
}

ApduMsg& ApduMsg::set(const char *hexdata) {
	m_bin.set(hexdata);
	return init();
}

ApduMsg& ApduMsg::set(const BinaryData& rhs) {
	m_bin.set(rhs);
	return init();
}

ApduMsg& ApduMsg::addData(const uint8_t data) {
	m_bin.add(data);
	return adjustSize();
}

ApduMsg& ApduMsg::addData(const char *hexdata) {
	m_bin.add(hexdata);
	return adjustSize();
}

ApduMsg& ApduMsg::addData(const uint8_t* data, const uint16_t length) {
	m_bin.add(data, length);
	return adjustSize();
}
ApduMsg& ApduMsg::addData(const BinaryData& rhs) {
	m_bin.add(rhs);
	return adjustSize();
}

ApduMsg& ApduMsg::cla(uint8_t CLA) { m_bin.m_data[0] = CLA; return *this; }
ApduMsg& ApduMsg::ins(uint8_t INS) { m_bin.m_data[1] = INS; return *this; }
ApduMsg& ApduMsg::p1(uint8_t P1) { m_bin.m_data[2] = P1; return *this; }
ApduMsg& ApduMsg::p2(uint8_t P2) { m_bin.m_data[3] = P2; return *this; }
ApduMsg& ApduMsg::le(uint16_t LE) {
	m_le = LE;
	if (m_hasLe) {
		m_bin.m_data[(m_bin.m_length - 1)] = m_le;
	}
	else {
		m_bin.add(m_le);
		m_hasLe = true;
	}
	return *this;
}


uint8_t ApduMsg::cla()const { return m_bin.m_data[0]; }
uint8_t ApduMsg::ins()const { return m_bin.m_data[1]; }
uint8_t ApduMsg::p1()const { return m_bin.m_data[2]; }
uint8_t ApduMsg::p2()const { return m_bin.m_data[3]; }
uint16_t ApduMsg::lc()const { return m_dataLength; }
uint16_t ApduMsg::hasle()const { return m_hasLe; }
uint16_t ApduMsg::le()const {
	if(m_hasLe)	return m_le; 
	else return NULL;
}

/*  0   1   2  3  4   5   6    7 */
/* cla ins p1 p2 lc  Data */
/* cla ins p1 p2 lc=0 lc1 lc2 data */
const uint8_t * ApduMsg::getApduData()const {
	return (m_type == Extended) ? m_bin.m_data + 7 : m_bin.m_data + 5;
};

std::string    ApduMsg::toHex() const {
	return m_bin.toHex();
}

/*==================================================================*/

ApduResp::ApduResp(const uint8_t* data, const uint16_t length)
	:m_bin(data, length)
{
	init();
}

ApduResp::ApduResp() :
m_bin(NULL,0)
{}

ApduResp& ApduResp::set(const char *hexdata) {
	m_bin.set(hexdata);
	return init();
};

ApduResp& ApduResp::set(const BinaryData& rhs) {
	m_bin.set(rhs);
	return init();
};

ApduResp& ApduResp::set(const uint8_t* data, const uint16_t length) {
	m_bin.set(data, length);
	return init();
}

ApduResp& ApduResp::reset() {
	m_bin.set(NULL, 0);
	m_status = 0;

	return *this;
};

std::string  ApduResp::ResptoHex() const {
	return m_bin.toHex();
}

uint8_t ApduResp::get_uint8(uint16_t offset)const {
	if (m_bin.m_length>offset) return m_bin.get_uint8(offset);
	return 0;
}

uint8_t* ApduResp::get_uint8p(uint16_t offset)const {
	if (m_bin.m_length > offset) return m_bin.m_data + offset;
	return 0;
}

//uint8_t* ApduResp::get_uint8p(uint16_t offset)const {
//	if (m_bin.m_length > offset) return m_bin.m_data + offset;
//	return 0;
//}

ApduResp& ApduResp::init() {

	if (m_bin.m_length == 0) return reset();

	uint16_t buf1 = (uint16_t) (m_bin.get_uint8(m_bin.getLength() - 2));
	buf1 = (buf1 << 8);
	uint16_t buf2 = (uint16_t) m_bin.get_uint8(m_bin.getLength() - 1);
	//buf2 = (buf2 << 8);
	m_status = ( buf1 | buf2);
	
	m_bin.forceLength(m_bin.m_length-2);

	return *this;
}


