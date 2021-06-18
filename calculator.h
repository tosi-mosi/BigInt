#ifndef CALCULATOR
#define CALCULATOR

#include "BigInt.h"
#include <functional>
#include <deque>
#include <stack>

#ifdef DEBUG_CALCULATOR
#  define DEBUG(x) std::cerr << x;
#else
#  define DEBUG(x) do {} while (0)
#endif

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

	// [?] place for comparison ops
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



// [?] maybe I won't need this
enum class IntegerType{
	decimal,
	hexidemical
};

// [?] maybe I won't need it
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


// Parses expression and evaluates them by calling BigInt operators:
// 1) Tokenize?
// 2) convert to postfix with shunting yard alg
// 3) evaluate expression from postfix notation
template<typename BI>
class Calculator{

public:

	struct Operator{
		const OperatorId id;
		const std::string strRepr;
		const OperatorPrecedence prec;
		const OperatorAssociativity assoc;

		// [?]
		const std::function<BI(const BI*, const BI&)> func;
	};

	static std::vector<Operator> mOperators;

	// [?] could be bigInt / usual
	class Integer{

	public:

		// [?] can't use string_view here, 
		// because for evaluate_postfix Integer has to own its mStrRepr
		std::string mStrRepr; 				// it will be pointing to the original expression's string
											//  update: no it won't 
											//   (cant use string_view, because it is necessary for Integer to own )
		BI mIntRepr;
		Integer(std::string_view strRepr, BI intRepr):
			mStrRepr{strRepr}, mIntRepr{intRepr} 
		{}
	};

	class Token{

	// [?] should it be public?
	public:

		TokenType mTokenType; 					// operator or integer

		Operator* mOp; 							// if token is operator,
												//	this will be a ptr to corresponding operator obj,
												// 	 nullptr otherwise
		
		std::shared_ptr<Integer> mInt;			// same here, but with Integer

		Token(TokenType tokenType, Operator* op, std::shared_ptr<Integer> inte) :
			mTokenType{ tokenType },
			mOp{ op },
			mInt{ inte }
		{}

	};

	static std::deque<Token> mTokens;

	// [?] maybe shall return Integer ?
	static std::string calculate(std::string input){

		tokenize(input);
		convert_infix_to_postfix_notation();
		return evaluate_postfix().mIntRepr.get_as_string();

	}

private:

	static Integer evaluate_postfix() {

		std::stack<Token> operand_stack;

		for(auto it{mTokens.begin()}, end{mTokens.end()}; it!=end; ++it){

			if(it->mTokenType == TokenType::integer)
				operand_stack.push(*it);
			else{

				if(operand_stack.size() < 2)
					throw std::runtime_error("invalid postfix notation expr");

				Token rightOperand{ operand_stack.top() };
				operand_stack.pop();
				Token leftOperand{ operand_stack.top() };
				operand_stack.pop();

				BI subResult = it->mOp->func(&leftOperand.mInt->mIntRepr, rightOperand.mInt->mIntRepr);

				operand_stack.emplace(
					TokenType::integer,
					nullptr, 
					std::make_shared<Integer>("", subResult)
				);
			}
		}

		if(operand_stack.size() != 1)
			throw std::runtime_error("invalid postfix notation expr");

		// initialize strRepr for result
		Integer exprResult{ *operand_stack.top().mInt };
		exprResult.mStrRepr = exprResult.mIntRepr.get_as_string();

		DEBUG("expr result = " << exprResult.mStrRepr);

		mTokens.clear();

		return exprResult; 
	}

	// [?] When to validate expression (), maybe when we will be evaluating 
	// using shunting yard algorithm from WIKI
	static bool convert_infix_to_postfix_notation(){

		std::deque<Token> result;
		std::stack<Token> operator_stack;

		for(auto it{mTokens.begin()}, end{mTokens.end()}; it!=end; ++it){
			
			if(it->mTokenType == TokenType::integer){

				result.push_back(*it);

			}else if(it->mOp->strRepr == "("){

				operator_stack.push(*it);

			}else if(it->mOp->strRepr == ")"){

				// if not "(" then put it to the deque
				while(true){
					
					if(operator_stack.empty())
						return false;

					Token operator_inside_parentheses{ operator_stack.top() };

					if(operator_inside_parentheses.mOp->strRepr == "("){
						operator_stack.pop();
						break;
					}

					result.push_back(operator_inside_parentheses);
					operator_stack.pop();
				
				}

			}else{

				if(!operator_stack.empty()){
					
					Token* top_el{};

					// decide whether to pop operators from stack, before pushing current operator on to it(depends on precedence and associativity):
					while(
						!operator_stack.empty() 																				// check if stack is not empty
						&& (top_el = &operator_stack.top())																		// read top element(can't be nullptr here)
						&& top_el->mOp->strRepr != "("																			// it shouldn't be left parentheses
						&& (
							top_el->mOp->prec > it->mOp->prec 																	// if in stack operator's precedence is higher
							|| (top_el->mOp->prec == it->mOp->prec && top_el->mOp->assoc == OperatorAssociativity::left)		// if current in stack operator == current operator, and they are left associative
						)
					){

						result.push_back(*top_el);
						operator_stack.pop();

					}
				}
				
				operator_stack.push(*it);

			}

		}

		// Popping the remaining items from operator stack to output queue
		while(!operator_stack.empty()){
			if(Token tmp{ operator_stack.top() }; tmp.mOp->strRepr != "(")
				result.push_back(operator_stack.top());
			operator_stack.pop();
		}

		mTokens = result;

		DEBUG("postfix notation:");
        for(auto& el: mTokens)
        	DEBUG((el.mOp ? el.mOp->strRepr : el.mInt->mIntRepr.get_as_string()) << ' ');
    	DEBUG('\n');

		return true;
	}

