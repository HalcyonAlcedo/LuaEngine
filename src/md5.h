#ifndef MD5_H
#define MD5_H

#include <string>
#include <fstream>

/* Type define */
typedef unsigned char Byte;
typedef unsigned long ulong;

using std::string;
using std::ifstream;

/* MD5 declaration. */
class MD5 {
public:
	MD5();
	MD5(const void* input, size_t length);
	MD5(const string& str);
	MD5(ifstream& in);
	void update(const void* input, size_t length);
	void update(const string& str);
	void update(ifstream& in);
	const Byte* digest();
	string toString();
	void reset();
private:
	void update(const Byte* input, size_t length);
	void final();
	void transform(const Byte block[64]);
	void encode(const ulong* input, Byte* output, size_t length);
	void decode(const Byte* input, ulong* output, size_t length);
	string bytesToHexString(const Byte* input, size_t length);

	/* class uncopyable */
	MD5(const MD5&);
	MD5& operator=(const MD5&);
private:
	ulong _state[4];	/* state (ABCD) */
	ulong _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	Byte _buffer[64];	/* input buffer */
	Byte _digest[16];	/* message digest */
	bool _finished;		/* calculate finished ? */

	static const Byte PADDING[64];	/* padding for calculate */
	static const char HEX[16];
	static const size_t BUFFER_SIZE = 1024;
};

#endif/*MD5_H*/