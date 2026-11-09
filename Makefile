NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g
INCLUDES = -Iinclude

LIB_FT_SRCS =\
	src/ft/HashFn.cpp \
	src/ft/Profiler.cpp \
	src/ft/StringBuilder.cpp \
	src/ft/string_view.cpp \
	src/ft/utils.cpp \

LIB_HTTP_SRCS =\
	src/http/Message.cpp \
	src/http/Request.cpp \
	src/http/Response.cpp \
	src/http/URL.cpp \

LIB_NET_SRCS =\
	src/net/Fd.cpp \
	src/net/PollTask.cpp \
	src/net/TCPClient.cpp \
	src/net/TCPListener.cpp \
	src/net/PollScheduler.cpp \

SRCS =\
	$(LIB_FT_SRCS) \
	$(LIB_HTTP_SRCS) \
	$(LIB_NET_SRCS) \
	src/main.cpp \
	src/LocationConfig.cpp \
	src/ServerConfig.cpp \
	src/ConfigParser.cpp \
	src/ConfigTokensValidation.cpp \
	src/Server.cpp \
	src/HttpConnexion.cpp \
	src/HttpConnexionFactory.cpp \
	src/RequestParser.cpp \

TESTS_SRCS =\
	tests/test_lib.cpp\
	tests/test_parsing_conf.cpp \
	tests/test_url.cpp\

NET_EXAMPLE_SRCS =\
	src/net/example/EchoTask.cpp\
	src/net/example/EchoTaskFactory.cpp\
	src/net/example/server.cpp\


OBJS = $(SRCS:src/%.cpp=obj/%.o)
DEPS = $(OBJS:.o=.d)
TESTS = $(TESTS_SRCS:tests/%.cpp=tests/bin/%.out)
TESTS_OBJS = $(filter-out obj/main.o,$(OBJS))
NET_EXAMPLE_OBJS = $(filter-out obj/main.o,$(OBJS)) $(NET_EXAMPLE_SRCS:src/%.cpp=obj/%.o)

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

net-example: $(NET_EXAMPLE_OBJS)
	$(CXX) $(CXXFLAGS) $(NET_EXAMPLE_OBJS) -o $@

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@


-include $(DEPS)

run_tests: $(TESTS_OBJS) $(TESTS)
	@TESTS_BINARIES="$(TESTS)" ./tests/run_all_tests.sh

tests/bin/%.out: tests/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TESTS_OBJS) $< -o $@


clean:
	rm -rf obj
	rm -rf tests/bin

fclean: clean
	rm -f $(NAME)

re: fclean all
