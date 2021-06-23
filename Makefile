.PHONY: testBigIntBuild, testCalcBuild

#--------------------------------------------

testBigIntBuild: test_big_int

test_big_int: test_big_int.o
	g++ test_big_int.o -std=c++17 -g -fsanitize=address -o  test_big_int

test_big_int.o: test_big_int.cpp test_utils.h big_int.h
	g++ test_big_int.cpp -std=c++17 -fsanitize=address -DDEBUG_BIG_INT -DDEBUG_CALCULATOR -DDEBUG_TEST_UTILS -c -o test_big_int.o

#--------------------------------------------

testCalcBuild: test_calc

test_calc: test_calc.o
	g++ test_calc.o -std=c++17 -g -fsanitize=address -o test_calc

test_calc.o:  test_calc.cpp calculator.h big_int.h test_utils.h
	#g++ test_calc.cpp -std=c++17 -DDEBUG_BIG_INT -DDEBUG_CALCULATOR -DDEBUG_TEST_UTILS -c -o test_calc.o
	g++ test_calc.cpp -std=c++17 -fsanitize=address -c -o test_calc.o

#--------------------------------------------

clean:
	rm -rf *.o test_big_int test_calc