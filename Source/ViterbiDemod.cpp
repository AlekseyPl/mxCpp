#include "../Include/ViterbiDemod.h"
#include "../Include/WtsCalc.h"
#include "../Include/Sym2Bit.h"
#include <algorithm>
#include <iostream>

using namespace std;

ViterbiDemodulator::ViterbiDemodulator( const SignalParams& sp, int TBLen, bool debug ):
	TBLen( TBLen ), M( sp.M ), L( sp.L ), log2M( sp.log2M ), type( sp.type ), debug( debug ),
	ML( sp.ML ), ML1( sp.ML1 ), ML2( sp.ML2 ), Nt( sp.Nt ), N( sp.N ), NBits( sp.NBits ),
	constType( sp.constType )
{
	NewCumWts.resize(ML1);
	OldCumWts.resize(ML1);


	PrevSts.resize(TBLen);
	for (auto &p : PrevSts)     p.resize(sp.ML1);

	if( sp.type == DataType::real)		wtsCalc = make_shared<WtsCalcReal>(sp);
	if( sp.type == DataType::complex)	wtsCalc = make_shared<WtsCalcComplex>(sp);


	if( constType == ConstellationType::binary )	 conv = make_shared<BinaryConvertor>(sp);
	else if( constType == ConstellationType::m_ary ) conv = make_shared<MaryConvertor>(sp);
}

ViterbiDemodulator::~ViterbiDemodulator()
{

}

