NAME = ircserv

RED = \033[1;7;31m
GREEN = \033[1;7;32m
YELLOW = \033[1;7;33m
BLUE = \033[1;7;34m
MAGENTA = \033[1;3;35m
CYAN = \033[0;36m
RESET = \033[0m

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP
CPPFLAGS = -Iinclude
LDFLAGS =
LDLIBS =

RM = rm -rf

SOURCES_DIR = ./src
SOURCES_LIST =	main.cpp \
				Server.cpp \
				ClientManager.cpp \
				Client.cpp \
				ChannelManager.cpp \
				Channel.cpp \
				commands/Command.cpp \
				commands/CommandManager.cpp \
				commands/response.cpp \
				commands/INVITE.cpp \
				commands/JOIN.cpp \
				commands/KICK.cpp \
				commands/MessageBuffer.cpp \
				commands/MODE.cpp \
				commands/NICK.cpp \
				commands/PART.cpp \
				commands/PASS.cpp \
				commands/PING.cpp \
				commands/PRIVMSG.cpp \
				commands/QUIT.cpp \
				commands/TOPIC.cpp \
				commands/USER.cpp \
				utils/port.cpp \
				utils/split.cpp \
				utils/trailing.cpp \


SOURCES = $(addprefix $(SOURCES_DIR)/, $(SOURCES_LIST))

BONUS_SOURCES_LIST =	bonus/main.cpp \
						commands/MessageBuffer.cpp \
						commands/Command.cpp \
						commands/CommandManager.cpp \
						Server.cpp \
						ClientManager.cpp \
						Client.cpp \
						ChannelManager.cpp \
						Channel.cpp \
						commands/Command.cpp \
						commands/CommandManager.cpp \
						commands/response.cpp \
						commands/INVITE.cpp \
						commands/JOIN.cpp \
						commands/KICK.cpp \
						commands/MessageBuffer.cpp \
						commands/MODE.cpp \
						commands/NICK.cpp \
						commands/PART.cpp \
						commands/PASS.cpp \
						commands/PING.cpp \
						commands/PRIVMSG.cpp \
						commands/QUIT.cpp \
						commands/TOPIC.cpp \
						commands/USER.cpp \
						utils/port.cpp \
						utils/split.cpp \
						utils/trailing.cpp \

BONUS_SOURCES = $(addprefix $(SOURCES_DIR)/, $(BONUS_SOURCES_LIST))
BONUS_OBJECTS_LIST = $(patsubst %.cpp, %.o, $(BONUS_SOURCES_LIST))
BONUS_OBJECTS = $(addprefix $(OBJECTS_DIR)/, $(BONUS_OBJECTS_LIST))
BONUS_OBJECTS_DIRS = $(sort $(dir $(OBJECTS)))
BONUS_NAME = ircbot

OBJECTS_DIR = objects
OBJECTS_LIST = $(patsubst %.cpp, %.o, $(SOURCES_LIST))
OBJECTS = $(addprefix $(OBJECTS_DIR)/, $(OBJECTS_LIST))
OBJECTS_DIRS = $(sort $(dir $(OBJECTS) $(BONUS_OBJECTS)))

all: $(NAME)

bonus: $(BONUS_NAME)

$(NAME): $(OBJECTS)
	@echo "$(YELLOW) Building $(BLUE) IRC $(YELLOW) program... $(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	@echo "$(GREEN) Done $(RESET)\n"

$(BONUS_NAME): $(BONUS_OBJECTS)
	@echo "$(YELLOW) Building $(BLUE) IRC $(YELLOW) program... $(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	@echo "$(GREEN) Done $(RESET)\n"

$(OBJECTS_DIR)/%.o: $(SOURCES_DIR)/%.cpp | $(OBJECTS_DIRS)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@
	@echo "$(GREEN) OBJECTS ADDED $(RESET)\n"

clean:
	@echo "$(RED) Deleting objects files... $(RESET)\n"
	@$(RM) $(OBJECTS_DIR)

fclean: clean
	@echo "$(RED) Cleaning built program... $(RESET)\n"
	@$(RM) -f $(NAME) $(BONUS_NAME) $(OBJECTS_DIR)

re:
	@$(MAKE) -s fclean
	@$(MAKE) -s all
	@echo "$(GREEN) DELETED AND RECOMPILED $(RESET)\n"

-include $(OBJECTS:.o=.d)

$(OBJECTS_DIRS):
	@mkdir -p $@
	@echo "$(GREEN) Create objects folder $(RESET)\n"

.PHONY: all bonus clean fclean re
