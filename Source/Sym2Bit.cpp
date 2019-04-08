#include "../Include/Sym2Bit.h"
#include <math.h>

// Функция вычисления и записи значения оценки бит при известном
// значении оценки символа. При M = 2 задача вырожденная и используется
// функция Sym2Bits2, в противном случае используется Sym2BitsM.
// Вместо самого BitNum передаётся указатель на него, чтобы изменения
// BitNum в функции Sym2Bits приводили к изменению BitNum и в основной
// функции.


void BinaryConvertor::convert( DVector& OutLLR, const int& OutSym )
{
	OutLLR.push_back( 1000 - 2000 * static_cast<double>( OutSym ) );
}

void MaryConvertor::convert( DVector& OutLLR,const int& OutSym )
{
 	// Для случая, когда старший бит идёт первым
	uint32_t Mult = ( 1 << ( log2M - 1 ) );
	for ( int n = log2M - 1; n >= 0; --n) {
		OutLLR.push_back(1000 -	2000 * static_cast<double>( ( OutSym & Mult ) >> n ) );
		Mult >>= 1;
	}
}
