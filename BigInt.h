
#ifndef BIG_NUMBERS
#define BIG_NUMBERS

#include <array>
#include <memory>
#include <string_view>
#include <sstream>

#include <iostream>

#include <iomanip> // for std::setfill("0")

#ifdef DEBUG_BIG_INT
#  define DEBUG(x) std::cerr << x;
#else
#  define DEBUG(x) do {} while (0)
#endif

// [+][-][+][solved] BUG: fasf -> it reads without throwing exception - not what I want

// [?] add noexcepts to functions and ctors ?
//  need to revise how they work

// [?] maybe use move_if_no_except() in move ctor/assignment ?

// [?] STORAGE_TYPE should be unsigned
template<typename _STORAGE_TYPE, int _bit_length>
class BigInt{

public:

	//so definitions outside of class can access STORAGE_TYPE 
	using STORAGE_TYPE = _STORAGE_TYPE;

	constexpr static int max_bit_length = _bit_length;
	constexpr static int max_hex_digits = (max_bit_length/4); // 1 hex digit = 4 bits
	constexpr static int StorageNum = (max_bit_length)/(sizeof(STORAGE_TYPE)*8);
	constexpr static int StorageSize = sizeof(STORAGE_TYPE);
	constexpr static int storage_el_bit_length = (StorageSize*8);

	constexpr static STORAGE_TYPE max_el_value{ ~BigInt::STORAGE_TYPE{0} };
	constexpr static STORAGE_TYPE ms_half_mask{ max_el_value << (storage_el_bit_length/2) };
	constexpr static STORAGE_TYPE ls_half_mask{ ~ms_half_mask };

private:

	// not making member std::array directly, because big array obj would take up a lot of stack space,
	//  since it's just a wrapper over T[N].
	std::unique_ptr<std::array<STORAGE_TYPE, StorageNum>> m_parr;

	enum class ComparisonOption{
		smaller,
		equal,
		greater
	};

public:

	BigInt(): 
		m_parr{ std::make_unique< std::array<STORAGE_TYPE, StorageNum> >() }
	{
		m_parr->fill(0);
	}
	
	BigInt(std::string_view str_repr):
		BigInt{}
	{

		//check if template parameter max_bit_length complies to requirements
		static_assert(
			max_bit_length%(sizeof(STORAGE_TYPE)*8) == 0 && max_bit_length > 0, 
			"max_bit_length should be a multiple of sizeof(STORAGE_TYPE) and > 0"
		);

		//if prepended with 0x, then remove 0x
		if(str_repr[0] == '0' && str_repr[1] == 'x')
			str_repr.remove_prefix(2);

		//check for prepending zeros
		for(int i{0};;++i){
			if(str_repr[i]!='0'){
				str_repr.remove_prefix(i);
				break;
			}
		}

		// check ranges
		// size in bytes = 2 * number of hex digits
		auto str_payload_len = str_repr.length();
		auto hex_digs_in_one_storage_element = 2 * StorageSize;
		auto max_hex_digits_in_general = hex_digs_in_one_storage_element * StorageNum;
		if(str_payload_len > max_hex_digits_in_general)
			throw std::range_error{
				std::string{"[BigInt ctor] max allowed hex digits = "}
				+ std::to_string(max_hex_digits_in_general)
				+ std::string{", while it was provided ="}
				+ std::to_string(str_payload_len)
			};

		std::stringstream ss;

		//[TODO] how to check input string for validity with stringstream?

		for(int i{str_payload_len}; i>0; i-=hex_digs_in_one_storage_element){
			int offset_from_left{};
			int count{};

			if(i-hex_digs_in_one_storage_element>=0){
			    count = hex_digs_in_one_storage_element;
			    offset_from_left = i-hex_digs_in_one_storage_element;
			}
			else{
			    count = i;
			    offset_from_left = 0;
			}

			std::string tmp_str{str_repr.substr(offset_from_left, count)};
			ss << std::hex << tmp_str;

			// I need tellg = -1, because when after extracting stream has smth left -> sets failbit -> tellg = -1
			bool b_extracted = ss >> (*m_parr)[(str_payload_len-i)/hex_digs_in_one_storage_element];
			bool b_characters_left_in_ss = ss.tellg()!=-1;

			if(!b_extracted || b_characters_left_in_ss){
				DEBUG("b_extracted = " << b_extracted << ", b_chars_lef_in_ss = " << b_characters_left_in_ss << '\n');
				throw std::runtime_error(std::string{"BigInt invalid input "} + tmp_str);
			}

			ss.clear();
		}
	}

