# ============================================================================
# Makefile - Projet ISIMA S4
# ============================================================================

CC       := gcc
SRC_DIR  := src
BIN      := simulation.out
TEX_DIR  := report

# Options de compilation.
CFLAGS   := -Wall -Wextra -Wpedantic -O2
CPPFLAGS :=
LDFLAGS  := -lm -lSDL2_ttf $(shell sdl2-config --cflags --libs)

# Fichiers temporaires générés par GCC et LaTeX.
CLEANABLE := *.swp *.gch *.gz *.toc *.aux *.log $(BIN)

.PHONY: all build run pdf zip clean

# Cible par défaut.
all: build

# Compile le programme.
build:
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $(BIN) main.c $(SRC_DIR)/*.c $(LDFLAGS)

# Compile puis lance le programme.
run: build
	./$(BIN)

# Compile le rapport LaTeX deux fois afin de résoudre les références.
pdf:
	cd $(TEX_DIR) && pdflatex report.tex && pdflatex report.tex

# Crée une archive du projet après nettoyage.
zip: clean
	tar -zcvf simulation.tar.gz *

# Supprime les fichiers générés.
clean:
	rm -f $(CLEANABLE)
	cd $(TEX_DIR) && rm -f $(CLEANABLE)
