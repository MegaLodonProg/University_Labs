TARGET = lab4
ASM = nasm 
GCC = gcc
ASMFLAGS = -f elf64 -g
LDFLAGS = -no-pie
LDLIBS = -lm
FILE_NAME = output.txt

# Цель lab4 зависит от файла lab4.o
$(TARGET): $(TARGET).o
	$(GCC) $(LDFLAGS) $< -o $@ $(LDLIBS)

$(TARGET).o: $(TARGET).asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(TARGET).o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(FILE_NAME)
	@echo ""
	@echo "Код возврата: $$?"

# Объявляет цели clean и run как phony (ненастоящие). make не будет искать файлы с такими именами и всегда будет выполнять команды
.PHONY: clean run