void ViterbiDemodulator::process(const DMatrix& RefSigsR,const DMatrix& RefSigsI, const DMatrix& SigR, const DMatrix& SigI,
	DVector& OutLLR, int SigsShift, int BitNumShift)
{
	int k, k1, n, m;

	int PrevSt;
	int MaxSt = ML1;

	// ���������� ��� �������� ������ �������� ������� (reference signal number)
	int RefSigNum;

	// ���������� ��� �������� �������� ������������ �������� ��� ������ �������� � ������ ������� (minimum)
	double Min;
	// ��������� ���������� (buffer)
	double Buf;

	// ������������� �������� NewCumWts � OldCumWts
	fill(OldCumWts.begin(), OldCumWts.end(), MInf);
	fill(NewCumWts.begin(), NewCumWts.end(), 0);

	OldCumWts.at(0) = 0;


	// ���������� ��� �������� ������ �������� ������� � PrevSts (column number)
	int32_t PrevStsColNum = TBLen - 2;

	// ���������� ��� �������� ������ �������� �������������� ������� (out
	// symbol)
	int OutSym;
	// ���������� - ����� ��� �� ������� ���� �������� �������� ��������
	// ��������� ����� �������� OutSym
	int32_t OutSymBitShift = log2M * ( L - 2 );

	// ���������� ��� �������� ������ ����, ������� ���� �������� �������� (bit number)
	int BitNum = BitNumShift;
	// ���������� �������� ������ ������� BitNum ��� �������� �������
	int BitNumRevShift;

	// ���������� ��� �������� ���������� ��������� ��� ��������� �� (X),
	// ����� ����������� ��� �������� (Z) � ��������������� (Y) ��������
	// ��� ��������� �������� ������� �� PrevSts (�������������� ���������
	// ����� ���� ����)
	int X, Y, Z;

	if( debug ) cout << "First step of processing" << endl;
	// 1. �� ������ T-1 �� �������� ������ ���� ��� ��������� ������� �
	// ��������
	for (k = 0, k1 = SigsShift; k < TBLen - 1; ++k, ++k1) { // k - �����
															// �������� ��
															// �������� ���� ���� ���������
		wtsCalc->CalculateWts( RefSigsR, RefSigsI, SigR, SigI, k, k1 );
		// ��������� �������� ���� � ������� ���� �����
		wtsCalc->CalculatePrevStAndCumWts( OldCumWts, NewCumWts, PrevSts.at(k));
		// ������������ � ��������� ��������
		swap(OldCumWts, NewCumWts);
	}


	if( debug ) cout << "Second step of processing" << endl;
	// 2. �� X �� �������� ������� ��������� ������� (���� ������� �� ����
	// ������ �������� ������), ����� ���� �� ����� ��. ��������, ����
	// ������� ������������ �������� TBLen = NSyms+L-1, �� ����� � ����� 2
	// �� ����� �����!
	X = ( TBLen <= L ) ? N - 1 : N + L - 1 - TBLen;
	if( debug ) cout << "X = " << X << " N " << N << " L " << L <<" TB" << TBLen<< endl;

	for (k = TBLen - 1, k1 = k + SigsShift; k < TBLen - 1 + X; ++k, ++k1) {
		// k - ����� �������� ��
		// �������� ���� ���� ���������
		wtsCalc->CalculateWts( RefSigsR, RefSigsI, SigR, SigI, k, k1 );

		// ��������� �������� ���� � ������� ���� �����
	 
		PrevStsColNum = ( PrevStsColNum + 1 ) % TBLen;

		wtsCalc->CalculatePrevStAndCumWts( OldCumWts, NewCumWts,  PrevSts.at(PrevStsColNum) );
		// ������������ � ��������� ��������
		swap(OldCumWts, NewCumWts);
		// ����������� �������� ���������� ������� �������� �������� ��
		// PrevSts.
		// ���� �� ��������� � ����� "�������" �������, �� ��������
		// �� ��� ���������, ������ �������������� ������ ���������
		// ���������� ��������� ��������� � M ���. ���� �����
		// �������������� ����� ������� ������� ������ �� ��������
		// �������� L-2 �������� � ��, ���� �� �������
		// ������������ �������� TBLen. ��� ���, ��� �������
		// ����������� ��������� �������� ���������� �������, �����
		// ��������� (����������������) ���������� ���� ��� �����.
		if (k >= N ) 			MaxSt >>= log2M; // ������� �� M

		// ������� ��������� ���� � ������ �������� �� ������� ����
		Min = MInf;

 		auto mi = min_element( OldCumWts.begin(), OldCumWts.begin() + MaxSt,
			[ ](double left, double right) { return ( left < right ); });
		Min = *mi;
		PrevSt = distance( OldCumWts.begin(), mi );

		// ������� TBLen-1 �������� ��������� �� PrevSts ��
		// ��������� � ������� PrevSt �� ������� ����
		m = PrevStsColNum;
		for (n = 0; n < TBLen - 1; n++) {
			PrevSt = PrevSts[ m ][ PrevSt ];
			if (--m== -1)			m = TBLen - 1;
		}
		// ����������� ����� ��������� � ����� �������
		OutSym = ( PrevSt >> OutSymBitShift );

		// ����������� ����� ������� � �������� ��� � ������� �� �
		// �������, ������� � BitNum
		conv->convert( OutLLR, OutSym);
	}
	if( debug ) cout << "Third step of processing " << endl;
	// 3. �������� ���� �� ����� �� � ��������� ������� �� Y �������� ��
	// ���� �������� ������. ���� T <= L �� Y = 1, ����� Y = T-(L-1).
	// ������������� � Y �������� ���������� �������� �������� �����
	// Z = T - Y.
	Y = (TBLen <= L) ? 1 : TBLen - ( L - 1 );
	Z = TBLen - Y;

	// ���� �� ����� ��
	// �������� ���� ���� ��������� (k ����� ������ ��������!)
	wtsCalc->CalculateWts( RefSigsR, RefSigsI, SigR, SigI, k, k1 );

	// ��������� �������� ���� � ������� ���� �����
	PrevStsColNum = ( PrevStsColNum + 1 ) % TBLen; 
	wtsCalc->CalculatePrevStAndCumWts( OldCumWts, NewCumWts, PrevSts.at(PrevStsColNum));

	// ��������� ������� �� Y ��������
	// C������ ���� ������� Z �������� �����
	PrevSt = 0; // �� ����� ����� �� ������ ��������� ��������
	for (k = 0; k < Z; ++k) {
		// ���� ��� ����� �� "�������" �������
		PrevSt = PrevSts.at(PrevStsColNum).at(PrevSt);
		PrevStsColNum--;
		if (PrevStsColNum == -1)
			PrevStsColNum = TBLen - 1;
	}


	// ��������� �������� ����� �� PrevSts, �� ��� � ������������
	// ��������� BitNum � ������� ������� ���� ����������
	// �������
	BitNum = NBits - log2M + BitNumShift;
	// ����� ����������� �������� ���������� ���� ��������
	// ������� ���� ����� �������� BitNum �� 2*log2M �����,
	// �.�. �� ������� ������� ���� ����������� �������
	BitNumRevShift = ( log2M << 1 );

	if( debug ) cout << "Y = " << Y <<" PrevSt " << PrevSt << endl;
	for (k = 0; k < Y; ++k) {
		// ����������� ����� ��������� � ����� �������
		OutSym = ( PrevSt >> OutSymBitShift );

		// ����������� ����� ������� � �������� ��� � �������
		// �� � �������, ������� � BitNum
		conv->convert( OutLLR, OutSym);
		BitNum -= BitNumRevShift;

		// ���� ��� ����� �� "�������" �������
		PrevSt = PrevSts.at(PrevStsColNum).at(PrevSt);
		PrevStsColNum--;
		if (PrevStsColNum == -1)
			PrevStsColNum = TBLen - 1;
	}
}

