#include "big_int.h"

using test_big_int_t = BigInt<unsigned int, 2048>;

// [!] instantiate template BigInt<unsigned int, 2048>
// so I can populate mOperators in calculator.h with BigInt<...> functions
template class BigInt<unsigned int, 2048>; 

#include "calculator.h"

#include "test_utils.h"

using test_utils_t = TestUtils<unsigned int, 2048>;

int main(){

	auto test_cases = std::vector<std::string>{{
		"0x2284/0x5+0x1235+0x12555+(0x2*(0x3+0x3))+0x12315125*(0x1+0x3^0x5541)*0x51871^0x151513^413", // good
		"0x1231515^0x11+((0x458727592876987fa52+0x411235)*0x516)^0x3",
		"0x15299285292859285+0x15299285292859285*0x15299285292859285-0x15299285292859285+0x1231515+0x1231515*0x5",
		"0x1+0x1"
	}};

	for(auto& el: test_cases)
		test_utils_t::check_compound_expression(el);

}

// comment
