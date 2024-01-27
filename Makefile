.DEFAULT_GOAL := test

# cpp_flags := -std=c++98 -W{all,extra,error} -g -fsanitize=undefined
cpp_flags := -std=c++98 -g -fsanitize=undefined

.PHONY : test
test : objects/test ; ./objects/test

objects/test : $(addprefix objects/, $(addsuffix .o, test parse)) Makefile
	g++ $(cpp_flags) $(filter %.o, $^) -o $@

objects/main : $(addprefix objects/, $(addsuffix .o, Utils Server main Client parse Command Channel)) Makefile
	c++ $(cpp_flags) $(filter %.o, $^) -o $@

objects/%.o : sources/%.cpp Makefile
	c++ $(cpp_flags) -c -o $@ $<

generated_makefiles/%.mk : sources/%.cpp Makefile
	c++ \
		$(cpp_flags) \
		-M `# Generate a makefile` \
		-MM `# Don't mention system dependencies` \
		-MP `# Make a phony target for all headers` \
		$< > $@ \
	;

all_source_names :
	echo $(all_sources)

all_sources := $(shell cd sources; printf '%s\n' *.cpp | sed 's/\.cpp$$//g' | tr '\n' ' '; echo)
all_generated_makefiles := $(addprefix generated_makefiles/, $(addsuffix .mk, $(all_sources)))
all_objects := $(addprefix objects/, $(addsuffix .o, $(all_sources)))

extra_executables := test

.PHONY : clean
clean :
	rm -f $(all_generated_makefiles) $(all_objects) $(addprefix objects/, $(extra_executables))

-include $(all_generated_makefiles)
