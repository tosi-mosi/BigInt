#include "BigInt.h"
#include "calculator.h"


#include <sstream>

int main(){


	DEBUG("hello" << 2);

	BigInt<unsigned int, 2048> bi{"0x2284"};

	std::string_view input{"0x1+(0x2+0x3))()fasf(+0x4*0x5"};
	// //Calculator<BigInt<unsigned int, 2048>>::tokenize(input);

	input = "0x2284/0x5+1235+12555+(2*(3+3))";
	Calculator<BigInt<unsigned int, 2048>>::tokenize(input);	
	DEBUG("between tokenize and convert\n");
	Calculator<BigInt<unsigned int, 2048>>::convert_infix_to_postfix_notation();



	// // Sorting out how to handle input in BigInt ctor with stringstream
	// // answer: use tellg() != tmp_str.length to see if there are characters left in the stream
	// std::string input{};
	// std::cin >> input;

	// std::stringstream ss{};
	// short var, var2;
	
	// ss << std::hex << input;

	// std::cout 
	// 	<< "gcount before extraction = " << ss.gcount() 
	// 	<< ", str content = " << ss.str()
	// 	<< '\n';
	// // bool bres = static_cast<bool>(ss >> var);

	// // //what will clear do?
	// // ss.clear();

	// // std::cout
	// // 	<< "(bool) = " << bres
	// // 	<< ", value = " << std::hex << var
	// // 	<< ", good = " << ss.good()
	// // 	<< ", fail = " << ss.fail()
	// // 	<< ", bad = " << ss.bad()
	// // 	<< ", gcount = " << ss.gcount()
	// // 	<< ", str content = " << ss.str()
	// // 	<< "\n";  	

	// // bres = static_cast<bool>(ss >> var2);

	// // // char arro[256];
	// // // ss.read(arro, 3);
	// // std::cout
	// // 	<< "(bool) = " << bres
	// // 	<< ", value = " << std::hex << var2
	// // 	<< ", good = " << ss.good()
	// // 	<< ", fail = " << ss.fail()
	// // 	<< ", bad = " << ss.bad()
	// // 	<< ", gcount = " << ss.gcount()
	// // 	<< ", str content = " << ss.str()
	// // 	<< "\n"; 

	// std::cout << "starting pos = " << ss.tellg() << '\n';

	// for(auto i{0}; i<7; ++i){

	// 	bool bres = static_cast<bool>(ss >> var);

	// 	//what will clear do?
	// 	//ss.clear();

	// 	std::cout
	// 		<< "pos = " << ss.tellg()
	// 		<< "(bool) = " << bres
	// 		<< ", value = " << std::hex << var
	// 		<< ", good = " << ss.good()
	// 		<< ", fail = " << ss.fail()
	// 		<< ", bad = " << ss.bad()
	// 		<< ", eof = " << ss.eof() 
	// 		<< ", gcount = " << ss.gcount()
	// 		<< ", str content = " << ss.str()
	// 		<< "\n";  	

	// }

}