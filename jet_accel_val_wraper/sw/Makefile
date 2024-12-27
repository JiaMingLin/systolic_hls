# Need to choose which xrt library to link depending on emulation option
ifeq ($(XCL_BUILD_MODE), sw_emu)
	XRT_DEFINES := -DXCL_EMULATION_MODE=sw_emu
	XRT_LIB := -lxrt_swemu
else
ifeq ($(XCL_BUILD_MODE), hw_emu)
	XRT_DEFINES := -DXCL_EMULATION_MODE=hw_emu
	XRT_LIB := -lxrt_hwemu
else
	XCL_BUILD_MODE := hw
	XRT_LIB := -lxrt_core
endif
endif

# Always link xrt_coreutil
XRT_LIB += -lxrt_coreutil

# Set project directory one level above of Makefile directory. $(CURDIR) is a GNU make variable containing the path to the current working directory
PROJDIR := $(realpath $(CURDIR)/..)
SOURCEDIR := $(PROJDIR)
BUILDDIR := $(PROJDIR)/build
OUTPUTDIR := $(PROJDIR)/build/build_$(XCL_BUILD_MODE)

# Name of the final executable
TARGET = vitis_flow.exe

# Decide whether the commands will be shown or not
VERBOSE = TRUE

# DIRS = \
#    drivers/device_interface \
#    drivers/vitis_flow/mm2s \
#    drivers/vitis_flow/s2mm \
#    applications \
#    applications/vitis_flow_objects

DIRS = sw

SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))

# Generate the GCC includes parameters by adding -I before each source folder
INCLUDES = $(foreach dir, $(SOURCEDIRS), $(addprefix -I, $(dir))) $(SQL_INCDIR)
INCLUDES += -I$(XILINX_XRT)/include
INCLUDES += -I${XILINX_HLS}/include
INCLUDES += -I$(XILINX_VITIS)/tps/boost_1_72_0

# Add this list to VPATH, the place make will look for the source files
VPATH = $(SOURCEDIRS)

# Create a list of *.cpp sources in DIRS
SOURCES = $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.cpp)) \
        #   $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.cc)) \
		#   $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.c))

# Define objects for all sources
OBJS := $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.cpp=.o)) \
        # $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.cc=.o)) \
		# $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c=.o))

# Define dependencies files for all objects
DEPS = $(OBJS:.o=.d)

# Name the compiler
CXX = g++
DEFINES	 := -D_UNICODE
CXXFLAGS := -g -std=gnu++11 -fPIC -pthread -D_REENTRANT $(DEFINES) $(XRT_DEFINES)# -Wall -Wextra
LDFLAGS  := -pthread -L$(XILINX_XRT)/lib  -lstdc++ -lm -luuid $(XRT_LIB)

# OS specific part
ifeq ($(OS),Windows_NT)
    RM = del /F /Q
    RMDIR = -RMDIR /S /Q
    MKDIR = -mkdir
    ERRIGNORE = 2>NUL || true
    SEP=\\
else
    RM = rm -rf
    RMDIR = rm -rf
    MKDIR = mkdir -p
    ERRIGNORE = 2>/dev/null
    SEP=/
endif

# Remove space after separator
PSEP = $(strip $(SEP))

# Hide or not the calls depending of VERBOSE
ifeq ($(VERBOSE),TRUE)
    HIDE =
else
    HIDE = @
endif

# Define the function that will generate each rule
define generateRules
$(1)/%.o: %.cpp
	@echo Building $$@
	$(HIDE)$(CXX) $(CXXFLAGS) -c $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
endef

# define generateCCRules
# $(1)/%.o: %.cc
# 	@echo Building $$@
# 	$(HIDE)$(CXX) $(CXXFLAGS) -c $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
# endef

# define generateCRules
# $(1)/%.o: %.c
# 	@echo Building $$@
# 	$(HIDE)$(CXX) $(CXXFLAGS) -c $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
# endef

.PHONY: all clean directories

all: directories $(OUTPUTDIR)/$(TARGET)

$(OUTPUTDIR)/$(TARGET): $(OBJS)
	$(HIDE)echo Linking $@
	$(HIDE)$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJS) -o $(OUTPUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBS)

# Include dependencies
-include $(DEPS)

# Generate rules
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))
# $(foreach targetdir, $(TARGETDIRS), $(eval $(call generateCCRules, $(targetdir))))
# $(foreach targetdir, $(TARGETDIRS), $(eval $(call generateCRules, $(targetdir))))

directories:
	$(HIDE)$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Remove all objects, dependencies and executable files generated during the build
clean:
	$(HIDE)$(RMDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)
	$(HIDE)$(RM) $(OUTPUTDIR)/$(TARGET) $(ERRIGNORE)
	@echo Cleaning done !
