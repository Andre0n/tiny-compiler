cc = gcc -std=c11 -Isrc/include
CFLAGS = -Wall -Wextra -pedantic -lm -ldl -fPIC -rdynamic
CFLAGS_DEBUG = -O0 -fno-builtin -ggdb
CFLAGS_REALEASE = -O3

output_dir = build/
object_dir = build/objects/
sources = $(wildcard src/*.c)
objects = $(patsubst src/%.c, $(object_dir)/%.o, $(sources))

target = tiny

.PHONY: clean

$(object_dir)/%.o: src/%.c | $(object_dir)
	@echo [Compiling] $@ $(CFLAGS)
	@$(cc) -c $(CFLAGS) $(CFLAGS_DEBUG) -o $@ $<

$(target): $(objects)
	@echo [LD] $@
	@$(cc) -o $(output_dir)/$@ $^
	
$(object_dir):
	@mkdir -p $@

clean:
	@rm -rf $(object_dir)