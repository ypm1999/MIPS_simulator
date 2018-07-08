# 指令编译器和选项


# 目标文件
TARGET=code
SRCS = main.cpp

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS)
	g++ Memory.cpp MipsParser.cpp MipsSimulator.cpp main.cpp -o code -std=c++11 -O1 -Wall -Wextra -Wconversion
#	@echo TARGET:$@
#	@echo OBJECTS:$^
#	$(CC) -o $@ $^

clean:
	rm -rf code
