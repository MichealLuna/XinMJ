#makefile for xin/base/tests
.SUFFIXES: .h .cc .o .sh .cpp .c

AR 			= ar
AR_FLAGS	= crs

CXX			= g++
CXXFLAGS	= -std=c++11 -ggdb3 -Wall -Werror
CXX_MACRO	= 

BASE_DIR 	= ../../../
INC_DIR		= $(BASE_DIR)
SRC_DIR		= ./
OBJ_DIR		= obj
LIB_DIR		= lib
BIN_DIR		= bin

XIN_INC		= -I$(INC_DIR)
XIN_LIB		= -L$(LIB_DIR) -lxin_base -lpthread

#GTEST_INC	= -I$(BASE_DIR)/xin/other
#GTEST_LIB	= -L$(BASE_DIR)/xin/other/gtest/lib -lgtest

INC			= $(XIN_INC)
LIB			= $(XIN_LIB)

XINLIB		= libxin_base.a
BIN			= Timestamp_test

all: debug release

debug release gprof: dir $(addprefix $(LIB_DIR)/,$(XINLIB)) $(addprefix $(BIN_DIR)/,$(BIN))

dir:
	@-mkdir -p $(INC_DIR)
	@-mkdir -p $(SRC_DIR)
	@-mkdir -p $(OBJ_DIR)
	@-mkdir -p $(BIN_DIR)
	@-mkdir -p $(LIB_DIR)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXX_MACRO) -o $@ $(INC) -c $<

$(OBJ_DIR)/%.o:$(BASE_DIR)/xin/base/%.cc
	$(CXX) $(CXXFLAGS) $(CXX_MACRO) -o $@ $(INC) -c $<

$(LIB_DIR)/libxin_base.a:$(addprefix $(OBJ_DIR)/,Thread.o Exception.o Timestamp.o)
	$(AR) $(AR_FLAGS) $@ $^

$(BIN_DIR)/Exception_test:$(addprefix $(OBJ_DIR)/,Exception_test.o)
	$(CXX) $(CXXFLAGS) $(CXX_MACRO) -o $@ $^ $(LIB)

$(BIN_DIR)/Thread_test:$(addprefix $(OBJ_DIR)/,Thread_test.o)
	$(CXX) $(CXXFLAGS) $(CXX_MACRO) -o $@ $^ $(LIB)

$(BIN_DIR)/Timestamp_test:$(addprefix $(OBJ_DIR)/,Timestamp_test.o)
	$(CXX) $(CXXFLAGS) $(CXX_MACRO) -o $@ $^ $(LIB)

strip:
	strip $(addprefix $(BIN_DIR)/,$(BIN))

clean:
	rm -rf $(LIB_DIR)
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)

test:
	gdb $(addprefix $(BIN_DIR)/,$(BIN))