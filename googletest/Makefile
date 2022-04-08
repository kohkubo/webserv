NAME := tester

SHELL := /bin/bash
CXX := clang++
CXXFLAGS = -std=c++11 -MMD -MP -DDEBUG -g -fsanitize=undefined -fsanitize=address
LIBS := -lpthread -lgtest_main -lgtest
INCLUDES := -I./includes -I../includes
LIBINCLUDES = -L./$(LIBDIR)

ifeq ($(shell uname), Linux)
	lcov_op := --gcov-tool ./llvm-gcov.sh
endif

LIBDIR := lib
SRCDIR := ../srcs
OBJDIR := ../objs
MAIN_FILE := main.cpp
SRCFILE := $(filter-out %$(MAIN_FILE),$(shell find $(SRCDIR) -name "*.cpp" -type f))
OBJS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCFILE:.cpp=.o))
DEPS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCFILE:.cpp=.d))

# ==== Path to test ==== #
TESTDIR := ../tests
TOBJDIR := tobjs
TESTS := $(shell find $(TESTDIR) -name "*.cpp" -type f)
TOBJS = $(patsubst $(TESTDIR)%,$(TOBJDIR)%,$(TESTS:.cpp=.o))
TDEPS = $(patsubst $(TESTDIR)%,$(TOBJDIR)%,$(TESTS:.cpp=.d))

all: $(NAME)
-include $(DEPS) $(TDEPS)

$(NAME):$(LIBDIR) $(OBJS) $(TOBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(TOBJS) $(INCLUDES) $(LIBINCLUDES) $(LIBS) -o $@

run: $(NAME)
	./$(NAME) || :
	$(RM) $(NAME)

$(TOBJDIR)/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(TOBJDIR)/$(*D)
	$(CXX) $(CXXFLAGS) -c $< $(INCLUDES) -o $@

$(LIBDIR):
	git clone --depth=1 https://github.com/google/googletest.git
	mkdir -p googletest/build includes
	cmake -S ./googletest -B ./googletest/build
	$(MAKE) -C googletest/build
	cp -r googletest/build/lib ./$(LIBDIR)
#	cp -r googletest/googlemock/include/gmock ./includes/gmock
	cp -r googletest/googletest/include/gtest ./includes/gtest
	$(RM) -r googletest

cov: CXXFLAGS += -fprofile-arcs -ftest-coverage
cov: re
	./$(NAME)
	lcov -c -b . -d . -o cov_test.info $(lcov_op)
	genhtml cov_test.info -o cov_test
	$(RM) $(NAME) cov_test.info
	$(RM) $(TOBJS) $(TOBJDIR)/*.gcda $(TOBJDIR)/*.gcno
	open cov_test/index-sort-f.html

clean:
	$(RM) $(TOBJS) $(TOBJDIR)/*.gcda $(TOBJDIR)/*.gcno
	$(RM) -r $(TOBJDIR)
	$(RM) -r cov_test

fclean: clean
	$(RM) $(NAME)

re : fclean all

.PHONY: all run cov clean fclean re
