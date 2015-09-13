CFLAGS := -fPIC -O3 -g -Wall -std=c++11
CC := g++
NAME := sketches

SRCS = hash.cpp random.cpp sketches.cpp # source files
OBJS = $(SRCS:.cpp=.o)

.PHONY: all
all: lib

lib: lib$(NAME).so

lib$(NAME).so: $(OBJS)
	$(CC) -shared -Wl,-soname,lib$(NAME).so $^ -o $@

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^

install: lib$(NAME).so
	cp lib$(NAME).so /usr/local/lib/

.PHONY: clean
clean:
	$(RM) *.o *.so*
