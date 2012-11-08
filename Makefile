CXX = clang++
CXXFLAGS = -std=c++0x -Wall -Wextra -Werror -O0
LLVMFLAGS = `llvm-config --cppflags --ldflags --libs core jit native asmparser asmprinter linker` -ldl
SOURCES = UI/console.cpp
default:hylas

hylas:
	$(CXX) $(SOURCES) -o hylas.o $(CXXFLAGS) $(LLVMFLAGS)





clean:
	rm hylas.o
