NAME = webserv
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -Wshadow -MMD -MP -std=c++98 -I$(includes) -g -fsanitize=address -fsanitize=undefined -ftrivial-auto-var-init=pattern -Wtautological-compare -Wsign-compare -gfull -fstandalone-debug -gdwarf-5 -gz

includes = srcs
srcsdir = srcs
objsdir = objs
srcs = $(shell find $(srcsdir) -name "*.cpp" -type f)
objs = $(srcs:$(srcsdir)%.cpp=$(objsdir)%.o)
deps = $(srcs:$(srcsdir)%.cpp=$(objsdir)%.d)

# == path of googletest dir ==
gtestdir := googletest

# == path of integrationtest dir ==
itestdir := integrationtest

.PHONY: all
all: $(NAME)

$(NAME): $(objs)
	$(CXX) $(CXXFLAGS) $^ -o $(NAME)

$(objsdir)/%.o: $(srcsdir)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: test
test:
	$(MAKE) -C $(gtestdir) run

.PHONY: cav
cav: $(objs)
	$(MAKE) -C $(gtestdir) cav

.PHONY: itest
itest: $(NAME)
	$(MAKE) -C $(itestdir) run

.PHONY: itest_all
itest_all: $(NAME)
	$(MAKE) -C $(itestdir) run_all

.PHONY: gotest
gotest:
	$(MAKE) -C $(itestdir) gotest

.PHONY: wtest
wtest: test itest

.PHONY: clean
clean:
	$(RM) $(objs) $(deps)
	$(RM) -r $(objsdir)
	$(MAKE) -C $(gtestdir) clean

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(gtestdir) fclean

.PHONY: re
re: fclean all

.PHONY: prepush
prepush:
	zsh .github/pre-push

# 改修予定
gbench	=	./test/benchmark
benchdir = ./gbench

benchflg = clang++ -std=c++11 -O2
benchflg2 = $(benchdir)/gbench.cpp \
	-isystem $(gbench)/include \
	-L$(gbench)/build/src -lbenchmark -lpthread \
	-I$(gtestdir) -I/usr/local/opt/llvm/include -I$(includes) -I$(benchdir) -o benchmark

.PHONY: bench
bench:
	$(benchflg) \
	$(benchflg2) \
	./benchmark \
	--benchmark_out_format=csv \
	--benchmark_out=benchmark.csv # --benchmark_filter=BM_VectorInsert_input_iterator

-include $(deps)
