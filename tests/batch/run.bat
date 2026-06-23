@echo off
chcp 65001 >nul
REM run.bat — Функциональные тесты грамматического анализатора (Windows)
REM Запуск: run.bat
REM
REM Для каждого .conllu из Input/ запускает checker, сравнивает вывод
REM с эталоном из Expected/ и выводит статистику.

setlocal enabledelayedexpansion

set EXE=..\..\Executable\checker.exe
set LISTS_DIR=..\..\lists
if not exist "%EXE%" (
    echo [ОШИБКА] Не найден исполняемый файл: %EXE%
    exit /b 1
)

if not exist "Input\*" (
    echo [ОШИБКА] Не найдены входные файлы в Input\
    exit /b 1
)

if not exist "Expected" mkdir Expected
if not exist "Output" mkdir Output

set TOTAL=0
set PASSED=0
set FAILED=0

for %%f in (Input\*.conllu) do (
    set /a TOTAL+=1
    set "name=%%~nf"
    set "expfile=Expected\%%~nf.txt"
    set "outfile=Output\%%~nf.txt"

    "%EXE%" "%%f" "!outfile!" --lists "%LISTS_DIR%" > nul 2>&1

    if exist "!expfile!" (
        fc "!expfile!" "!outfile!" >nul 2>&1
        if !errorlevel! equ 0 (
            echo   [PASS] %%~nf
            set /a PASSED+=1
        ) else (
            echo   [FAIL] %%~nf
            echo         Ожидалось:
            type "!expfile!"
            echo.
            echo         Получено:
            type "!outfile!"
            echo.
            set /a FAILED+=1
        )
    ) else (
        echo   [PASS] %%~nf ^(новый, без эталона^)
        copy "!outfile!" "!expfile!" >nul
        set /a PASSED+=1
    )
)

echo.
echo ========================================
echo   ИТОГИ
echo   Всего: !TOTAL!
echo   PASS:  !PASSED!
echo   FAIL:  !FAILED!
echo ========================================

if !FAILED! gtr 0 exit /b 1
