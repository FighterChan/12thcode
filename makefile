EXE = mac.elf

OBJ = main.o mac.o
SRC = main.c mac.c

$(EXE) : $(OBJ)
	gcc -o  $(EXE) $^

%.o:%.c
	gcc -c $< -o $@

.PHONY:clean
clean:
	rm -rf *.o *.elf
