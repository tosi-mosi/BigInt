
#include "big_numbers.h"
#include "big_numbers_utils_for_testing.h"

using test_utils_t = TestUtils<unsigned int, 2048>;

void test_right_shift(auto& test_parr, test_utils_t& testUtils){
	
	std::cout << "\tTesting >>:\n";

	for(const auto& el: *test_parr){	

		auto shift{ testUtils.generate_number()%test_utils_t::test_big_int_t::max_bit_length };

		testUtils.check_binary_operation<test_utils_t::test_big_int_t, int>(
			el,
			shift,
			">>",
			"%3E%3E",
			&test_utils_t::test_big_int_t::operator >>
		);
		sleep(0.5);
	}
}

void test_left_shift(auto& test_parr, test_utils_t& testUtils){
	
	std::cout << "\tTesting <<:\n";

	// auto check left shift
	for(const auto& el: *test_parr){
	
		auto shift{ testUtils.generate_number()%test_utils_t::test_big_int_t::max_bit_length };
	
		testUtils.check_binary_operation<test_utils_t::test_big_int_t, int>(
			el,
			shift,
			"<<",
			"%3C%3C",
			&test_utils_t::test_big_int_t::operator <<
		);
		sleep(0.5);
	}
}

void test_addition(auto& test_parr, test_utils_t& testUtils){

	std::cout << "\tTesting addition:\n";

	//go from both ends of the array converging at the middle
	for(auto l_to_r{(*test_parr).begin()}, r_to_l{(*test_parr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){
		testUtils.check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*l_to_r,
			*r_to_l,
			"+",
			"%2B",
			&test_utils_t::test_big_int_t::operator +
		);
		sleep(0.5);
	}	
}

void test_subtraction(auto& test_parr, test_utils_t& testUtils){

	std::cout << "\tTesting subtraction:\n";

	for(auto l_to_r{(*test_parr).begin()}, r_to_l{(*test_parr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){
		
		test_utils_t::test_big_int_t *smaller_ptr, *greater_ptr;

	 	// defuse.ca's - works with signed arithmetic, my - doesnt, so if a<b -> just swap them
		if(*l_to_r > *r_to_l)
			greater_ptr = &(*l_to_r), smaller_ptr = &(*r_to_l);
		else
			smaller_ptr = &(*l_to_r), greater_ptr = &(*r_to_l);

		testUtils.check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*greater_ptr,
			*smaller_ptr,
			"-",
			"-",
			&test_utils_t::test_big_int_t::operator -
		);
		sleep(0.5);
	}	
}

void test_multiplication(auto& test_parr, test_utils_t& testUtils){
	
	std::cout << "\tTesting multiplication:\n";

	for(auto l_to_r{(*test_parr).begin()}, r_to_l{(*test_parr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){

		testUtils.check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*l_to_r,
			*r_to_l,
			"*",
			"*",
			&test_utils_t::test_big_int_t::operator *
		);
		sleep(0.5);
	}	
}

void test_division(auto& test_parr, test_utils_t& testUtils){

	std::cout << "\tTesting division:\n";

	for(auto l_to_r{(*test_parr).begin()}, r_to_l{(*test_parr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){
		
		test_utils_t::test_big_int_t *smaller_ptr, *greater_ptr;

		if(*l_to_r > *r_to_l)
			greater_ptr = &(*l_to_r), smaller_ptr = &(*r_to_l);
		else
			smaller_ptr = &(*l_to_r), greater_ptr = &(*r_to_l);

		testUtils.check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*greater_ptr,
			*smaller_ptr,
			"/",
			"%2F",
			&test_utils_t::test_big_int_t::operator /
		);
		sleep(0.5);
	}	
}

void test_exponentation(auto& test_parr, test_utils_t& testUtils){

	std::cout << "\tTesting exponentation:\n";

	for(auto l_to_r{(*test_parr).begin()}, r_to_l{(*test_parr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){

		testUtils.check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*l_to_r,
			*r_to_l,
			"^",
			"%5E",
			&test_utils_t::test_big_int_t::pow
		);
		sleep(0.5);
	}	
}

int main(){

	test_utils_t test_utils{123};

	std::cout << "size of BigInt::STORAGE_TYPE = " << sizeof(test_utils_t::test_big_int_t::STORAGE_TYPE) << '\n';

	auto test_parr{
		test_utils.generate_array<test_utils_t::test_big_int_t, 5>()
	};

	test_right_shift(test_parr, test_utils);
	test_left_shift(test_parr, test_utils);
	test_addition(test_parr, test_utils);
	test_subtraction(test_parr, test_utils);
	test_multiplication(test_parr, test_utils);
	test_division(test_parr, test_utils);
	test_exponentation(test_parr, test_utils);

}
