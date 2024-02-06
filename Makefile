# School 42 requirements: $(NAME), all, clean, fclean, re.

NAME := ircserv

$(NAME) : $(addprefix objects/, $(addsuffix .o, Utils Server main Client parse Socket Command Channel)) Makefile
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

# The rest.

.DEFAULT_GOAL := $(NAME)

cpp_flags := -std=c++98 -W{all,extra,error}

# -g -fsanitize=undefined -ferror-limit=1

objects/test : $(addprefix objects/, $(addsuffix .o, test parse)) Makefile
	c++ $(cpp_flags) $(filter %.o, $^) -o $@

objects/%.o : sources/%.cpp Makefile
	c++ $(cpp_flags) -c -o $@ $<

objects/%.mk : sources/%.cpp Makefile
	c++ \
		$(cpp_flags) \
		-M `# Generate a makefile` \
		-MM `# Don't mention system dependencies` \
		-MP `# Make a phony target for all headers` \
		-MT objects/$*.o \
		$< > $@ \
	;

all_sources := $(shell cd sources; printf '%s\n' *.cpp | sed 's/\.cpp$$//g' | tr '\n' ' '; echo)
all_generated_makefiles := $(addprefix objects/, $(addsuffix .mk, $(all_sources)))
all_objects := $(addprefix objects/, $(addsuffix .o, $(all_sources)))

extra_executables := test

-include $(all_generated_makefiles)
