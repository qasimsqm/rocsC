all_target = run_tests

include common.mk

INCLUDES += -I$(ROCSDIR)/packager
SRC_DIR = $(testdir)/Serialization
STATIC_OBJ_DIR = $(SRC_DIR)/build

test_src = serialization_tests.cpp
test_obj = $(addprefix $(STATIC_OBJ_DIR)/, $(test_src:.cpp=.o))

dependencies = $(test_obj:.o=.d)

include $(EXLIB_DIR)/make_scripts/make_auto_depend

.PHONY: run_tests
run_tests: $(test_obj) $(required_libs)
	@echo serialization.mk run_tests
	@$(CXX) $(INCLUDES) $(ARCH) $(CXXFLAGS) $(required_libs) \
		-o $(STATIC_OBJ_DIR)/run_tests $(test_obj) \
		$(ROCSDIR)/packager/create_show.cpp -F$(FRAMEWORKS) -framework CoreFoundation \
		
	@echo Running serialization_tests
	@$(STATIC_OBJ_DIR)/run_tests


to_clean = $(test_obj) $(dependencies) $(STATIC_OBJ_DIR)/run_tests

.PHONY: clean
clean:
	-@$(RM) $(to_clean) 2> /dev/null