#!/bin/bash
echo "=== Coverage Report ==="

BUILD_DIR="cmake-build-debug-coverage"
cd "$BUILD_DIR"

REPORT_DIR="coverage_report"
mkdir -p "$REPORT_DIR"

echo "Запуск тестов..."
./bin/test_hash_table
./bin/test_workzone
./bin/test_worker
./bin/test_workman
./bin/test_master
./bin/test_brigade

echo "Генерация отчета..."
gcovr --root .. \
      --filter ../model/src/ \
      --filter ../model/include/ \
      --exclude ../tests/ \
      --html \
      --html-details \
      -o "$REPORT_DIR"/coverage.html \
      --print-summary \

echo "Отчеты созданы в: $BUILD_DIR/$REPORT_DIR/"