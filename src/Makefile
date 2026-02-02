SRCS=$(wildcard *.cpp)
BINS=$(SRCS:.cpp=.exe)
CC=C:\ProgramData\mingw64\mingw64\bin\g++
#esm:  
#	esm.exe ../asm/$(filter-out $@,$(MAKECMDGOALS))
%.asm:
	esm.exe ../asm/$@

emu:
	emulator.exe

all: $(BINS)

%.exe: %.cpp
	$(CC) .\$< -o .\$@

clean:
	rm $(BINS)
