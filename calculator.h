#include "BigInt.h"
#include <deque>
#include <stack>

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

	//function
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

	class Token{

	// [?] should it be public?
	public:

		TokenType mTokenType; 					// operator or identifier

		Operator* mOp; 							// if token is operator,
												//	this will be a ptr to corresponding operator obj,
												// 	 nullptr otherwise
		
		std::shared_ptr<Integer> mInt;			// unique_ptr here?

		Token(TokenType tokenType, Operator* op, std::shared_ptr<Integer> inte) :
			mTokenType{ tokenType },
			mOp{ op },
			mInt{ inte }
		{}

		// Token(const Token& to_be_copied) :
		// 	mTokenType{ to_be_copied.mTokenType },
		// 	mOp{ to_be_copied.mOp},					// shallow copy suffices
		// 	mInt{  }		// need deep copy here, no way around
		// {}

	};

	static std::deque<Token> mTokens;

	static bool calculate(std::string_view& expr){
		// tokenize
		// parse tokenized
		if(!tokenize(expr))
			return false;

		if(!parse_tokenized_expr())
			return false;

		//calculate, return result

	}

	static bool parse_tokenized_expr() {

		

	}

	// [?] When to validate expression (), maybe when we will be evaluating 
	// using shunting yard algorithm from WIKI
	static bool convert_infix_to_postfix_notation(){

		std::deque<Token> result;
		std::stack<Token> operator_stack;

		for(auto it{mTokens.begin()}, end{mTokens.end()}; it!=end; ++it){
			
			if(it->mTokenType == TokenType::integer){

				result.push_back(*it);

			}else if(it->mTokenType == TokenType::operatorr){

				if(!operator_stack.empty()){
					
					Token* top_el{};
					while(
						!operator_stack.empty() 				// check if stack is not empty
						&& (top_el = &operator_stack.top())		// read top element
						&& top_el->mOp->strRepr != "("
						&& (
							top_el->mOp->prec > it->mOp->prec 
							|| (top_el->mOp->prec == it->mOp->prec && top_el->mOp->assoc == OperatorAssociativity::left)
						)
					){

						result.push_back(*top_el);
						operator_stack.pop();

					}
				}
				
				operator_stack.push(*it);

			}else if(it->mTokenType == TokenType::operatorr && it->mOp->strRepr == "("){

				operator_stack.push(*it);

			}else if(it->mTokenType == TokenType::operatorr && it->mOp->strRepr == ")"){

				// if not "(" then put it to the deque
				while(true){
					
					if(operator_stack.empty())
						return false;

					Token operator_inside_parentheses{ operator_stack.top() };

					if(operator_inside_parentheses.mOp->strRepr != "("){
						operator_stack.pop();
						break;
					}

					result.push_back(operator_inside_parentheses);
					operator_stack.pop();
				
				}
			}
		}

		// Popping the remaining items from operator stack to output queue
		while(!operator_stack.empty()){
			if(Token tmp{ operator_stack.top() }; tmp.mOp->strRepr != "(")
				result.push_back(operator_stack.top());
			operator_stack.pop();
		}

		DEBUG("end of convert\n");

		mTokens = result;

		return true;
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
        		
        		DEBUG("index = " 
        			<< it_a - expr.begin() 
        			<< ", starts_with_operator = " 
        			<< starts_with_operator(std::string_view{it_a, end-it_a}));

        		//operators are delimiters, ints are meaningful entities
				if(auto operatorId = starts_with_operator(std::string_view{it_a, end-it_a}); operatorId != -1){
				
					mTokens.push_back(
						Token{ TokenType::operatorr, &mOperators[operatorId], nullptr }
					);
					it_a += mOperators[operatorId].strRepr.length();
				
				}else{
				
					// end_of_number_index = (end of number - begining_of_number)
					size_t number_end_index = expr.find_first_of(operatorsCombined, it_a-expr.begin());
					
					DEBUG("br2) number_end_index = " << number_end_index);
					
					size_t number_str_length = (number_end_index == std::string::npos) ? end-it_a : number_end_index-(it_a-expr.begin());
					
					DEBUG("br2) number_str_length = " << number_str_length);

					std::string_view strRepr{ 
						it_a,
						number_str_length
					};


					BI intRepr{strRepr};
					mTokens.push_back(
						Token{ TokenType::integer, nullptr,  std::make_shared<Integer>(strRepr, intRepr) }
					);

					it_a += number_str_length;
				}
	        }

        }catch(std::runtime_error& e){

    		// [?] handle input validation exceptions here
        	std::cout << "Aborting calculation. Reason: " << e.what() << '\n'; 
        	return false;

        }		

        for(auto& el: mTokens)
        	std::cout << (el.mOp ? el.mOp->strRepr : el.mInt->mIntRepr.get_as_string()) << ' ';
    	std::cout << '\n'; 

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
std::deque<typename Calculator<BI>::Token> Calculator<BI>::mTokens{};