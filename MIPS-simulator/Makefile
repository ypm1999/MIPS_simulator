
# 目标文件
TARGET=code
SRCS = main.cpp

OBJS = $(SRCS:.c=.o)

$(TARGET):$(OBJS)
	g++ -c Memory.cpp -std=c++14 -Wall -Wextra  -O0 -pthread
	g++ -c MipsParser.cpp -std=c++14 -Wall -Wextra  -O0 -pthread
	g++ -c MipsSimulator.cpp -std=c++14 -Wall -Wextra  -O0 -pthread
	g++ -c main.cpp -std=c++14 -Wall -Wextra  -O0 -pthread
	g++ Memory.o MipsParser.o MipsSimulator.o main.o -o code -std=c++14 -Wall -Wextra -O0 -pthread
# -O1 -Wall -Wextra -Wconversion
#	@echo TARGET:$@
#	@echo OBJECTS:$^
#	$(CC) -o $@ $^

clean:
	rm -rf code
