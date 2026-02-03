SRCS=$(wildcard *.cpp)
BINS=$(SRCS:.cpp=.exe)
CC=C:\ProgramData\mingw64\mingw64\bin\g++
#esm:  
#	esm.exe ../asm/$(filter-out $@,$(MAKECMDGOALS))
all: $(BINS)
	.\assembly.exe
%.exe: %.cpp
	$(CC) .\$< -o .\$@


%.asm:
	esm.exe ../asm/$@
emu:
	emulator.exe


clean:
	rm $(BINS)
