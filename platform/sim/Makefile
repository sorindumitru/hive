CC=g++
CPPFLAGS=-Wall -Werror -Iinclude/ -std=c++0x
LDFLAGS=-rdynamic
LDLIBS=-pthread -ldl

OBJS=main.o module.o 

hive: $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o hive $(OBJS) $(LDLIBS)

test: test/test.o
	$(CC) $(CPPFLAGS) $(LDFLAGS) -shared -fPIC -o test/test.so test/test.o

clean:
	rm -f $(OBJS)
