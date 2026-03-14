@echo off
setlocal EnableDelayedExpansion

REM Пути к инструментам
set TOOLS_DIR=tools
set MANAGER=%TOOLS_DIR%\Portable.RemoteTasks.Manager.exe

REM Проверка наличия Portable.RemoteTasks.Manager.exe
if not exist "%MANAGER%" (
    echo ERROR: Portable.RemoteTasks.Manager.exe not found in tools\!
    echo Please copy Portable.RemoteTasks.Manager.exe and its .dll files to .\3\tools\
    echo.
    echo Example structure:
    echo   .\3\
    echo     ├── tools\
    echo     │   ├── Portable.RemoteTasks.Manager.exe
    echo     │   └── *.dll ^(required libraries^)
    echo     ├── vm.target.pdsl
    echo     └── ...
    pause
    exit /b 1
)

REM Параметры для твоей VM
set archName=vm
set defFile=vm.target.pdsl
set asmListing=program.asm

REM Параметры авторизации Portable.RemoteTasks.Manager
set login=509092
set password=3e27905a-827f-43c8-8274-70ad7e088ab5

echo ====================================
echo VM Architecture Build Script
echo ====================================
echo Architecture: %archName%
echo Definition file: %defFile%
echo Assembly listing: %asmListing%
echo Manager: %MANAGER%
echo Login: %login%
echo ====================================
echo.

REM Создаём директорию util если нет
if not exist util mkdir util

REM Ассемблирование
echo [1/4] Assembling %asmListing%...
%MANAGER% -ul %login% -up %password% -w -id -s Assemble asmListing %asmListing% definitionFile %defFile% archName %archName% > util\tmp.txt
set /p id=<util\tmp.txt

if "!id!"=="" (
    echo ERROR: Assembly failed. Check util\tmp.txt for details
    type util\tmp.txt
    pause
    exit /b 1
)

echo TaskID: !id!
echo.

REM Дождаться завершения задачи
echo [2/4] Waiting for task to complete...
%MANAGER% -ul %login% -up %password% -g !id! > util\status.txt
type util\status.txt
echo.

REM Получение бинарного файла
echo [3/4] Getting binary file...
%MANAGER% -ul %login% -up %password% -g !id! -r out.ptptb > util\asmfile
echo Binary file saved as: out.ptptb
echo.

REM Дизассемблирование (для проверки)
echo [4/4] Disassembling for verification...
%MANAGER% -ul %login% -up %password% -w -id -s Disassemble in.ptptb out.ptptb out.ptptb definitionFile %defFile% archName %archName% > util\tmp.txt
set /p id2=<util\tmp.txt
%MANAGER% -ul %login% -up %password% -g !id2!
%MANAGER% -ul %login% -up %password% -g !id2! -r disasmListing.txt > util\dissfile
echo Disassembly saved as: disasmListing.txt
echo.

REM Вывод параметров для выполнения
echo ====================================
echo Parameters for execution:
echo ====================================
echo archName %archName% \
echo binaryFileToRun out.ptptb \
echo codeRamBankName RAM \
echo ipRegStorageName ip \
echo finishMnemonicName HALT
echo ====================================

pause