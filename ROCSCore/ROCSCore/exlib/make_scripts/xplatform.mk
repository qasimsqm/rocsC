# arg 1 is the output name, arg2 is the list of object files, arg3 is a list of any other
# link commands, like -lexlib
CXX_LINK_LIB?=-lc++
ARCH = -arch_only x86_64
ifeq ($(TARGET_PLATFORM),Darwin)

ADDRESS_SANITIZER ?= 0
ifeq ($(ADDRESS_SANITIZER), 1)
STATIC_EXE_FLAGS := $(CXXFLAGS) -g -fsanitize=address
CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer -I/usr/local/opt/llvm/include
LDFLAGS=-L/usr/local/otp/llvm/lib
CXX=/usr/local/Cellar/llvm/3.3/bin/clang++
else
STATIC_EXE_FLAGS = $(CXXFLAGS)
endif

STATIC_CMD = libtool -static $(ARCH) -o $(1) $(2)
DYNAMIC_CMD = libtool -dynamic $(ARCH) -o $(1) $(2) $(CXX_LINK_LIB) -lsystem -L$(BUILD_TARGET_DIR) $(3)
else ifeq ($(TARGET_PLATFORM),Linux)
STATIC_CMD = ar rcs -o $(1) $(2)
DYNAMIC_CMD = $(CXX) -shared -Wl,-soname=$(notdir $(1)) -o $(1) $(2) -lc++ -L$(BUILD_TARGET_DIR) $(3)
else ifeq ($(TARGET_PLATFORM),Windows)
$(error 'You must define STATIC_CMD and DYNAMIC_CMD for Windows in exlib/make_scripts/xplatform.mk')
endif

ifeq ($(MAKECMDGOALS),dynamic)
    ifneq ($(TARGET_PLATFORM), Darwin)
        export CXXFLAGS += -fPIC
    else
	    export CXXFLAGS += -dynamic
	endif
endif


