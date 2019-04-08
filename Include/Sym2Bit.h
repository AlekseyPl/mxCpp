#/*
 * Sym2Bit.h
 *
 *  Created on: Nov 9, 2018
 *      Author: aplotnikov
 */

#ifndef INCLUDE_SYM2BIT_H_
#define INCLUDE_SYM2BIT_H_

#include "Types.h"
#include <algorithm>
#include <vector>
#include <memory>

class Sym2Bit {
public: 

	Sym2Bit( const SignalParams& sp ):
		log2M( sp.log2M )	{}
	virtual ~Sym2Bit()		{}

	virtual void convert(DVector& OutLLR, const int& OutSym) = 0;

protected:
	int					log2M;
};

class BinaryConvertor : public Sym2Bit {
public:

	BinaryConvertor(const SignalParams& sp):
		Sym2Bit(sp)		{}
	~BinaryConvertor( ) {}

	virtual void convert(DVector& outLLR, const int& OutSym );
};

class MaryConvertor : public Sym2Bit {
public:

	MaryConvertor(const SignalParams& sp):
		Sym2Bit(sp)		{}
	~MaryConvertor( )	{}

	virtual void convert(DVector& outLLR, const int& OutSym );
};


#endif
