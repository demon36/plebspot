export CC := gcc
export CXX := g++
ARCH :=$(shell getconf LONG_BIT)
BUILD := debug
SRC_DIR := ./src
OBJ_DIR := ./obj/$(ARCH)/$(BUILD)
INC_DIR := ./include
BIN_DIR := ./bin/$(ARCH)/$(BUILD)
LIB_DIR := ./lib/$(ARCH)/$(BUILD)
DEP_DIR := ./dep
COV_REPORTS_DIR := ./cov
DOCS_DIR := ./docs
TEST_SRC_DIR := ./test
TEST_BIN_DIR := ./bin/test/$(ARCH)
PROJECT_NAME := $(shell basename $(CURDIR))
MAJOR_VERSION := 0
MINOR_VERSION := 1.9

DEP_SRC_FILES := thirdparty/cpp-base64/base64.cpp
DEP_SRC_DIRS := thirdparty/fmt/src thirdparty/captcha/src thirdparty/pugixml/src thirdparty/hoedown/src
DEP_CFLAGS := -Ithirdparty/cpp-httplib -Ithirdparty/fmt/include -Ithirdparty/hoedown/src -Ithirdparty/Mustache \
	-Ithirdparty/captcha/include -Ithirdparty/plusaes/include -Ithirdparty/cpp-base64 -Ithirdparty/pugixml/src

SRC_FILES := $(shell test -d $(SRC_DIR) && find $(SRC_DIR) $(DEP_SRC_DIRS) -regex '.*\.\(c\|cc\|cpp\|cxx\)') $(DEP_SRC_FILES)
TEST_SRC_FILES := $(shell test -d $(TEST_SRC_DIR) && find $(TEST_SRC_DIR) -regex '.*\.\(c\|cc\|cpp\|cxx\)')
OBJ_FILES := $(patsubst %,$(OBJ_DIR)/%.o,$(SRC_FILES))
TEST_OBJ_FILES := $(patsubst $(TEST_SRC_DIR)/%,$(OBJ_DIR)/$(TEST_SRC_DIR)/%.o,$(TEST_SRC_FILES))
DEP_FILES := $(patsubst %,$(DEP_DIR)/%.dep,$(SRC_FILES)) $(patsubst $(TEST_SRC_DIR)/%,$(DEP_DIR)/$(TEST_SRC_DIR)/%.dep,$(TEST_SRC_FILES))

SO_FILE := $(PROJECT_NAME).so
A_FILE := $(PROJECT_NAME).a
ifeq ($(OS),Windows_NT)
	EXEC_FILE := $(PROJECT_NAME).exe
else
	EXEC_FILE := $(PROJECT_NAME)
endif

TEST_FILE := main_test

#todo: only use fpermissive with thirdparty libraries
CFLAGS := -m$(ARCH) -Wall -g -std=c++17 -I$(INC_DIR) -fpermissive#-Wconversion -Werror
CFLAGS_DEBUG := -DDEBUG
CFLAGS_RELEASE := -O3 -w -DNDEBUG
INC := $(DEP_CFLAGS)
LIBS := -lpthread -lssl -lcrypto
ifeq ($(OS),Windows_NT)
	LIBS += -lws2_32 
endif

LDFLAGS := -m$(ARCH) -static-libstdc++
SO_LDFLAGS := -shared -Wl,-zdefs,-soname,$(SO_FILE).$(MAJOR_VERSION),-rpath,'$$ORIGIN'
TEST_LDFLAGS := -L$(LIB_DIR) -l:$(SO_FILE) -Wl,-rpath,'$$ORIGIN/lib:$$ORIGIN/dep:$$ORIGIN/../../../$(LIB_DIR)'

ifeq ($(BUILD),debug)
	CFLAGS += $(CFLAGS_DEBUG)
else ifeq ($(BUILD),release)
	CFLAGS += $(CFLAGS_RELEASE)
	SO_DBG_FILE := $(SO_FILE).dbg
	EXEC_DBG_FILE := $(EXEC_FILE).dbg
else ifeq ($(BUILD),coverage)
	CFLAGS += --coverage
	LDFLAGS += --coverage
else
$(error "allowed BUILD values are debug, release, coverage")
endif

all: depend exec

shared: $(LIB_DIR)/$(SO_FILE)

static: $(LIB_DIR)/$(A_FILE)

exec: $(BIN_DIR)/$(EXEC_FILE)

test: shared $(TEST_BIN_DIR)/$(TEST_FILE)

run: test
	$(TEST_BIN_DIR)/$(TEST_FILE)
