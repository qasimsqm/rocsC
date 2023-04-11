all_target = run_tests

include common.mk

SRC_DIR = $(testdir)/ChangeLog
STATIC_OBJ_DIR = $(SRC_DIR)/build

test_src = read_marker_tests.cpp
test_obj = $(addprefix $(STATIC_OBJ_DIR)/, $(test_src:.cpp=.o))

dependencies = $(test_obj:.o=.d)

include $(EXLIB_DIR)/make_scripts/make_auto_depend

.PHONY: run_tests
run_tests: $(lib_name) $(test_obj) $(required_libs)
	@$(CXX) $(INCLUDES) $(ARCH) $(CXXFLAGS) $(LDFLAGS) $(required_libs) \
		-o $(testdir)/ChangeLog/run_tests $(testdir)/Sources/run_tests.cpp $(test_obj)
	@echo Running read_marker_tests
	@$(testdir)/ChangeLog/run_tests


to_clean = $(test_obj) $(dependencies) $(testdir)/ChangeLog/run_tests

.PHONY: clean
clean:
	-@$(RM) $(to_clean) 2> /dev/null
