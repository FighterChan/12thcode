EXE = mac.elf

OBJ = main.o mac.o tools.o
SRC = main.c mac.c tools.c

$(EXE) : $(OBJ)
	gcc -o  $(EXE) $^

%.o:%.c
	gcc -c $< -o $@

.PHONY:clean
clean:
	rm -rf *.o *.elf
