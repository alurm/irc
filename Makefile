.DEFAULT_GOAL := test

cpp_flags := -std=c++98 -W{all,extra,error} -g -fsanitize=undefined

test : $(addprefix objects/, $(addsuffix .o, test)) Makefile
	c++ $(cpp_flags) $(filter %.o, $^) -o $@

validation : $(addprefix objects/, $(addsuffix .o, validation Server parse Client Channel State Base Utils Pass  )) Makefile
	c++ $(cpp_flags) $(filter %.o, $^) -o $@

objects/%.o : %.cpp Makefile
	c++ $(cpp_flags) -c -o $@ $<

generated_makefiles/%.mk : %.cpp Makefile
	c++ \
		$(cpp_flags) \
		-M `# Generate a makefile` \
		-MM `# Don't mention system dependencies` \
		-MP `# Make a phony target for all headers` \
		$< > $@ \
	;

sources_without_extension := Client Server after_parsing_stub dispatch m parse test validation
generated_makefiles := $(addprefix generated_makefiles/, $(addsuffix .mk, $(sources_without_extension)))
objects := $(addprefix objects/, $(addsuffix .o, $(sources_without_extension)))

.PHONY : clean
clean :
	rm -f $(generated_makefiles) $(objects)

-include $(generated_makefiles)
