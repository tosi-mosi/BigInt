#include "BigInt.h"
#include <vector>

enum class OperatorId{
	
	//
	notAnOperator,

	// [?] right operand can be not BigInt for them
	//bitShiftLeft,
	//bitShiftRight,
	
	leftBracket,
	rightBracket,
	plus,
	minus,
	multiply,
	divide,
	exponentiate

	// [TODO] place for comparison ops
};

enum class OperatorPrecedence{
	none,
	low,
	medium,
	high
};

enum class OperatorAssociativity{
	none,
	left,
	right
};

struct Operator{
	const OperatorId id;
	const std::string strRepr;
	const OperatorPrecedence prec;
	const OperatorAssociativity assoc;

	// [?]
	// const void* operator_func_ptr???
};

// [?] maybe I don't need this
enum class IntegerType{
	decimal,
	hexidemical
};

// [?] maybe I don't need it
// need only BigInts and usual ints only when bitshift?
enum class IntegerStorageType{
	big, // if
	usual
};

enum class TokenType{
	operatorr,
	integer
};



//[TODO] after creating Calculator -> should test BigInts for memory leaks (Valgrind?)


// Parses expression, starts operatoins
// Tokenize?
// convert to postfix with shunting yard alg
// using stack? structure calculate result of postfix 
template<typename BI>
class Calculator{

public:

	static std::array<Operator, 7> mOperators;

	// [?] could be bigInt / usual
	struct Integer{
		std::string_view mStrRepr; 				// it will be pointing to the original expression's string
		BI mIntRepr;
		Integer(std::string_view strRepr, BI intRepr):
			mStrRepr{strRepr}, mIntRepr{intRepr} 
		{}
	};

	struct Token{
		TokenType mTokenType; 					// operator or identifier

		Operator* mOp; 							// if token is operator,
												//	this will be a ptr to corresponding operator obj,
												// 	 nullptr otherwise
		
		std::unique_ptr<Integer> mInt;			// unique_ptr here?
	};

	static std::vector<Calculator<BI>::Token> mTokens;

	static bool parse(){
		// tokenize
		// parse tokenized
	}

	static bool parse_tokenized_expr(const std::string_view& expr) {

		// if(!tokenize(expr))
		// 	return false;

		// return true;
	}

	static std::string initializeOperationCombination(){
		std::string res{};
        for(const Operator& el: mOperators)
            res += el.strRepr;
        return res;
	}

	// [?] should whitespaces be allowed?
	static bool tokenize(std::string_view& expr) {

		// Initializing string that is a combination of strReprs of all operators: "+-/*"
		// Need to initialize it only once -> that is why it is static local
		static std::string operatorsCombined{ initializeOperationCombination() };

        // handle BigInt ctor's exception (if invalid input)
        try{

        	for(auto it_a{expr.begin()}, end{expr.end()}; it_a!=end; ){
        		
        		//operators are delimiters, ints are meaningful entities
				if(auto operatorId = starts_with_operator(std::string_view{it_a, end-it_a}); operatorId != -1){
				
					mTokens.push_back(
						Token{ TokenType::operatorr, &mOperators[operatorId], nullptr }
					);
					it_a += mOperators[operatorId].strRepr.length();
				
				}else{
				
					size_t end_of_number_index = expr.find_first_of(operatorsCombined, it_a-expr.begin());
					end_of_number_index = (end_of_number_index == std::string::npos) ? end-it_a : end_of_number_index;
					std::string_view strRepr{ 
						it_a,
						end_of_number_index
					};


					BI intRepr{strRepr};
					mTokens.push_back(
						Token{ TokenType::integer, nullptr,  std::make_unique<Integer>(strRepr, intRepr) }
					);

					it_a += end_of_number_index;
				}
	        }

        }catch(std::runtime_error& e){

        	std::cout << "Aborting calculation. Reason: " << e.what() << '\n'; 
        	return false;

        }		

        for(auto& el: mTokens)
        	std::cout << (el.mOp ? el.mOp->strRepr : el.mInt->mIntRepr.get_as_string()) << '\n'; 

        return true;
	}

private:
	// on success -> return operator's index in mOperators
	// on failure -> return -1
	static int starts_with_operator(std::string_view str){
		for(int i=0, end = mOperators.size(); i<end; ++i)
			if(str.rfind(mOperators[i].strRepr, 0) == 0)
				return i;
		return -1;
	}

};

// [!] it is secure
// won't lead to multiple definitions, because compiler makes sure 
// that templates are instantiated only once
template<typename BI>
std::array<Operator, 7> Calculator<BI>::mOperators{ 
	Operator{OperatorId::leftBracket, "(", OperatorPrecedence::none, OperatorAssociativity::none},
	Operator{OperatorId::rightBracket, ")", OperatorPrecedence::none, OperatorAssociativity::none},
	Operator{OperatorId::plus, "+", OperatorPrecedence::low, OperatorAssociativity::left},
	Operator{OperatorId::minus, "-", OperatorPrecedence::low, OperatorAssociativity::left},
	Operator{OperatorId::multiply, "*", OperatorPrecedence::medium, OperatorAssociativity::left},
	Operator{OperatorId::divide, "/", OperatorPrecedence::medium, OperatorAssociativity::left},
	
	// [?] ^ or **.
	// ** -> could be problematic because there already is *
	// ** -> when encountered * will have to make extra check
	Operator{OperatorId::exponentiate, "^", OperatorPrecedence::high, OperatorAssociativity::right}
};

template<typename BI>
std::vector<typename Calculator<BI>::Token> Calculator<BI>::mTokens{};