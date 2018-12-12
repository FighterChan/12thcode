EXE = mac.elf

OBJ = main.o
SRC = main.c

$(EXE) : $(OBJ)
	gcc -o $(EXE) $^

%.o:%.c
	gcc -c $< -o $@

.PHONY:clean
clean:
	rm -rf *.o *.elf
