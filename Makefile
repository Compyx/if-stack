CC = gcc
LD = $(CC)
CFLAGS = -O3 -g -Wall -Wextra \
	 -Wcast-qual \
	 -Wconversion \
	 -Wformat \
	 -Wformat-security \
	 -Wmissing-prototypes \
	 -Wshadow \
	 -Wsign-compare \
	 -Wstrict-prototypes


PROG = stack-test
OBJS = main.o ifstack.o

all: $(PROG)


$(PROG): $(OBJS)
	$(LD) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(PROG)
