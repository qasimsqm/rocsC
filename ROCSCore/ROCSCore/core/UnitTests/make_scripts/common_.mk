# common definitions for ROCS UnitTest makefiles

testdir = $(ROCSDIR)/UnitTests
INCLUDES = $(ROCS_INC) -I$(testdir)
LDFLAGS = -lUnitTest++
ARCH = -arch x86_64

lib_rocs = $(ROCS_CORE)/librocs_core.a
testinglib = $(testdir)/libtesting.a
testing_exe = $(testdir)/bin/test_testing

all_target ?= $(testing_exe)

all: $(all_target)

$(lib_rocs): FORCE
	@echo making $(lib_rocs)
	@$(MAKE) -C $(ROCS_CORE)/make_scripts -f $(ROCS_CORE)/make_scripts/lib_rules.mk
	

FORCE:

