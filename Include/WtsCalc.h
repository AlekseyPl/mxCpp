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

	WtsCalc( const SignalParams& sp, bool debug = false);
	~WtsCalc( );


	void CalculateWts( const DMatrix& RefSigsR, const DMatrix& RefSigsI,
					   const DMatrix& SigR, const DMatrix& SigI, int k, int k1);
	void CalculatePrevStAndCumWts(const DVector& OldCumWts, DVector& NewCumWts, IVector& PrevStsCurCol);

private:

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

	// ���������� ������� ����� ��������� (Wts - branch // weights) �������� ML ���������
	DVector wts;

	void CalculateComplex(const DMatrix& RefSigsR, const DMatrix& RefSigsI,
						  const DMatrix& SigR, const DMatrix& SigI, int k, int k1);
	void CalculateReal(const DMatrix& RefSigsR, const DMatrix& SigR, int k, int k1);
};

#endif /* INCLUDE_WTSCALC_H_ */
