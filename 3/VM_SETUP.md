# Настройка и запуск Виртуальной Машинны

## Структура файлов

```
3/
├── tools/                      # Папка для инструментов
│   ├── Portable.RemoteTasks.Manager.exe  # Ассемблер/Дизассемблер
│   └── *.dll                         # Требуемые библиотеки
├── vm.target.pdsl              # Описание архитектуры VM
├── program.asm                 # Ассемблерный листинг (сгенерирован)
├── test-vm.bat                 # Скрипт сборки (Windows)
├── test-vm.sh                  # Скрипт сборки (Linux/macOS)
├── util/                       # Временные файлы (создаётся автоматически)
└── src/
    └── utils.c                  # get_instruction_name, get_register_name
```

## Требования

### Необходимые инструменты:

1. **Portable.RemoteTasks.Manager.exe** - должен находиться в папке `tools/`
   - Поставляется кафедрой ПМиИКТ
   - Используется для ассемблирования и дизассемблирования
   - Требует authorization
   - **Важно:** Требуемые .dll файлы должны быть в той же папке `tools/`

2. Для Linux/macOS также требуется **Mono**:
   ```bash
   # Установка на macOS
   brew install mono

   # Установка на Linux (Ubuntu)
   sudo apt-get install mono-complete
   ```

## Установка инструментов

### 1. Создайте папку tools (уже создана):
```bash
mkdir -p tools
```

### 2. Скопируйте Portable.RemoteTasks.Manager.exe:
- Получите файлы у преподавателя/кафедры
- Скопируйте **ВСЕ следующие файлы** в папку `./3/tools/`:
  - `Portable.RemoteTasks.Manager.exe`
  - 所有 соответствующие `.dll` файлы (все библиотеки, которые идут с .exe)

### 3. Проверьте структуру:
```bash
cd 3
ls tools/
# Должно показать:
# Portable.RemoteTasks.Manager.exe
# *.dll (несколько .dll файлов)
```

## Настройка авторизации

Скрипты содержат следующие параметры авторизации:
```
Login: 509092
Password: 3e27905a-827f-43c8-8274-70ad7e088ab5
```

Если эти параметры изменились, обновите их в:
- `test-vm.bat` (строки 17-18)
- `test-vm.sh` (строки 17-18)

## Параметры архитектуры

В соответствии с твоим вариантом (М3, 1: common RAM):

```
archName vm \
binaryFileToRun out.ptptb \
codeRamBankName RAM \
ipRegStorageName ip \
finishMnemonicName HALT
```

### Объяснение параметров:

| Параметр | Значение | Описание |
|----------|----------|----------|
| archName | vm | Название архитектуры из `vm.target.pdsl` |
| binaryFileToRun | out.ptptb | Бинарный файл, получается после ассемблирования |
| codeRamBankName | RAM | Название банка памяти (range RAM[0x0000..0xffff]) |
| ipRegStorageName | ip | Название storage регистра IP (storage ip[32]) |
| finishMnemonicName | HALT | Мнемоника инструкции остановки |

## Инструкции (mnemonics)

### Арифметические:
- MOV, ADD, SUB, MUL, DIV

### Сравнение:
- CMP

### Переходы:
- JMP, JE, JNE, JL, JG

### Стек:
- PUSH, POP

### Подпрограммы:
- CALL, RET

### Память:
- LOAD, STORE

### Завершение:
- HALT

## Регистры

### Общие:
- R0, R1, R2, R3, R4, R5, R6, R7

### Специальные:
- IP - Instruction Pointer
- SP - Stack Pointer
- BP - Base Pointer
- FLAGS - Флаги (Z, S, C, O)

## Формат ассемблерного файла

```assembly
.data
; Секция данных
str_0: db "Hello"
var_1: dd 42

.code
main:
    ; Секция кода
    PUSH BP
    MOV BP, SP
    
    ; Твой код здесь
    
    HALT
```

## Как собрать

### Проверка наличия Portable.RemoteTasks.Manager.exe:
```bash
ls tools/Portable.RemoteTasks.Manager.exe
ls tools/*.dll    # Проверить наличие dll файлов
```

Если файлы не найдены, скопируйте их в папку `./3/tools/` из материалов лабораторной работы.

### Windows:
```cmd
# Находясь в папке ./3/
test-vm.bat
```
Скрипт автоматически найдет Portable.RemoteTasks.Manager.exe в папке tools/ и использует его с авторизацией.

### Linux/macOS:
```bash
# Находясь в папке ./3/
./test-vm.sh
```
Требуется установленный **Mono**. Скрипт автоматически найдет Portable.RemoteTasks.Manager.exe в папке tools/ и использует его через mono с авторизацией:
```bash
mono tools/Portable.RemoteTasks.Manager.exe -ul 509092 -up 3e27905a-827f-43c8-8274-70ad7e088ab5 ...
```

## Результаты

После выполнения скрипта в папке `./3/`:

1. **out.ptptb** - бинарный файл (готов к выполнению на VM)
2. **disasmListing.txt** - дизассемблированный код (для проверки)
3. **util/tmp.txt** - временный файл с TaskID

## Сценарии работы

### 1. Ассемблирование
Преобразует `.asm` в бинарный `.ptptb` формат по определению архитектуры `vm.target.pdsl`.

### 2. Дизассемблирование
Обратно преобразует `.ptptb` в `.asm` для верификации.

### 3. Запуск на эмуляторе
Использует полученный `out.ptptb` с указанными параметрами архитектуры.

## Проверка

Функции `get_instruction_name` и `get_register_name` в `src/utils.c` **НЕ НУЖНО МЕНЯТЬ** - они полностью соответствуют мнемоникам в `vm.target.pdsl`.

## Поиск проблем

### Если mono не установлен:
```bash
# Проверка
mono --version

# Установка (macOS)
brew install mono

# Установка (Ubuntu)
sudo apt-get install mono-complete
```

### Если Portable.RemoteTasks.Manager.exe отсутствует:
- Получите его у преподавателя
- Скопируйте **Portable.RemoteTasks.Manager.exe** и все **.dll файлы** в папку `./3/tools/`
- **Все файлы должны быть в одной папке tools/**
- Убедитесь, что файлы и папка имеют правильные права доступа

### Ошибка авторизации:
- Проверьте актуальность логина/пароля
- Обновите параметры в `test-vm.bat` и `test-vm.sh`

### Ошибка запуска Portable.RemoteTasks.Manager.exe:
- Убедитесь, что все требуемые .dll файлы находятся в `tools/`
- Иногда требуется запуск Portable.RemoteTasks.Manager.exe один раз для инициализации (на Windows)
- Проверьте, что architecture name совпадает: `vm` в скрипте и `vm.target.pdsl`