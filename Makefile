.POSIX:
CFLAGS = -std=c99 -pedantic -D_POSIX_C_SOURCE=200809L -Wall -Wextra
LDLIBS = -lpthread

.c:
		$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)
