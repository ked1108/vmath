CC = gcc

CFLAGS = -pedantic -Wall -Wextra -lm -ggdb


SRCS = main.c vmath.h

OUT = balls

all: $(OUT)

$(OUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS) 

clean:
	rm -f $(OUT)

install:
	install -m 755 $(OUT) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(OUT)
