
#ifndef UTILS_FOR_TESTING
#define UTILS_FOR_TESTING

#include "big_int.h"
#include "calculator.h"
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

#include <unordered_map>

#ifdef DEBUG_TEST_UTILS
#  define DEBUG(x) std::cerr << x;
#else
#  define DEBUG(x) do {} while (0)
#endif

template<typename _T, int _bitlength>
class TestUtils{
		
public:	

	// [!] need to update this map as new operators are added to BigInt 
	static std::unordered_map<std::string, std::string> operators_url_encoded;

	using test_big_int_t = BigInt<_T, _bitlength>;
	using storage_type_t = typename test_big_int_t::STORAGE_TYPE;
	using calc_t = Calculator<BigInt<_T, _bitlength>>;

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
	
	
	// need OperandWrapper to abstract away conversions to strings of operands of different types
	// because for BigInt it was BigInt::get_as_string()
	// while for int it was std::to_string()
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
	static std::string exec(const char* cmd)  {
	    std::array<char, 128> buffer;
	    std::string result;
	    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	    if (!pipe) {
	        throw std::runtime_error("popen() failed!");
	    }
	    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
	        result += buffer.data();
	    }
	    return result;
	}

	static std::string parse_response(const std::string& target) {
		static const std::regex r{"<div id=\"bignumber\" style=\"padding: 10px; font-family: monospace; border: solid black 2px;\">[\\s]+(.*)<\\/div>"};
		std::smatch match{};
		std::regex_search(target, match, r);

		auto str_match{match[1].str()};

		const std::string new_line_delimiter{"<br />"};

		for(size_t entry_index{};(entry_index = str_match.find(new_line_delimiter, entry_index))!=std::string::npos;)
			str_match.erase(entry_index, std::size(new_line_delimiter));

		return str_match;
	}

	//[+][TODO] fix so that any BigInt<X, Y> can be used
	template<typename BIG_INT>
	std::vector<BIG_INT> generate_test_data_vec(int size) {
		std::vector<BIG_INT> result;

		std::cout << "seed = " << seed << '\n';

		for(int i{0}; i<size; ++i){
			result.emplace_back();
			for(auto& storage_el: *result.back().m_parr){
				storage_el = distrib(mt);
			}
		}

		return result;
	}

	unsigned int generate_number() {
		return distrib(mt);
	}

	template<typename OP1_TYPE, typename OP2_TYPE = OP1_TYPE>
	static void check_binary_operation(
		const OP1_TYPE& operand1,								// OP1_TYPE - is always BigInt<A, x> -> no need to wrap it
		const OperandPtrWrapper<OP2_TYPE>& operand2_ptr,		// OP2_TYPE - BigInt<A, x> or int
		const std::string& str_repr_of_operator,
		const std::string& str_urlencoded_repr_of_operator,
		std::function<OP1_TYPE(const OP1_TYPE*, const OP2_TYPE&)> operation_func)
	{
		std::string request_str_command = "curl -s -X POST -d 'eqn="
			+ operand1.get_as_string(true,false) 
			+ str_urlencoded_repr_of_operator
			+ operand2_ptr.get_as_string(true, false)
			+ "&submit=Calculate&base=hex' https://defuse.ca/big-number-calculator.htm";

		auto my_response{
			operation_func(&operand1, *operand2_ptr).get_as_string()
		};

		auto html_dump{
			exec(request_str_command.c_str())
		};

		auto parsed_response{ parse_response(html_dump) }; 

		format_response(parsed_response);

		if(auto res = (my_response == parsed_response))
			std::cout << std::boolalpha << res << '\n';
		else{
			std::string error_info{
				std::to_string(res) + " <- " + operand1.get_as_string(true) + str_repr_of_operator
				+ operand2_ptr.get_as_string(true) + "\n"
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

	// -----------------------------------------------------------------------------
	// 					For Calculator (testing compound expressions)
	// -----------------------------------------------------------------------------	

	static std::string url_encode_expr_string(std::string input){

		// merge operator strReprs into one string,
		// so that it can be used in string::find_first_of
		static std::string operatorsStrCombined{
			// no need to capture operators_url_encoded,
			//  because it is of static-storage duration
			//   so it will be captured automatically
			[]()->std::string{
				std::string res{};
		        for(const auto& el: operators_url_encoded)
		            res += el.first;
		        return res;
			}()
		};
		
		for(size_t index{0}; ; ){
			
			index = input.find_first_of(operatorsStrCombined, index);
			if(index == std::string::npos)
				break;

			// [?] this wouldn't work if there are operators with strRepr of length > 1
			input.replace(index, 1, operators_url_encoded[std::string{input[index]}]);
			index += operators_url_encoded[std::string{input[index]}].length();
		}

		return input;
	}

	static void format_response(std::string &parsed_response){

		// truncate parsed response, if it is larger than our bit_length
		// we need this, because defuse.ca can sometimes return us numbers larger than we can handle
		auto resp_len = parsed_response.length();
		if(resp_len > BigInt<_T, _bitlength>::max_hex_digits){
			parsed_response.erase(0, resp_len - BigInt<_T, _bitlength>::max_hex_digits);
			parsed_response.erase(0, parsed_response.find_first_not_of('0'));
		}

		if(parsed_response == "")
			parsed_response = "0";

	}

	// [?] workaround for: "Sorry, we can't calculate numbers THAT big!" when using ^
	// since ^ works differently in defuse.ca, 
	// I can check ^ separately using python pow(x, y, p):
	//     1) would have to divide expression, so that ^ will be calculated on python
	//        and results of ^ will be substituted into defuse.ca request string
	static void check_compound_expression(std::string expression){

		std::string request_str_command = "curl -s -X POST -d 'eqn="
			+ url_encode_expr_string(expression)
			+ "&submit=Calculate&base=hex' https://defuse.ca/big-number-calculator.htm";

		DEBUG("curl req: " << request_str_command << '\n');

		auto html_dump{
			exec(request_str_command.c_str())
		};

		auto parsed_response{ parse_response(html_dump) }; 

		format_response(parsed_response);

		auto my_response{
			Calculator<BigInt<_T, _bitlength>>::calculate(expression)
		};

		DEBUG("my: " << my_response << "\nhis: " << parsed_response << '\n');

		if(auto res = (my_response == parsed_response))
			std::cout << std::boolalpha << res << '\n';
		else{
			std::string error_info{
				expression + '\n'
				+ "my: " + my_response + '\n'
				+ "his: " + parsed_response + '\n'
			};
			std::string error_log_command{
				"echo \"" + error_info + "\" >> log.txt"
			};
			std::cout << std::boolalpha << error_info;
			system(error_log_command.c_str());
		}

	}

};

template<typename _T, int _bitlength>
std::unordered_map<std::string, std::string> TestUtils<_T, _bitlength>::operators_url_encoded{{
	{"(", "%28"},
	{")", "%29"},
	{"+", "%2B"},
	{"-", "-"},
	{"*", "*"},
	{"/", "%2F"},
	{"^", "%5E"}
}};

#endif