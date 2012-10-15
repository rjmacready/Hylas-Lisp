CC = g++
CXXFLAGS = -std=c++0x -Wall -Wextra -Werror
LDFLAGS = `llvm-config --libs engine --cxxflags --ldflags`
SRCDIR = src
SOURCES = $(SRCDIR)/*.cpp
default:hylas

hylas:
	$(CC) $(SOURCES) -o hylas $(CXXFLAGS)

clean:
	rm hylas
