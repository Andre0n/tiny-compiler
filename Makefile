cc = gcc -std=c11
CFLAGS = -Wall -Wextra -pedantic -lm -ldl -fPIC -rdynamic -Werror
CFLAGS_DEBUG = -O0 -fno-builtin -ggdb -g3 -gdwarf-2
CFLAGS_REALEASE = -O3 -s

output_dir = build/
object_dir = build/objects/
sources = $(wildcard src/*.c)
objects = $(patsubst src/%.c, $(object_dir)/%.o, $(sources))

target = tiny

.PHONY: clean

release: CFLAGS += $(CFLAGS_REALEASE)
release: $(target)

debug: CFLAGS += $(CFLAGS_DEBUG) 
debug: $(target)

$(object_dir)/%.o: src/%.c | $(object_dir)
	@echo [Compiling] $@ $(CFLAGS)
	@$(cc) -c $(CFLAGS) -o $@ $<

$(target): $(objects)
	@echo [LD] $@
	@$(cc) -o $(output_dir)/$@ $^

$(object_dir):
	@mkdir -p $@

clean:
	@echo cleaning $(object_dir)
	@rm -rf $(object_dir)