#include "BigInt.h"


// [!] instantiate template BigInt<unsigned int, 2048>
// so I can populate mOperators with BigInt<...> functions
template class BigInt<unsigned int, 2048>; 

#include "calculator.h"

#include "TestUtils.h"

int main(){


	std::string_view sv{};

	// DEBUG("hello" << 2);

	BigInt<unsigned int, 2048> bi{"0x2284"};

	std::string input{"0x1+(0x2+0x3))()fasf(+0x4*0x5"};
	// //Calculator<BigInt<unsigned int, 2048>>::tokenize(input);

	input = "0x2284/0x5+1235+12555+(2*(3+3))+0x12315125*(1+3)";
	// Calculator<BigInt<unsigned int, 2048>>::tokenize(input);
	// // DEBUG("between tokenize and convert\n");
	// Calculator<BigInt<unsigned int, 2048>>::convert_infix_to_postfix_notation();
	// Calculator<BigInt<unsigned int, 2048>>::evaluate_postfix();
	Calculator<BigInt<unsigned int, 2048>>::calculate(input);



}