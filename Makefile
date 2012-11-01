CXX = g++
CXXFLAGS = -std=c++0x -Wall -Wextra -Werror -ggdb -fPIC -rdynamic -g
LLVMFLAGS = `llvm-config --cppflags --ldflags --libs core jit native asmparser asmprinter linker`
SOURCES = src/console.cpp
default:hylas

hylas:
	$(CXX) $(SOURCES) -o hylas.o $(CXXFLAGS) $(LLVMFLAGS)





clean:
	rm hylas.o
