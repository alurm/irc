.DEFAULT_GOAL := test

cpp_flags := -std=c++98 -W{all,extra,error} -g -fsanitize=undefined

test : test.cpp Makefile
	c++ $(cpp_flags) test.cpp -o test

validation : validation.cpp Server.cpp parse.cpp Client.cpp Makefile
	c++ $(cpp_flags) $(filter %.cpp, $^) -o $@

%.d : %.cpp
	@ c++ -M -MM -MP $^ > $@

-include validation.d
