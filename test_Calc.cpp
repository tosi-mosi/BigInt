#include "BigInt.h"
#include "calculator.h"

int main(){

	std::string_view input{"0x1+(0x2+0x3))()fasf(+0x4*0x5"};
	Calculator<BigInt<unsigned int, 2048>>::tokenize(input);

}