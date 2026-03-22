#!/bin/bash

echo "=== Генерация документации Doxygen ==="

# Очистка и создание папки
rm -rf docs/
mkdir -p docs

echo "Генерация документации..."
doxygen Doxyfile

# Проверка результата
if [ -f "docs/html/index.html" ]; then
    echo "=== Документация успешно сгенерирована ==="
    echo "Открыть: docs/html/index.html"
else
    echo "=== ОШИБКА: Документация не сгенерирована ==="
    echo "Проверьте структуру проекта:"
    echo "ls -la include/ src/ tests/"
    ls -la include/ src/ tests/ 2>/dev/null || echo "Папки не найдены"
fi