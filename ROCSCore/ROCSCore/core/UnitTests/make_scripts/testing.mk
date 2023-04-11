# uses environment variable ROCS_CORE, the path to the rocs_core directory,

include common_.mk

SRC_DIR = $(testdir)/Sources
STATIC_OBJ_DIR = $(SRC_DIR)/Build

testlib_src =	rocs_events_compare.cpp
				
testlib_obj = $(addprefix $(STATIC_OBJ_DIR)/, $(testlib_src:.cpp=.o))

testing_src =	test_testing.cpp \
				run_tests.cpp
				
testing_obj = $(addprefix $(STATIC_OBJ_DIR)/, $(testing_src:.cpp=.o))


dependencies = $(testlib_obj:.o=.d) $(testing_obj:.o=.d)

include $(EXLIB_DIR)/make_scripts/make_auto_depend

$(testinglib): $(testlib_obj)
	@mkdir -p $(@D)
	@libtool -static -o $(testinglib) $(testlib_obj)


$(testing_exe): $(testing_obj) $(testinglib) $(lib_rocs)
	@echo making $(testing_exe)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(ARCH) $(INCLUDES) $(LDFLAGS) \
	    -o $(testing_exe) $(testing_obj) $(testinglib) $(lib_rocs) \
	    -F$(FRAMEWORKS) -framework CoreFoundation
	@echo Running $(testing_exe)
	@$(testing_exe)

to_clean = $(testlib_obj) $(dependencies) $(testinglib) $(testing_exe) $(testing_obj)

.PHONY: clean
clean:
	-@$(RM) $(to_clean) 2> /dev/null