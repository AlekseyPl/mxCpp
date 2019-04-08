/*
 * WtsCalc.h
 *
 *  Created on: Nov 9, 2018
 *      Author: aplotnikov
 */

#ifndef INCLUDE_WTSCALC_H_
#define INCLUDE_WTSCALC_H_

#include "Types.h"
#include <algorithm>
#include <vector>
#include <memory>

class WtsCalc {
public:

	WtsCalc(const SignalParams& sp, bool debug = false);
	virtual ~WtsCalc( ) { }


	virtual void CalculateWts( const DMatrix& RefSigsR, const DMatrix& RefSigsI,
					   const DMatrix& SigR, const DMatrix& SigI, int k, int k1) = 0;
	void CalculatePrevStAndCumWts(const DVector& OldCumWts, DVector& NewCumWts, IVector& PrevStsCurCol);

protected:

	DataType type;
	bool debug;
	int M;
	int L;
	int log2M;

	int ML;
	int ML1;
	int ML2;

	int Nt;
	int RefSigShift;
	int BufRefSigShift;

	int NSyms;
	// Объявление вектора весов переходов (Wts - branch // weights) размером ML элементов
	DVector wts;
};


class WtsCalcReal : public WtsCalc {
public:
	WtsCalcReal(const SignalParams& sp, bool debug = false):
		WtsCalc(sp,debug)  {}
	~WtsCalcReal( )		   {}

	void CalculateWts( const DMatrix& RefSigsR, const DMatrix& RefSigsI,
					   const DMatrix& SigR, const DMatrix& SigI, int k, int k1);
};


class WtsCalcComplex: public WtsCalc {
public:
	WtsCalcComplex(const SignalParams& sp, bool debug = false):
		WtsCalc(sp,debug)	{}
	~WtsCalcComplex( )		{}

	void CalculateWts( const DMatrix& RefSigsR, const DMatrix& RefSigsI,
					   const DMatrix& SigR, const DMatrix& SigI, int k, int k1);
};

#endif /* INCLUDE_WTSCALC_H_ */
