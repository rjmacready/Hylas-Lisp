CC = g++
CXXFLAGS = -std=c++0x
LDFLAGS = `llvm-config --libs engine --cxxflags --ldflags`
SRCDIR = src
SOURCES = $(SRCDIR)/*.cpp
default:hylas

hylas:
	$(CC) $(SOURCES) -o hylas $(CXXFLAGS)

clean:
	rm hylas