NAME = webserv
CXX = clang++
CXXFLAGS = -Wall -Werror -Wextra -Wshadow -MMD -MP -std=c++98 -I$(includes) -g

includes = ./includes
srcsdir = srcs
objsdir = objs
srcs = $(shell find $(srcsdir) -name "*.cpp" -type f)
objs = $(patsubst $(srcsdir)%,$(objsdir)%,$(srcs:.cpp=.o))
deps = $(patsubst $(srcsdir)%,$(objsdir)%,$(srcs:.cpp=.d))

# == path of googletest dir ==
gtestdir := googletest

.PHONY: all
all: $(NAME)

$(NAME): $(objs)
	$(CXX) $(CXXFLAGS) $^ -o $(NAME)

$(objsdir)/%.o: $(srcsdir)/%.cpp
	@mkdir -p $(objsdir)/$(*D)
	@$(CXX) $(CXXFLAGS) -c $< $(INCLUDES) -o $@

.PHONY: test
test: $(objs)
	$(MAKE) -C $(gtestdir) run

.PHONY: cav
cav: $(objs)
	$(MAKE) -C $(gtestdir) cav

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

-include $(depends)
