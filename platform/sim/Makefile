CC=g++
CPPFLAGS=-Wall -Werror -Iinclude/ -std=c++0x
LDFLAGS=-rdynamic
LDLIBS=-pthread -ldl

OBJS=main.o module.o 

hive: $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o hive $(OBJS) $(LDLIBS)

clean:
	rm -f $(OBJS)
