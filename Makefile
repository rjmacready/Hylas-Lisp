CXX = g++ # >2012 >gcc
CXXFLAGS = -O0 -ggdb -std=c++0x # -Wall -Wextra -Werror
LLVMFLAGS = `llvm-config --cppflags --ldflags --libs core jit native asmparser asmprinter linker` -ldl
SOURCES = UI/console/console.cpp
default:console

console:
	$(CXX) $(SOURCES) -o hylas.o $(CXXFLAGS) $(LLVMFLAGS)

gui:
	make -C UI/syntagma

clean:
	rm hylas.o