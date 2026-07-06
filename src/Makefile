SRCS=$(wildcard *.cpp)
BINS=$(SRCS:.cpp=.exe)
CC=C:\MinGW64\bin\g++

ARGS := $(addprefix ../asm/,$(MAKECMDGOALS))
#esm:  
#	esm.exe ../asm/$(filter-out $@,$(MAKECMDGOALS))
all: $(BINS)
	.\assembly.exe
%.exe: %.cpp
	$(CC) .\$< -o .\$@ -std=c++14

.PHONY: esm emu clean

esm:
	esm.exe $(addprefix ../asm/,$(filter-out $@,$(MAKECMDGOALS)))

%.esm:
	@:

%.c:
	compiler.exe ../highlanguage/$@
emu:
	emulator.exe


clean:
	rm $(BINS)
