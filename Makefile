CFLAGS=-O2 -Wall -Werror -Wimplicit-fallthrough
SRCS=$(wildcard ./*.c)
HDRS=$(wildcard ./*.h)
OBJS=$(patsubst ./%.c, obj/%.o, $(SRCS))
CC=gcc

mmu: $(OBJS)
	$(CC) $(CFLAGS) -lm -o $@ $^ $(LDFLAGS)

$(OBJS): obj/%.o: ./%.c $(HDRS)
	@mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf mmu obj/

.PHONY: clean
