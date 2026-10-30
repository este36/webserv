NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iinclude

LIB_FT_SRCS =\
	src/ft/HashFn.cpp \
	src/ft/Profiler.cpp \
	src/ft/StringBuilder.cpp \
	src/ft/string_view.cpp \

SRCS =\
	$(LIB_FT_SRCS) \
	src/main.cpp \
	src/Config.cpp \
	src/LocationConfig.cpp \
	src/ServerConfig.cpp \
	src/ConfigParser.cpp \

TESTS_SRCS =\
	tests/test_lib.cpp\

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
