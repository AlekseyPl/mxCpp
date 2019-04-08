#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <vector>
#include <float.h>
#include <memory>

enum MexInParamsIdx {
	L_idx		= 0,
	M_idx		= 1,
	N_idx		= 2,
	Nt_idx		= 3,
	Nsymb_idx   = 4,
	Var_idx     = 5, // not used in Viterbi // ONLY FOR BCJR
	Spectrs_idx = 6,
	RefSigs_idx = 7,
	InLLR_idx   = 8, // not used in Viterbi // ONLY FOR BCJR
	TBLen_idx   = 9
};


enum class DataType {
	real = 0,
	complex = 1
};

enum class ConstellationType {
	binary = 0,
	m_ary  = 1

};

struct SignalParams 
{ // ���������, ���������� � ���� ��������� ������� � �������������� �����������
  // ��� ����������� ����������

	int M; // ������ ���������
	int L; // ������������ �������� � �������� ����������
	int Nt;// ���������� �������� �� ��
	int N; // ���������� ������������� ��������

	int log2M;
	int ML;   // M^L
	int ML1;  // M^(L-1)
	int ML2;  // M^(L-2)
	int ML2L1; // M^L * (2*L-1)

	int NL;    // N + L
	int NL1;   // N + L - 1
	int NBits;
	DataType type;					// �����������/������������ ������
	ConstellationType constType;	// ��� ���������(�������� ��������� ����������)

};


enum 	AlgorithmType {
	TrueBCJR  = 0,
	MaxBCJR   = 1,
	TrueMBCJR = 2,
	MaxMBCJR  = 3
};
 
struct ValueState {
    unsigned		state;
    double			value;
    ValueState( unsigned st = 0, double val = 0 ):
		state( st ), value( val ) { }
};

#ifdef BCJR
const double MInf = -DBL_MAX;
#else
const double MInf = DBL_MAX;
#endif

using StVector = std::vector< ValueState >;
using StMatrix = std::vector< StVector >;

using DVector = std::vector< double >;
using DMatrix =	std::vector< DVector >;

using IVector = std::vector< int >;
using IMatrix = std::vector< IVector >;


#endif
