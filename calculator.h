

#include <BigInt.h>


//[TODO] after creating Calculator -> should test BigInts for memory leaks (Valgrind?)


// Parses expression, starts operatoins
// Tokenize?
// convert to postfix with shunting yard alg
// using stack? structure calculate result of postfix 
template<typename BI>
class Calculator{

	struct OperationData{

	};

	// [TOTHINK] output from one operation -> could be operand for another
	// have to create some tree-like structure, that would be calculated from bottom to top?

	// here maybe something like a vector of (operation, operand1, operand2)

	// parsing (() (() ())) maybe connected with binary trees
	// 1) [?] 3 types of precedence(+, *, **)?
	static bool parse_expression() {



	}

};