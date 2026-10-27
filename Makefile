NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iinclude

# for linter only
HEADERS =

SRCS = src/main.cpp \

TESTS_SRCS = tests/test_example.cpp\

OBJS = $(SRCS:src/%.cpp=obj/%.o)
DEPS = $(OBJS:.o=.d)
TESTS = $(TESTS_SRCS:tests/%.cpp=tests/bin/%.out)
TESTS_OBJS = $(filter-out obj/main.o,$(OBJS))

.PHONY: all clean fclean re test_cgi tests

all: $(NAME)
debug:
	@echo $(SRCS)
	@echo
	@echo $(OBJS)
	@echo
	@echo $(TESTS)
	@echo

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

tests: $(TESTS_OBJS) $(TESTS)

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

-include $(DEPS)

run_tests: $(TESTS_OBJS) $(TESTS)
	@TESTS_BINARIES="$(TESTS)" ./tests/run_all_tests.sh

tests/bin/%.out: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TESTS_OBJS) $< -o $@

test_cgi: tests/bin/test_cgi.out
	./tests/bin/test_cgi.out

clean:
	rm -rf obj
	rm -rf tests/bin

fclean: clean
	rm -f $(NAME)

re: fclean all

lint:
	clang-tidy $(HEADERS) $(SRCS) -- $(CXXFLAGS) -Iinclude
	clang-format --dry-run --Werror $(SRCS) $(HEADERS)

fix-lint:
	clang-tidy --fix $(SRCS) -- $(CXXFLAGS) -Iinclude
	clang-format -i $(SRCS) $(HEADERS)
