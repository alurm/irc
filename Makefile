.DEFAULT_GOAL := test

.PHONY : test
test :
	c++ -W{all,extra,error} -fsanitize=undefined test_parse.cpp -o test
	./test
