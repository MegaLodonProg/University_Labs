nasm -f elf64 -g  lab4.asm -o lab4.o
gcc -no-pie lab4.o -o lab4 -lm
./lab4 output.txt

