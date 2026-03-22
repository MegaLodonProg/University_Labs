%: %.asm
	nasm -f elf64 -g $< -o $*.o
	ld -o $@ $*.o

run-%: %.asm
	$(MAKE) $<
	gdb ./$*

clean:
	rm -f *.o lab*