	// copy ctors
	//  copies will be done with copy ctor 
	//   -> less performant than move, but still allows copying, which is cool I guess
	BigInt(const BigInt& to_be_copied): 
		m_parr{std::make_unique< std::array<STORAGE_TYPE, StorageNum> >(*to_be_copied.m_parr)}
	{}

	BigInt& operator= (const BigInt& rhs) {

		//check for self-assignment
		if(this == &rhs)
			return *this;

		//make copy by calling copy assignment of std::array
		(*m_parr) = (*rhs.m_parr);

		return *this;
	}

	// move ctors
	//  I should implement move semantics,
	//   returning from generator functions will be done with move 
	//    -> boost in performance
	BigInt(BigInt&& to_be_stolen_from): 
		m_parr(std::move(to_be_stolen_from.m_parr))
	{}

	BigInt& operator= (BigInt&& rhs) {

		//check for self-assignment
		if(this == &rhs)
			return *this;

		m_parr = std::move(rhs.m_parr);
		return *this;
	}

	~BigInt() = default;

	BigInt operator >> (int operand) const {

		BigInt result{};

		int shift_storage_elements_val{ operand/storage_el_bit_length };
		int shift_bits_val{ operand%storage_el_bit_length };

		STORAGE_TYPE all_ones{ ~STORAGE_TYPE{0} };

		for(int i{0}; i<StorageNum-shift_storage_elements_val; ++i){
			auto mask1 = all_ones<<shift_bits_val;
			auto mask2 = ~mask1;
			(*result.m_parr)[i] |= 
				( ((*m_parr)[i+shift_storage_elements_val] & mask1) ) >> shift_bits_val;
			if(i+shift_storage_elements_val < StorageNum - 1)
				(*result.m_parr)[i] |=
					((*m_parr)[i+shift_storage_elements_val+1] & mask2) << (storage_el_bit_length - shift_bits_val);
		}

		return result;
	}

	// [?] will I need this?
	// BigInt operator >> (const BigInt& operand) const {

	// 	// result is the same as 
	// 	if(operand > BigInt{""})

	// 	return (*this)>>(operand%)
	// }

	BigInt operator<< (int operand) const {

		BigInt result{};

		int shift_storage_elements_val{ operand/storage_el_bit_length };
		int shift_bits_val{ operand%storage_el_bit_length };

		STORAGE_TYPE all_ones{ ~STORAGE_TYPE{0} };

		for(int i{StorageNum-1}; i>=shift_storage_elements_val; --i){
			auto mask1 = all_ones>>shift_bits_val;
			auto mask2 = ~mask1;
			(*result.m_parr)[i] |= 
				( ((*m_parr)[i-shift_storage_elements_val] & mask1) ) << shift_bits_val;
			if(i-shift_storage_elements_val-1 >= 0)
				(*result.m_parr)[i] |=
					((*m_parr)[i-shift_storage_elements_val-1] & mask2) >> (storage_el_bit_length - shift_bits_val);
		}

		return result;
	}

