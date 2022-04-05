NAME := tester

SHELL := /bin/bash
CXX := clang++
CXXFLAGS = -std=c++11 -MMD -MP -DDEBUG -g -fsanitize=undefined -fsanitize=address
# CXXFLAGS = -std=c++11 -MMD -MP -DDEBUG -g -fsanitize=integer -fsanitize=address -fprofile-arcs -ftest-coverage
LIBS := -lpthread
INCLUDES := -I. -I../includes


ifeq ($(shell uname), Linux)
	lcov_op :=  --gcov-tool ./llvm-gcov.sh
endif

# ==== Enter latest release URL ==== #
GTEST_URL := https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
GTEST_RELEASE := googletest-$(basename $(basename $(notdir $(GTEST_URL))))

SRCDIR := ../srcs
OBJDIR := ../objs
MAIN_SUFFIX := main.cpp
SRCFILE := $(filter-out %$(MAIN_SUFFIX),$(shell find $(SRCDIR) -name "*.cpp" -type f))
OBJS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCFILE:.cpp=.o))
DEPS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(SRCFILE:.cpp=.d))

# ==== Path to test ==== #
TESTDIR := ../tests
TOBJDIR := tobjs
TESTS := $(shell find $(TESTDIR) -name "*.cpp" -type f)
TOBJS = $(patsubst $(TESTDIR)%,$(TOBJDIR)%,$(TESTS:.cpp=.o))
TDEPS = $(patsubst $(TESTDIR)%,$(TOBJDIR)%,$(TESTS:.cpp=.d))

GTSRCDIR := gtest
GTOBJDIR := gobjs
.SECONDARY: $(GTESTSRC)
GTESTSRC := $(GTSRCDIR)/gtest-all.cc $(GTSRCDIR)/gtest_main.cc
GTOBJS   := $(patsubst $(GTSRCDIR)%,$(GTOBJDIR)%,$(GTESTSRC:.cc=.o))


all: $(NAME)
-include $(DEPS) $(TDEPS)

$(NAME): $(OBJS) $(GTOBJS) $(TOBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $@

run: $(NAME)
	./$(NAME) || :
	$(RM) $(NAME)

$(GTOBJDIR)/%.o: $(GTSRCDIR)/%.cc
	@mkdir -p $(GTOBJDIR)
	$(CXX) $(CXXFLAGS) -c $< $(INCLUDES) -o $@

$(TOBJDIR)/%.o: $(TESTDIR)/%.cpp
	@mkdir -p $(TOBJDIR)/$(*D)
	$(CXX) $(CXXFLAGS) -c $< $(INCLUDES) -o $@

$(GTESTSRC): $(GTSRCDIR)
	@:

$(GTSRCDIR): $(GTEST_RELEASE)
	python3 $(GTEST_RELEASE)/googletest/scripts/fuse_gtest_files.py .
	cp $(GTEST_RELEASE)/googletest/src/gtest_main.cc gtest/gtest_main.cc
	$(RM) -r $(GTEST_RELEASE)

$(GTEST_RELEASE):
	if [ ! -f $(notdir $(GTEST_URL)) ]; then curl -OL $(GTEST_URL); fi
	tar -xzf $(notdir $(GTEST_URL)) $(GTEST_RELEASE) && $(RM) $(notdir $(GTEST_URL))

cav: CXXFLAGS += -fprofile-arcs -ftest-coverage
cav: re
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

.PHONY: all run cav clean fclean re