ifeq ($(BUILD),coverage)
	lcov --quiet -c --directory . --output-file $(OBJ_DIR)/.info --no-external
	genhtml --quiet $(OBJ_DIR)/.info --output-directory $(COV_REPORTS_DIR)
	xdg-open $(COV_REPORTS_DIR)/index.html
endif

$(OBJ_DIR)/%.o: %
	@mkdir -p $(@D) $(DEP_DIR)/$(<D)
	$(CXX) $(CFLAGS) $(INC) -fPIC -c -o $@ $< -MMD -MF $(DEP_DIR)/$<.dep

$(OBJ_DIR)/$(TEST_SRC_DIR)/%.o: $(TEST_SRC_DIR)/%
	@mkdir -p $(@D) $(DEP_DIR)/$(<D)
	$(CXX) $(CFLAGS) $(INC) -c -o $@ $< -MMD -MF $(DEP_DIR)/$<.dep

$(LIB_DIR)/$(SO_FILE): $(OBJ_FILES)
	@mkdir -p $(@D)
	$(CXX) -g -o $@.$(MAJOR_VERSION).$(MINOR_VERSION) $(OBJ_FILES) $(LDFLAGS) $(SO_LDFLAGS) $(LIBS)
	ln -sf ./$(SO_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(SO_FILE).$(MAJOR_VERSION)
	ln -sf ./$(SO_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(SO_FILE)

install: all
	cp $(BIN_DIR)/$(EXEC_FILE) '${HOME}/bin/'

$(LIB_DIR)/$(A_FILE): $(OBJ_FILES)
	@mkdir -p $(@D)
	ar rcs $@.$(MAJOR_VERSION).$(MINOR_VERSION) $^
	ln -sf ./$(A_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(A_FILE).$(MAJOR_VERSION)
	ln -sf ./$(A_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(A_FILE)

$(BIN_DIR)/$(EXEC_FILE): $(OBJ_FILES)
	@mkdir -p $(@D)
	$(CXX) -g $^ -o $@.$(MAJOR_VERSION).$(MINOR_VERSION) $(LDFLAGS) $(LIBS)
	ln -sf ./$(EXEC_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(BIN_DIR)/$(EXEC_FILE).$(MAJOR_VERSION)
	ln -sf ./$(EXEC_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(BIN_DIR)/$(EXEC_FILE)

%.dbg: %
	objcopy --only-keep-debug $< $@
	objcopy --strip-unneeded $< $<
	objcopy --add-gnu-debuglink=$< $@

$(TEST_BIN_DIR)/$(TEST_FILE): $(TEST_OBJ_FILES)
	@mkdir -p $(@D)
	$(CXX) -g $^ -o $@ $(LDFLAGS) $(TEST_LDFLAGS) $(LIBS) 

init:
	mkdir -p $(SRC_DIR) $(INC_DIR) $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(DEP_DIR) $(TEST_SRC_DIR) $(TEST_BIN_DIR) $(DOCS_DIR)

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ_FILES) \
	$(LIB_DIR)/$(SO_FILE) $(LIB_DIR)/$(SO_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(SO_FILE).$(MAJOR_VERSION) $(LIB_DIR)/$(SO_FILE).dbg \
	$(LIB_DIR)/$(A_FILE) $(LIB_DIR)/$(A_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(A_FILE).$(MAJOR_VERSION) $(LIB_DIR)/$(A_FILE).dbg \
	$(BIN_DIR)/$(EXEC_FILE) $(BIN_DIR)/$(EXEC_FILE).$(MAJOR_VERSION).$(MINOR_VERSION) $(LIB_DIR)/$(EXEC_FILE).$(MAJOR_VERSION) $(BIN_DIR)/$(EXEC_FILE).dbg \
	$(TEST_BIN_DIR)/$(TEST_FILE) $(DEP_FILES)

.PHONY: init all shared static exec run clean depend

-include $(shell test -d $(DEP_DIR) && find $(DEP_DIR) -name '*.dep')

#todo: add an extra target for the symlink itself
#todo: add target cleanall
#todo: add rebuild target
#todo: add section for other makefile deps
#todo: support pkg-config
#todo: support excluding source files from build
#todo: make test should link test executable with obj files directly
#todo: allow lib users to use #include <mylib/myheader.h>
#todo: separate private headers from public headers
#todo: use obj files for building a test binary instead of relying on a static or a dynamic lib
#todo: use a custom program entry point for building a test for an executable
#todo: fix issue that happens to debug symbols when you make release after a successful make release
