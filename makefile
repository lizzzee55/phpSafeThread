CC = g++ -std=c++11 -w
SRCS := $(wildcard ./WebThreadSafe/*.cpp)
OBJS := $(patsubst %cpp,%o,$(SRCS))


all : $(OBJS)
	$(CC) -g -o ./Release/WebThreadSafe $^ -lpthread

%.o: %.cpp
	$(CC) -g -c  $<    -o   $@     -DLINUX

clean :
	rm ./WebThreadSafe/*.o
	