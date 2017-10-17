FRAMEWORKS := -framework IOKit -framework Foundation
CFLAGS := -Wall -Werror -g

keylogger: main.c keymap.h
	clang -o $@ $< $(CFLAGS) $(FRAMEWORKS)

keymap.h: keymap.py
	python3 $< > $@