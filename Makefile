.PHONY: testBigInt, testCalc

testBigInt: test_BigInt

test_BigInt: test_BigInt.o
	g++ test_BigInt.o -std=c++17 -g -fsanitize=address -o  test_BigInt

test_BigInt.o: test_BigInt.cpp TestUtils.h BigInt.h
	g++ test_BigInt.cpp -std=c++17 -c -o test_BigInt.o

testCalc: test_Calc

test_Calc: test_Calc.o
	g++ test_Calc.o -std=c++17 -g -fsanitize=address -o test_Calc

test_Calc.o:  test_Calc.cpp calculator.h BigInt.h
	g++ test_Calc.cpp -std=c++17 -DDEBUG_BUILD -c -o test_Calc.o


clean:
	rm -rf *.o test_BigInt test_Calc