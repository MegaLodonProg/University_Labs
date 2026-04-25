TARGET = lab3
ASM = nasm 
LD = ld
ASMFLAGS = -f elf64 -g

# Цель lab2 зависит от файла lab2.o
$(TARGET): $(TARGET).o
	$(LD) $< -o $@

$(TARGET).o: $(TARGET).asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(TARGET).o $(TARGET)

run: $(TARGET)
	./$(TARGET)
	@echo ""
	@echo "Код возврата: $$?"

# Объявляет цели clean и run как phony (ненастоящие). make не будет искать файлы с такими именами и всегда будет выполнять команды
.PHONY: clean run