	static std::string initializeOperationCombination(){
		std::string res{};
        for(const Operator& el: mOperators)
            res += el.strRepr;
        return res;
	}

	// [?] should whitespaces be allowed?
	// [?] should it catch exceptions here?, 
	// maybe somewhere higher on stack in one place handle all exceptions
	static bool tokenize(std::string_view expr) {

		// Initializing string that is a combination of strReprs of all operators: "+-/*"
		// Need to initialize it only once -> that is why it is static local
		static std::string operatorsCombined{ initializeOperationCombination() };

    	for(auto it_a{expr.begin()}, end{expr.end()}; it_a!=end; ){
    		
    		DEBUG("index = " 
    			<< it_a - expr.begin() 
    			<< ", starts_with_operator = " 
    			<< starts_with_operator(std::string_view{it_a, end-it_a})
    			<< '\n');

    		//operators are delimiters, ints are meaningful entities
			if(auto operatorId = starts_with_operator(std::string_view{it_a, end-it_a}); operatorId != -1){
			
				mTokens.emplace_back(
					TokenType::operatorr, &mOperators[operatorId], nullptr
				);
				it_a += mOperators[operatorId].strRepr.length();
			
			}else{
			
				// end_of_number_index = (end of number - begining_of_number)
				size_t number_end_index = expr.find_first_of(operatorsCombined, it_a-expr.begin());
				
				DEBUG("br2) number_end_index = " << number_end_index << '\n');
				
				size_t number_str_length = (number_end_index == std::string::npos) ? end-it_a : number_end_index-(it_a-expr.begin());
				
				DEBUG("br2) number_str_length = " << number_str_length << '\n');

				std::string_view strRepr{ 
					it_a,
					number_str_length
				};


				BI intRepr{strRepr};
				mTokens.emplace_back(
					TokenType::integer, nullptr,  std::make_shared<Integer>(strRepr, intRepr)
				);

				it_a += number_str_length;
			}
        }

        DEBUG("tokens:\n");
        for(auto& el: mTokens)
        	DEBUG((el.mOp ? el.mOp->strRepr : el.mInt->mIntRepr.get_as_string()) << ' ');
    	DEBUG('\n');

        return true;
	}

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
std::vector<typename Calculator<BI>::Operator> Calculator<BI>::mOperators{ 
	Calculator<BI>::Operator{OperatorId::leftBracket, "(", OperatorPrecedence::none, OperatorAssociativity::none},
	Calculator<BI>::Operator{OperatorId::rightBracket, ")", OperatorPrecedence::none, OperatorAssociativity::none},
	Calculator<BI>::Operator{OperatorId::plus, "+", OperatorPrecedence::low, OperatorAssociativity::left, &BI::operator +},
	Calculator<BI>::Operator{OperatorId::minus, "-", OperatorPrecedence::low, OperatorAssociativity::left, &BI::operator -},
	Calculator<BI>::Operator{OperatorId::multiply, "*", OperatorPrecedence::medium, OperatorAssociativity::left, &BI::operator *},
	Calculator<BI>::Operator{OperatorId::divide, "/", OperatorPrecedence::medium, OperatorAssociativity::left, &BI::operator /},
	
	// [?] ^ or **.
	// ** -> could be problematic because there already is *
	// ** -> when encountered * will have to make extra check
	Calculator<BI>::Operator{OperatorId::exponentiate, "^", OperatorPrecedence::high, OperatorAssociativity::right, &BI::pow}
};

template<typename BI>
std::deque<typename Calculator<BI>::Token> Calculator<BI>::mTokens{};

#endif