include $(EXLIB_DIR)/make_scripts/xplatform.mk
SRC_DIR = $(EXLIB_DIR)/Sources
BUILD_DIR = $(BUILD_TARGET_DIR)
STATIC_OBJ_DIR = $(BUILD_STATIC_DIR)/$(EXLIB_DIR_NAME)
DYNAMIC_OBJ_DIR = $(BUILD_DYNAMIC_DIR)/$(EXLIB_DIR_NAME)

INCLUDES = -I$(EXCELERANDO_DIR) -I$(THIRD_PARTY_INCLUDES)
# CXXFLAGS += -pthread

static_lib = $(BUILD_TARGET_DIR)/$(EXLIB_STATIC_NAME)
dynamic_lib = $(BUILD_TARGET_DIR)/$(EXLIB_DYLIB_NAME)

src =	string_gen.cpp \
		string_lib.cpp \
		binary_string_io.cpp \
		format.cpp \
		numeric_string_compare.cpp \
		uuid.cpp \
		path.cpp \
		listdir.cpp \
		ex_errno.cpp \
		time_stamp.cpp \
		iostreambuf.cpp \
		id3.cpp \
		cout_buffer.cpp \
		thread_names.cpp \
		ex_lock.cpp \
        hash.cpp \
		log.cpp
		
static_objects = $(addprefix $(STATIC_OBJ_DIR)/, $(src:.cpp=.o))
shared_objects = $(addprefix $(DYNAMIC_OBJ_DIR)/, $(src:.cpp=.o))
static_dependencies = $(static_objects:.o=.d) 
shared_dependencies = $(shared_objects:.o=.d)

static: $(static_lib)

dynamic: $(dynamic_lib)

include $(EXLIB_DIR)/make_scripts/make_auto_depend

link_depends =

$(static_lib): $(static_objects)
	@mkdir -p $(@D)
	@echo Creating $(static_lib)...
	$(SHOW)$(call STATIC_CMD,$(static_lib),$(static_objects))
	
$(dynamic_lib): $(shared_objects)
	@mkdir -p $(@D)
	@echo Creating $(dynamic_lib)...
	$(SHOW)$(call DYNAMIC_CMD,$(dynamic_lib),$(shared_objects),$(link_depends))

to_clean = $(dynamic_lib) $(static_lib) $(static_objects) $(shared_objects) \
            $(static_dependencies) $(shared_dependencies)

.PHONY: clean	
clean:
	-@$(RM) $(to_clean) 2> /dev/null
