#!/bin/bash
# run.sh — Функциональные тесты грамматического анализатора
# Запуск: ./run.sh
#
# Для каждого .conllu из Input/ запускает checker, сравнивает вывод
# с эталоном из Expected/ и выводит статистику.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXE="${SCRIPT_DIR}/../../build/Desktop-Debug/app/app"
IN_DIR="${SCRIPT_DIR}/Input"
EXP_DIR="${SCRIPT_DIR}/Expected"
OUT_DIR="${SCRIPT_DIR}/Output"
LISTS_DIR="${SCRIPT_DIR}/../../lists"

echo "========================================"
echo "  ЗАПУСК ФУНКЦИОНАЛЬНЫХ ТЕСТОВ"
echo "========================================"
echo

if [ ! -f "$EXE" ]; then
    echo "[ОШИБКА] Не найден исполняемый файл: $EXE"
    exit 1
fi

mkdir -p "$OUT_DIR"

TOTAL=0
PASSED=0
FAILED=0

for infile in "$IN_DIR"/*.conllu; do
    name=$(basename "$infile" .conllu)
    expfile="$EXP_DIR/$name.txt"
    outfile="$OUT_DIR/$name.txt"
    console="$OUT_DIR/${name}_console.txt"

    TOTAL=$((TOTAL + 1))

    rc=0
    "$EXE" "$infile" "$outfile" --lists "$LISTS_DIR" > "$console" 2>&1 || rc=$?

    # Сравнение: если outfile не создан (ошибка до записи), считаем пустым
    [ -f "$outfile" ] || true > "$outfile"

    if [ -f "$expfile" ]; then
        if [ $rc -ne 0 ]; then
            # Негативный тест: программа завершилась с ошибкой.
            # Сравниваем stderr (console) с эталоном, outfile игнорируем.
            if diff -q "$expfile" "$console" > /dev/null 2>&1; then
                echo "  [PASS] $name"
                PASSED=$((PASSED + 1))
            else
                echo "  [FAIL] $name — сообщение об ошибке не совпадает"
                echo "         Ожидалось: $(cat "$expfile")"
                echo "         Получено:  $(cat "$console")"
                FAILED=$((FAILED + 1))
            fi
        else
            if diff -q "$expfile" "$outfile" > /dev/null 2>&1; then
                echo "  [PASS] $name"
                PASSED=$((PASSED + 1))
            else
                echo "  [FAIL] $name — вывод не совпадает с эталоном"
                echo "         Ожидалось: $(cat "$expfile")"
                echo "         Получено:  $(cat "$outfile")"
                FAILED=$((FAILED + 1))
            fi
        fi
    else
        if [ $rc -eq 0 ]; then
            echo "  [PASS] $name (новый, без эталона)"
            cp "$outfile" "$expfile"
            PASSED=$((PASSED + 1))
        else
            echo "  [FAIL] $name — exit code $rc (нет эталона)"
            FAILED=$((FAILED + 1))
        fi
    fi
done

echo
echo "========================================"
echo "  ИТОГИ"
echo "  Всего: $TOTAL"
echo "  PASS:  $PASSED"
echo "  FAIL:  $FAILED"
echo "========================================"

if [ $FAILED -gt 0 ]; then
    exit 1
fi
