CXX = g++ # >2012 >gcc
CXXFLAGS = -O0 -ggdb -std=c++0x # -Wall -Wextra -Werror
LLVMFLAGS = `llvm-config --cppflags --ldflags --libs core jit native asmparser asmprinter linker` -ldl
SOURCES = UI/console/console.cpp
DEFINES= -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
default:console

console:
	$(CXX) $(SOURCES) -o hylas.o $(CXXFLAGS) $(LLVMFLAGS) $(DEFINES)

gui:
	make -C UI/syntagma

clean:
	rm hylas.o