	BigInt operator + (const BigInt& rhs) const {

		BigInt result{};

		for(auto i{0}; i<BigInt::StorageNum; ++i){
			
			// check_for_wraparound, if wraparound happened -> carry = 1
			if((i != BigInt::StorageNum-1)){
				if(
					((*rhs.m_parr)[i]==max_el_value && (*result.m_parr)[i] == 1) 					// (*rhs.m_parr)[i] + (*result.m_parr)[i]) may wrap around, so check if it wraps here, if it wraps -> then carry = 1 automatically
					|| ( (max_el_value - (*m_parr)[i] < (*rhs.m_parr)[i] + (*result.m_parr)[i]) )
				)
					(*result.m_parr)[i+1] += 1;
			}

			(*result.m_parr)[i] += (*m_parr)[i] + (*rhs.m_parr)[i];
		}

		return result;
	}

	BigInt operator - (const BigInt& rhs) const {

		BigInt result{};

		BigInt::STORAGE_TYPE carry{ 0 }; 
		for(auto i{0}; i<BigInt::StorageNum; ++i){
			
			(*result.m_parr)[i] = (*m_parr)[i] - (*rhs.m_parr)[i] - carry;

			carry = 0;

			// check_for_wraparound, if wraparound happened -> carry = 1
			if(i != StorageNum-1)
				if(
					((*rhs.m_parr)[i] == max_el_value && carry == 1)
					|| (*m_parr)[i] < (*rhs.m_parr)[i] + carry
				)
					carry = 1;
		}

		return result;
	}

	// use halves of StorageType
	//  because havle * havle fits in whole StorageType 
	//   but whole * whole - doesn't
	BigInt operator * (const BigInt& rhs) const {
		
		BigInt result{};		
		STORAGE_TYPE tmp{0};

		int half_bitsize_of_storage_type = storage_el_bit_length / 2; // (( x/8 ) * 2) = x/4 if 8|x

		for(auto i{0}; i<StorageNum*2; ++i){

			// iterate over every half-element
			STORAGE_TYPE carry{0};
			for(int j{0}; j<StorageNum*2; ++j){

				// condition, j+i < storagenum*2 -> skip everything
				if(i+j < StorageNum*2){

					tmp =( i%2 == 0 ? ((*m_parr)[i/2] & ls_half_mask) : ((*m_parr)[i/2] >> half_bitsize_of_storage_type) )
						* ( j%2 == 0 ? ((*rhs.m_parr)[j/2] & ls_half_mask) : ((*rhs.m_parr)[j/2] >> half_bitsize_of_storage_type) )
						+ ( (i+j)%2 == 0 ? ((*result.m_parr)[(i+j)/2] & ls_half_mask) : ((*result.m_parr)[(i+j)/2] >> half_bitsize_of_storage_type) )
						+ carry;

					if((i+j)%2 == 0)
						(*result.m_parr)[(i+j)/2] = ((*result.m_parr)[(i+j)/2] & ms_half_mask) | (tmp & ls_half_mask);
					else
						(*result.m_parr)[(i+j)/2] = ((*result.m_parr)[(i+j)/2] & ls_half_mask) | (tmp << half_bitsize_of_storage_type);
					
					carry = tmp >> half_bitsize_of_storage_type;
				}				
			}
		}

		return result;
	}

private:
	// need this 2 functions for division:

	void set_bit(int bit_index) {
		if(bit_index >= max_bit_length)
			return;
		(*m_parr)[bit_index/storage_el_bit_length] |= STORAGE_TYPE{1} << (bit_index%storage_el_bit_length);
	}

	// linear search for index of MSB
	unsigned int get_payload_bit_length() const {
		for(int i{StorageNum-1}; i>=0; --i)
			if((*m_parr)[i]!=0)
				for(int j{storage_el_bit_length-1}; j>=0; j--)
					if((*m_parr)[i] >> j)
						return i*storage_el_bit_length + j + 1;
		return 0;
	}

public:

	std::array<BigInt, 2> division(const BigInt& rhs) const { 
		
		// [?]handle zero division, or maybe there is no point
		BigInt quotient{};
		BigInt remainder{*this};
		int k = rhs.get_payload_bit_length();

		BigInt tmp{};

		while(remainder >= rhs){
			int t = remainder.get_payload_bit_length();
			tmp = rhs << (t - k);
			if(remainder < tmp){
				--t;
				tmp = rhs << (t-k);
			}
			remainder = remainder - tmp;
			quotient.set_bit(t-k);
		}

		return std::array<BigInt, 2>{quotient, remainder};
	}

