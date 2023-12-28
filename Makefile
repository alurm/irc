.DEFAULT_GOAL := test

.PHONY : test
test :
	c++ -std=c++98 -W{all,extra,error} -g -fsanitize=undefined test.cpp -o test
	./test

