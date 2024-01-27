NAME := ircserv

$(NAME) : $(addprefix objects/, $(addsuffix .o, Utils Server main Client parse Command Channel)) Makefile
	c++ $(cpp_flags) $(filter %.o, $^) -o $@

.PHONY : all
all : $(NAME)

.PHONY : clean
clean :
	rm -f $(all_generated_makefiles) $(all_objects) $(addprefix objects/, $(extra_executables))

.PHONY: fclean
fclean : clean
	rm -f $(NAME)

.PHONY : re
re :
	make fclean
	make