	BigInt operator / (const BigInt& rhs) const {
		return division(rhs)[0];
	}

	BigInt operator % (const BigInt& rhs) const {
		return division(rhs)[1];
	}

	bool get_bit(unsigned int bit_index) const {
		return ((*m_parr)[bit_index/storage_el_bit_length] & (STORAGE_TYPE{1}<<(bit_index%storage_el_bit_length))) != 0;
	}

	BigInt pow(const BigInt& exp) const {
		BigInt result{"1"};

		// DEBUG("mantissa = " << this->get_as_string(false, true) << "\n exp = " << exp.get_as_string(false, true) << '\n');

		// DEBUG("bitlength = " << exp.get_payload_bit_length() << '\n');

		for(int i{exp.get_payload_bit_length()-1}; i>=0; --i){
			if(exp.get_bit(i)){
				result = result * (*this); 
				// DEBUG("res after * mantisa: " << result.get_as_string() << '\n');
			}
			if(i != 0)
				result = result * result;
			// DEBUG("res after ^2: " << result.get_as_string() << '\n');
		}

		return result;
	}

private:

	ComparisonOption comparison_helper(const BigInt& rhs) const{
		for(auto i{StorageNum-1}; i>=0; --i){
			if((*m_parr)[i] > (*rhs.m_parr)[i])
				return ComparisonOption::greater; // this is bigger
			else if ((*m_parr)[i] < (*rhs.m_parr)[i])
				return ComparisonOption::smaller; // rhs is bigger
		}
		return ComparisonOption::equal; // they are equal
	}

public:

	// [?][TODO] test comparison functions:
	// since defuse.sa has no >,=,< ... -> utilize "-" operation -> minus sign as indicator
	bool operator > (const BigInt& rhs) const {
		return comparison_helper(rhs) == ComparisonOption::greater ? true : false;
	}

	bool operator ==(const BigInt& rhs) const {
		return comparison_helper(rhs) == ComparisonOption::equal ? true : false;
	}

	bool operator < (const BigInt& rhs) const {
		return comparison_helper(rhs) == ComparisonOption::smaller ? true : false;
	}

	bool operator >= (const BigInt& rhs) const {
		auto tmp{ comparison_helper(rhs) };
		return tmp == ComparisonOption::greater || tmp == ComparisonOption::equal ? true : false;
	}

	bool operator <= (const BigInt& rhs) const {
		auto tmp{ comparison_helper(rhs) };
		return tmp == ComparisonOption::smaller || tmp == ComparisonOption::equal ? true : false;
	}

	std::string get_as_string(bool prepend_with_0x = false, bool upper_case = false) const {
		
		STORAGE_TYPE zero_storage_el{0};

		std::stringstream ss{};

		if(prepend_with_0x)
			ss << "0x";

		if(upper_case)
			ss << std::uppercase;

		bool leading_zeros{true};
		bool first_encountered{false};
		for(auto it{m_parr->rbegin()}, end{m_parr->rend()}; it!=end; ++it){
			
			leading_zeros = *it != 0 ? false : leading_zeros;

			if(!leading_zeros){

				// if it is the most significant element, dont prepend zeros,
				// prepend otherwise
				if(!first_encountered){
					first_encountered = true;
					ss << std::hex << *it;
				}
				else{
					ss << std::hex << std::setfill('0') << std::setw(StorageSize*2) << *it;
				}
			}
		}

		std::string result{ss.str()};

		// check if string was empty/"0x", if it was -> element is all 0's
		return (result.empty() || result == "0x") ? result+="0" : result;
	}

	// [!] need to make TestUtils friend -> 
	//  so that it can generate_test_data_vec
	//   (test_data_vec writes into private member of BigInt)
	template<typename _U, int __bit_length> friend class TestUtils;

};
#endif