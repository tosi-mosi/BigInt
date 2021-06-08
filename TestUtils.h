
#ifndef UTILS_FOR_TESTING
#define UTILS_FOR_TESTING

#include "BigInt.h"
#include <iostream>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <regex>
#include <unistd.h> // for sleep(seconds)

#include <random>

#include <functional>

template<typename _T, int _bitlength>
class TestUtils{
		
public:	

	//constexpr size_t s{2048};
	using test_big_int_t = BigInt<_T, _bitlength>;
	using storage_type_t = typename test_big_int_t::STORAGE_TYPE;

	unsigned int seed;
	std::random_device rd;
	std::mt19937 mt;
	std::uniform_int_distribution<storage_type_t> distrib;

	TestUtils(unsigned int _seed = 0): 
		seed{_seed},
		mt{seed}, 
		distrib{storage_type_t{0}, storage_type_t{0}-1} 
	{
		// if no seed provided -> make seed random
		if(seed == 0)
			mt.seed(rd());
	}
	
	
	//need OperandWrapper to abstract away conversions to strings of operands of different types
	//because for BigInt it was BigInt::get_as_string()
	//and for int it was std::to_string()
	template<typename T>
	class OperandPtrWrapper{
		const T* m_ptr;
	public:
		OperandPtrWrapper (const T& var): m_ptr{&var} {}
		std::string get_as_string(bool param1=false, bool param2=false) const {
			return std::to_string(*m_ptr);
		}
		const T& operator * () const {
			return *m_ptr;
		}
		const T* operator -> () const {
			return m_ptr;
		}
	};

	template<typename T, int size>
	class OperandPtrWrapper<BigInt<T, size>>{
		const BigInt<T, size>* m_ptr;
	public:
		OperandPtrWrapper(const BigInt<T, size>& var): m_ptr{&var} {}
		std::string get_as_string(bool param1=false, bool param2=false) const {
			return m_ptr->get_as_string(param1, param2);
		}
		const BigInt<T, size>& operator * () const {
			return *m_ptr;
		}
		const BigInt<T, size>* operator -> () const {
			return m_ptr;
		}
	};

	//copy pasta from https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
	std::string exec(const char* cmd)  const{
	    std::array<char, 128> buffer;
	    std::string result;
	    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	    if (!pipe) {
	        throw std::runtime_error("popen() failed!");
	    }
	    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
	        //std::cout << buffer.data() << '\n' << buffer.size() << '\n';
	        result += buffer.data();
	        //std::cout << buffer.data() << '\n';
	    }
	    return result;
	}

	std::string parse_response(const std::string& target) const{
		static const std::regex r{"<div id=\"bignumber\" style=\"padding: 10px; font-family: monospace; border: solid black 2px;\">[\\s]+(.*)<\\/div>"};
		std::smatch match{};
		std::regex_search(target, match, r);

		auto str_match{match[1].str()};

		const std::string new_line_delimiter{"<br />"};

		for(size_t entry_index{};(entry_index = str_match.find(new_line_delimiter, entry_index))!=std::string::npos;)
			str_match.erase(entry_index, std::size(new_line_delimiter));

		// std::cout << "match = " << str_match << '\n';

		return str_match;
	}

	//[TODO] fix so that any BigInt<X, Y> can be used
	template<typename BIG_INT, int size>
	std::unique_ptr<std::array<BIG_INT, size>> generate_array() {
		auto result{ 
			std::make_unique<std::array<BIG_INT, size>>() 
		};

		std::cout << "seed = " << seed << '\n';

		for(auto& b: *result){
			for(auto& storage_el: *b.m_parr){
				storage_el = distrib(mt);
			}
		}

		return result;
	}

	unsigned int generate_number() {
		return distrib(mt);
	}

	template<typename OP1_TYPE, typename OP2_TYPE = OP1_TYPE>
	void check_binary_operation(
		const OP1_TYPE& operand1,								//OP1_TYPE - is always BigInt<A, x> -> no need to wrap it
		const OperandPtrWrapper<OP2_TYPE>& operand2_ptr,		//OP2_TYPE - BigInt<A, x> or int
		const std::string& str_repr_of_operator,
		const std::string& str_urlencoded_repr_of_operator,
		std::function<OP1_TYPE(const OP1_TYPE*, OP2_TYPE)> operation_func) const
	{
		std::string request_str_command = "curl -s -X POST -d 'eqn="
			+ operand1.get_as_string(true,false) 
			+ str_urlencoded_repr_of_operator
			+ operand2_ptr.get_as_string(true, false)
			//+ std::to_string(operand2_ptr)
			+ "&submit=Calculate&base=hex' https://defuse.ca/big-number-calculator.htm";

		auto my_response{
			operation_func(&operand1, *operand2_ptr).get_as_string()
		};

		auto html_dump{
			exec(request_str_command.c_str())
		};

		auto parsed_response{ parse_response(html_dump) }; 

		//std::cout << "[DEBUG_1]" << parsed_response << '\n';

		//truncate parsed response, if it is larger than our bit_length
		//we need this for <<, because it can sometime return us numbers larger than we can handle
		auto resp_len = parsed_response.length();
		if(resp_len > OP1_TYPE::max_hex_digits){
			parsed_response.erase(0, resp_len - OP1_TYPE::max_hex_digits);
			parsed_response.erase(0, parsed_response.find_first_not_of('0'));
		}

		if(parsed_response == "")
			parsed_response = "0";

		//std::cout << "[DEBUG_2]" << parsed_response << '\n';

		//[TOFIX] same as above 
		if(auto res = (my_response == parsed_response))
			std::cout << std::boolalpha << res << '\n';
		else{
			std::string error_info{
				std::to_string(res) + " <- " + operand1.get_as_string(true) + str_repr_of_operator
				+ operand2_ptr.get_as_string(true) + "\n"
				// + std::to_string(operand2) + "\n"
				+ "my: " + my_response + "\n"
				+ "his:" + parsed_response + "\n"
			};
			std::string error_log_command{
				"echo \"" + error_info + "\" >> log.txt"
			};
			std::cout << std::boolalpha << error_info;
			system(error_log_command.c_str());
		}
	}

};

#endif