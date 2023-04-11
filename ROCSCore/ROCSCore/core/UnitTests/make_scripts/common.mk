include common_.mk

$(testinglib): FORCE
	@echo making $(testinglib)
	@$(MAKE) -s -C $(testdir)/make_scripts -f testing.mk $(testinglib)
	
required_libs = $(lib_rocs) $(testinglib)