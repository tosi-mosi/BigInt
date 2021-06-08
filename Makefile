all: test

test: test_BigInt.o
	g++ test_BigInt.o -std=c++17 -g -fsanitize=address -o  test

test_BigInt.o: test_BigInt.cpp
	g++ test_BigInt.cpp -std=c++17 -c -o test_BigInt.o

clean:
	rm -rf *.o test