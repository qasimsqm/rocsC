include $(EXLIB_DIR)/make_scripts/xplatform.mk
SRC_DIR = $(ROCS_CORE_DIR)/Sources
BUILD_DIR = $(BUILD_TARGET_DIR)
STATIC_OBJ_DIR = $(BUILD_STATIC_DIR)/$(ROCS_CORE_DIR_NAME)
DYNAMIC_OBJ_DIR = $(BUILD_DYNAMIC_DIR)/$(ROCS_CORE_DIR_NAME)
TEST_OBJ_DIR = $(STATIC_OBJ_DIR)/test

INCLUDES = \
	-I$(EXCELERANDO_DIR) \
	-I$(THIRD_PARTY_INCLUDES) \
	-I$(ROCS_DIR) \
	-I$(GTEST_INCLUDE_DIR)

static_lib = $(BUILD_TARGET_DIR)/$(ROCS_CORE_STATIC_NAME)
dynamic_lib = $(BUILD_TARGET_DIR)/$(ROCS_CORE_DYLIB_NAME)

src =   changelog/copy_rocs_event.cpp \
		changelog/change_log.cpp \
		changelog/cl_events.cpp \
		changelog/cl_sequences.cpp \
		changelog/editor.cpp \
		changelog/editor_history.cpp \
		changelog/read_marker.cpp \
		changelog/song_log.cpp \
		changelog/change_log_version.cpp \
		common/attacca_defs.cpp \
		common/codes.cpp \
		common/filter_filename.cpp \
		common/key_sigs.cpp \
		common/parse_filename.cpp \
		common/rocs_event.cpp \
		common/rocs_file_header.cpp \
		common/rocs_version.cpp \
		common/warnings.cpp \
		common/file_version.cpp \
		rocs_midi/groups.cpp \
		rocs_midi/show_data.cpp \
		rocs_midi/song_data.cpp \
		rocs_midi/voice_data.cpp \
		rocs_midi/voice_event.cpp \
		rocs_midi/voice_track.cpp \
		rocs_midi/show_data_version.cpp \
		std_midi/meta_messages.cpp \
		std_midi/midi_file.cpp \
		std_midi/midi_packet.cpp \
		std_midi/midi_track.cpp \
		std_midi/status_bytes.cpp \
		std_midi/utility.cpp \
		std_midi/voice_message.cpp \
		timeline/midi_loaders.cpp \
		timeline/timeline.cpp \
		timeline/tl_events.cpp

test_src = \
	core_tests.cpp \
	tl_events_tests.cpp \
	editor_tests.cpp

test_exe = $(BUILD_TARGET_DIR)/core_tests.o

static_objects = $(addprefix $(STATIC_OBJ_DIR)/, $(src:.cpp=.o))
shared_objects = $(addprefix $(DYNAMIC_OBJ_DIR)/, $(src:.cpp=.o))
static_dependencies = $(static_objects:.o=.d) 
shared_dependencies = $(shared_objects:.o=.d)

test_objects = $(addprefix $(TEST_OBJ_DIR)/, $(test_src:.cpp=.o))
test_dependencies = $(test_objects:.o=.d)

dynamic: $(dynamic_lib)

static: $(static_lib)

include $(EXLIB_DIR)/make_scripts/make_auto_depend

link_depends =	-L$(BUILD_TARGET_DIR) \
				-L$(THIRD_PARTY_LINK_DIR) \
			    $(EXLIB_LINK_NAME)

$(static_lib): $(static_objects)
	@mkdir -p $(@D)
	@echo Creating $(static_lib)...
	$(SHOW)$(call STATIC_CMD,$(static_lib),$(static_objects))
	
$(dynamic_lib): $(shared_objects)
	@mkdir -p $(@D)
	@echo Creating $(dynamic_lib)...
	$(SHOW)$(call DYNAMIC_CMD,$(dynamic_lib),$(shared_objects),$(link_depends))

test: $(test_objects) $(dynamic_lib)
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(test_exe) $(test_objects) $(link_depends) \
	$(dynamic_lib) -lgtest

check: test
	$(test_exe)

check_debug: test
	lldb -- $(test_exe) --gtest_catch_exceptions=0 --gtest_break_on_failure=1

to_clean = \
	$(dynamic_lib) \
	$(static_lib) \
	$(static_objects) \
	$(shared_objects) \
	$(test_objects) \
	$(test_exe) \
    $(static_dependencies) \
	$(shared_dependencies) \
	$(test_dependencies)

clean:
	-@$(RM) $(to_clean) 2> /dev/null
