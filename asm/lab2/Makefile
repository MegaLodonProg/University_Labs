TARGET = lab2
ASM = nasm 
LD = ld
ASMFLAGS = -f elf64 -g

# Направление сортировки (по умолчанию ASCENDING - возрастание)
SORT_DIR ?= ASC

# Добавляем флаг в зависимости от SORT_DIR
# ?= означает: "присвоить значение, только если переменная еще не определена".
# -DИМЯ в NASM означает "определить макрос ИМЯ".
ifeq ($(SORT_DIR), ASC)
    DEFINE_FLAG = -DASC
else
    DEFINE_FLAG = -DDES
endif

# Цель lab2 зависит от файла lab2.o
$(TARGET): $(TARGET).o
	$(LD) $< -o $@

$(TARGET).o: $(TARGET).asm
	$(ASM) $(ASMFLAGS) $(DEFINE_FLAG) $< -o $@

clean:
	rm -f $(TARGET).o $(TARGET)

run: $(TARGET)
	./$(TARGET)
	@echo "Код возврата: $$?"

# Объявляет цели clean и run как phony (ненастоящие). make не будет искать файлы с такими именами и всегда будет выполнять команды
.PHONY: clean run
