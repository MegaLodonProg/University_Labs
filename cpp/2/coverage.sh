#!/bin/bash
echo "=== Coverage Report ==="

BUILD_DIR="cmake-build-debug-coverage"
cd "$BUILD_DIR"

REPORT_DIR="coverage_report"
mkdir -p "$REPORT_DIR"

echo "Запуск тестов..."
./bin/test_student_work
./bin/test_workstack
./bin/test_myvector

echo "Генерация отчета..."
gcovr --root .. \
      --filter ../src/lib/ \
      --filter ../include/MyVector \
      --filter ../include/StudentWork \
      --filter ../include/WorkStack \
      --exclude ../tests/ \
      --html \
      --html-details \
      -o "$REPORT_DIR"/coverage.html \
      --print-summary

echo "Отчеты созданы в: $BUILD_DIR/$REPORT_DIR/"