#if 0
///////////////////////////////////////////////////////////////////////////
// �������� ��������� /////////////////////////////////////////////////////
// TBLen �������� ����� ���������� �������� ���������� (��), �����
// ��������� ������� ��������� ������� � ������ �������; ����� �
// ������ ����� �� ��������� ������� � ����� �������. ����� �������,
// ����������� ����� ������ � ����� �� TBLen �� (��. PrevSts). � �����
// ������ � ������ ����������� ������ ������ ���������� ���� ������
// ������� ��������� ������� �� ���� ������ �� ������� �� �������
// ������ ����. ������ � ����� ����������� � ������ ���������� �� ����
// �������� ������ �� ������ ������������ ���������� ����� ���������
// ��������. �������� ������ �������� � ����������� �� �������� TBLen,
// ������� ���� �� ���� ���������� ����������� �������� ���������� �,
// �������������, 1 <= TBLen <= NSyms+L-1. (����� � ������ � ��
// ��������� ��� ��������� ������ TBLen = T; NSyms = N.)
//
// 1 <= T <= L (����� "������� 1"):
//   � ���� �������� ���� �� ����������� ������������ ��������� (L-T)
//   ��. ��� N �������� ���������� ������� (�.�. � ������ ����� ��,
//   ������� � �� �T, ��������� ������� �� ����� �������). �����������
//   �������� ����������, ������������ ��� ��������� ��������
//   ���������� ������� ����� ������:
//     T = 1 (�� �������������� L-1 ��):
//         |<-N ��---->|<-L-1 ��>|
//       --|-| ... |-|-|-| ... |-|-->t
//           T �� -> | |r
//   
//     1 < T < L (�� �������������� L-T ��):  
//         |<-N ��---->|<-L-1 ��---------->|
//       --|-| ... |-|-|-| ... |-|-| ... |-|-->t
//                   |<-T ��---->|
//
//     T = L (�� �������������� 0 ��):  
//         |<-N ��---->|<-L-1 ��-->|
//       --|-| ... |-|-|-| ... |-|-|-->t
//                   |<-T ��------>|
//
// L+1 <= T <= N+L-1 (����� "������� 2"):
//   � ���� �������� ������ (N+L-1)-T �������� ���������� ������� (�.�.
//   � ������ ����� ��, ������� � �� �T, ��������� ������� �� �����
//   �������). ���������� T-(L-1) �������� ���������� �� ������
//   ������������ �� ���� �������� ������ (������ L-1 �������� �
//   �������� ������� �������������, ��� ��� ��� ��������������).
//     T = L+1 (������ N-2 �������� �������������� �������, ���������
//       ��� ������� ���������� �� ������ ��������� ������� �� ������
//       ������������, ��� ���� ������ L-1 �������� �������������, ���
//       ��� ��� ��������������):
//         |<-N ��------>|<-L-1 ��>|
//       --|-| ... |-|-|-|-| ... |-|-->t
//                   |<-T ��------>|
//
//     L+1 < T < N+L-1 (������ (N+L-1)-T �������� ���������� �������, �
//       ���������� T-(L-1) �� ������ ��������� �������):
//         |<-N ��------------>|<-L-1 ��>|
//       --|-| ... |-|-| ... |-|-| ... |-|-->t
//                   |<-T ��------------>|
//    
//     T = N+L-1 (��� N �������� ���������� �� ��������� �������):
//         |<-N ��-->|<-L-1 ��>|
//       --|-| ... |-|-| ... |-|-->t
//         |<-T ��------------>|
//    
// ����� ���������� ��� �������� � ����� ����, ����� ������ ���.
// 1. �� ������ T-1 �� �������� ������ ���������� ����� � ������ �����
//    (����) ��� ��������� ������� � ��������.
// 2. �� X �� �������� ������� ���� �� ����� ��, ����� ���������
//    ������� (���� ������� �� ���� ������ �������� ������). ����
//    T <= L �� X = N-1, ����� X = N+L-1-T.
// 3. �������� ���� �� ����� �� � ��������� ������� �� Y �������� ��
//    ���� �������� ������. ���� T <= L �� Y = 1, ����� Y = T-(L-1).
//
// ��������:
// ���� T <= L: ������������ ��������� (T-1)+(N-1)+1 = N+T-1 ��,
//              ��������� ������� � (N-1)+1 = N ��������.
// �����:       ������������ ��������� (T-1)+(N+L-1-T)+1 = N+L-1 ��,
//              ��������� ������� � (N+L-1-T)+(T-(L-1)) = N ��������.
//
// �.�. �� �����: � ����� ������� ��������� ������� � N ��������, �
// ������ ������ �������������� N+T-1 ��, �� ������ - ��� N+L-1 ��.
#endif
