.PHONY: testBigInt, all

testBigInt: test

test: test_BigInt.o
	g++ test_BigInt.o -std=c++17 -g -fsanitize=address -o  test

test_BigInt.o: test_BigInt.cpp
	g++ test_BigInt.cpp -std=c++17 -c -o test_BigInt.o

all: test

test: test_Calc.o
	g++ test_Calc.o -std=c++17 -g -fsanitize=address -o test

test_Calc.o: 

clean:
	rm -rf *.o test