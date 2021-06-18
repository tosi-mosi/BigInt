
#include "BigInt.h"
#include "TestUtils.h"

using test_utils_t = TestUtils<unsigned int, 2048>;

void test_right_shift(const auto& test_arr, test_utils_t& testUtils){
	
	std::cout << "\tTesting >>:\n";

	for(const auto& el: test_arr){	

		auto shift{ testUtils.generate_number()%test_utils_t::test_big_int_t::max_bit_length };

		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, int>(
			el,
			shift,
			">>",
			"%3E%3E",
			&test_utils_t::test_big_int_t::operator >>
		);
		sleep(0.5);
	}
}

void test_left_shift(const auto& test_arr, test_utils_t& testUtils){
	
	std::cout << "\tTesting <<:\n";

	// auto check left shift
	for(const auto& el: test_arr){
	
		auto shift{ testUtils.generate_number()%test_utils_t::test_big_int_t::max_bit_length };
	
		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, int>(
			el,
			shift,
			"<<",
			"%3C%3C",
			&test_utils_t::test_big_int_t::operator <<
		);
		sleep(0.5);
	}
}

void test_addition(const auto& test_arr){

	std::cout << "\tTesting addition:\n";

	// go from both ends of the array converging at the middle
	for(auto l_to_r{(test_arr).begin()}, r_to_l{(test_arr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){
		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*l_to_r,
			*r_to_l,
			"+",
			"%2B",
			&test_utils_t::test_big_int_t::operator +
		);
		sleep(0.5);
	}	
}

void test_subtraction(const auto& test_arr){

	std::cout << "\tTesting subtraction:\n";

	for(auto l_to_r{(test_arr).begin()}, r_to_l{(test_arr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){
		
		const test_utils_t::test_big_int_t *smaller_ptr, *greater_ptr;

	 	// defuse.ca's - works with signed arithmetic, my - doesnt, so if a<b -> just swap them
		if(*l_to_r > *r_to_l)
			greater_ptr = &(*l_to_r), smaller_ptr = &(*r_to_l);
		else
			smaller_ptr = &(*l_to_r), greater_ptr = &(*r_to_l);

		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*greater_ptr,
			*smaller_ptr,
			"-",
			"-",
			&test_utils_t::test_big_int_t::operator -
		);
		sleep(0.5);
	}	
}

void test_multiplication(const auto& test_arr){
	
	std::cout << "\tTesting multiplication:\n";

	for(auto l_to_r{(test_arr).begin()}, r_to_l{(test_arr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){

		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*l_to_r,
			*r_to_l,
			"*",
			"*",
			&test_utils_t::test_big_int_t::operator *
		);
		sleep(0.5);
	}	 
}

void test_division(const auto& test_arr){

	std::cout << "\tTesting division:\n";

	for(auto l_to_r{(test_arr).begin()}, r_to_l{(test_arr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){
		
		const test_utils_t::test_big_int_t *smaller_ptr, *greater_ptr;

		if(*l_to_r > *r_to_l)
			greater_ptr = &(*l_to_r), smaller_ptr = &(*r_to_l);
		else
			smaller_ptr = &(*l_to_r), greater_ptr = &(*r_to_l);

		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
			*greater_ptr,
			*smaller_ptr,
			"/",
			"%2F",
			&test_utils_t::test_big_int_t::operator /
		);
		sleep(0.5);
	}	
}

// can test this with python
void test_exponentation(const auto& test_arr){

	std::cout << "\tTesting exponentation:\n";

	for(auto l_to_r{(test_arr).begin()}, r_to_l{(test_arr).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){

		test_utils_t::check_binary_operation<test_utils_t::test_big_int_t, test_utils_t::test_big_int_t>(
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

	test_utils_t test_utils{};

	std::cout << "size of BigInt::STORAGE_TYPE = " << sizeof(test_utils_t::test_big_int_t::STORAGE_TYPE) << '\n';

	auto test_data_vec{
		test_utils.generate_test_data_vec<test_utils_t::test_big_int_t>(10)
	};


	// for(auto l_to_r{(test_data_vec).begin()}, r_to_l{(test_data_vec).end()-1}; l_to_r <= r_to_l; ++l_to_r, --r_to_l){

	// 	std::cout << l_to_r->pow(*r_to_l).get_as_string() << "\n\n";

	// }

	test_right_shift(test_data_vec, test_utils);
	test_left_shift(test_data_vec, test_utils);
	test_addition(test_data_vec);
	test_subtraction(test_data_vec);
	test_multiplication(test_data_vec);
	test_division(test_data_vec);
	test_exponentation(test_data_vec);



}
