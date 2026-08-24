# How To Develop

Этот документ объясняет, как устроен C++ проект, куда класть файлы, как писать `CMakeLists.txt`, как собирать проект и как запускать тесты.

Текст написан для человека, который впервые видит CMake.

## 1. Что вообще происходит при сборке C++ проекта

Если всю жизнь компилировать один файл, картина выглядит простой:

```bash
g++ main.cpp -o app
```

Есть один `main.cpp`, компилятор его прочитал, и получилась программа `app`.

В большом проекте файлов много:

```text
bin/game_service/main.cpp
library/common/include/library/common/version.h
library/common/src/version.cpp
```

Тут уже надо понять, как эти файлы связаны между собой.

### 1.1. Какие бывают файлы

В C++ обычно встречаются такие файлы:

```text
.h    заголовочный файл, или header
.cpp  файл реализации
.o    объектный файл, промежуточный результат компиляции
.a    статическая библиотека
```

В нашем примере:

```text
library/common/include/library/common/version.h
library/common/src/version.cpp
bin/game_service/main.cpp
```

`version.h` - публичный header библиотеки `common`.

`version.cpp` - реализация библиотеки `common`.

`main.cpp` - код бинаря `game_service`, который использует библиотеку `common`.

### 1.2. Что такое header

Header обычно отвечает на вопрос:

```text
Что существует и как этим пользоваться?
```

Например файл:

```text
library/common/include/library/common/version.h
```

содержит:

```cpp
#pragma once

#include <string>

namespace game::common {

std::string version();

} // namespace game::common
```

Этот header говорит компилятору:

```text
Есть namespace game::common.
В нем есть функция version().
Она не принимает аргументов и возвращает std::string.
```

Header не говорит, как именно эти функции работают. Он только описывает их форму.

Эту форму часто называют:

```text
объявление
интерфейс
публичный API
контракт
```

Смысл простой: другие файлы могут подключить header и понять, как вызывать функции.

### 1.3. Зачем header нужен, если есть .cpp

Важный момент: компилятор компилирует `.cpp` файлы отдельно.

Когда компилятор компилирует:

```text
bin/game_service/main.cpp
```

он не читает автоматически:

```text
library/common/src/version.cpp
```

Он не сканирует весь проект в поисках функции `version()`.

Он видит только:

```text
1. сам main.cpp
2. файлы, которые подключены через #include
```

Поэтому в `main.cpp` написано:

```cpp
#include <library/common/version.h>
```

Это значит:

```text
Перед компиляцией main.cpp открой header library/common/version.h и подставь его содержимое в этот файл.
```

После этого компилятор видит объявление:

```cpp
std::string version();
```

И может проверить строку:

```cpp
game::common::version()
```

Компилятор понимает:

```text
такая функция объявлена
она не принимает аргументов
она возвращает std::string
вызов выглядит корректно
```

Без header-а компилятор увидел бы вызов неизвестной функции и не понял бы, можно ли так писать.

### 1.4. Что такое .cpp

`.cpp` файл обычно отвечает на вопрос:

```text
Как это реализовано?
```

Файл:

```text
library/common/src/version.cpp
```

содержит:

```cpp
#include <library/common/version.h>

namespace game::common {

std::string version() {
    return "0.1.0";
}

} // namespace game::common
```

Тут уже есть настоящий код функций.

Почему `version.cpp` тоже подключает `version.h`?

Чтобы компилятор проверил, что реализация совпадает с объявлением.

Если в header написано:

```cpp
std::string version();
```

а в `.cpp` случайно написать:

```cpp
int version() {
    return 1;
}
```

компилятор увидит конфликт и сообщит об ошибке.

То есть header нужен не только пользователям библиотеки, но и самой реализации: он помогает держать объявление и реализацию синхронными.

### 1.5. Что происходит при #include

Строка:

```cpp
#include <library/common/version.h>
```

для новичка может выглядеть как “импорт библиотеки”. Но технически это не совсем импорт.

На этапе препроцессинга C++ компилятор фактически вставляет содержимое header-а в текущий `.cpp` файл.

Упрощенно, было:

```cpp
#include <library/common/version.h>

int main() {
    game::common::version();
}
```

После обработки include компилятор как будто видит:

```cpp
#include <string>

namespace game::common {

std::string version();

}

int main() {
    game::common::version();
}
```

Именно поэтому компилятору надо знать, где искать header-файлы.

Если он не знает, где лежит `library/common/version.h`, будет ошибка вида:

```text
fatal error: library/common/version.h: No such file or directory
```

### 1.6. Что такое объектный файл

Объектный файл - это промежуточный результат компиляции одного `.cpp` файла.

Когда компилятор компилирует:

```text
library/common/src/version.cpp
```

он получает примерно такой файл:

```text
version.cpp.o
```

Когда компилятор компилирует:

```text
bin/game_service/main.cpp
```

он получает примерно такой файл:

```text
main.cpp.o
```

Объектный файл уже содержит машинный код, но это еще не обязательно готовая программа.

Например `main.cpp.o` может содержать код `main()`, но внутри него остается ссылка:

```text
где-то должна быть функция game::common::version(...)
```

А `version.cpp.o` содержит реализацию этой функции.

Чтобы получить готовый бинарь, эти объектные файлы надо соединить.

Этим занимается линкер.

### 1.7. Что делает линкер

Линкер соединяет скомпилированные объектные файлы и библиотеки.

Упрощенно:

```text
main.cpp.o говорит: я вызываю game::common::version(...)
version.cpp.o говорит: я реализую game::common::version(...)
линкер соединяет вызов с реализацией
```

Если реализация не найдена, будет ошибка линковки.

Например, если `main.cpp` вызвал `version()`, но мы забыли слинковать бинарь с `common`, компиляция `main.cpp` может пройти, а линковка упадет.

Типичная идея ошибки будет такая:

```text
undefined reference to game::common::version(...)
```

Это значит:

```text
Компилятор видел объявление функции в header-е.
Но линкер не нашел её реализацию.
```

Это важное различие:

```text
ошибка include/header-а: компилятор не знает, что такая функция существует
ошибка линковки: компилятор знает, что функция существует, но линкер не нашел её код
```

### 1.8. Что такое библиотека

Библиотека - это способ собрать несколько `.cpp` файлов в переиспользуемый кусок.

В нашем примере `library/common` собирается в target:

```text
common
```

Сейчас внутри него один `.cpp`:

```text
library/common/src/version.cpp
```

Но позже библиотека может содержать много файлов:

```text
src/version.cpp
src/format.cpp
src/writer.cpp
```

CMake соберет их в одну библиотеку, и другие бинарники смогут подключать её одной зависимостью:

```cmake
target_link_libraries(game_service
    PRIVATE
        common
)
```

### 1.9. Пошагово: как собирается library/common

CMake-файл библиотеки:

```text
library/common/CMakeLists.txt
```

содержит:

```cmake
add_library(common
    src/version.cpp
)

target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Что происходит при сборке target-а `common`:

```bash
cmake --build build --target common
```

Шаги примерно такие:

```text
1. CMake видит target common.
2. CMake знает, что common собирается из src/version.cpp.
3. Система сборки запускает компилятор для version.cpp.
4. Компилятор открывает version.cpp.
5. В version.cpp есть #include <library/common/version.h>.
6. Компилятор ищет этот header в include-путях target-а common.
7. include-путь был задан через target_include_directories(... PUBLIC .../include).
8. Компилятор находит library/common/include/library/common/version.h.
9. Компилятор проверяет объявления из version.h.
10. Компилятор компилирует version.cpp в объектный файл version.cpp.o.
11. Система сборки упаковывает объектный файл в библиотеку common.
```

Итог:

```text
common собран
внутри есть реализация version()
наружу доступен public header library/common/version.h
```

### 1.10. Пошагово: как собирается bin/game_service

Бинарь лежит здесь:

```text
bin/game_service/main.cpp
```

Он содержит:

```cpp
#include <iostream>

#include <library/common/version.h>

int main() {
    std::cout << "game_service " << game::common::version() << '\n';
    return 0;
}
```

CMake-файл бинаря:

```text
bin/game_service/CMakeLists.txt
```

содержит:

```cmake
add_executable(game_service
    main.cpp
)

target_link_libraries(game_service
    PRIVATE
        common
)
```

Что происходит при сборке:

```bash
cmake --build build --target game_service
```

Шаги примерно такие:

```text
1. CMake видит target game_service.
2. CMake знает, что game_service собирается из main.cpp.
3. CMake видит, что game_service зависит от common.
4. Если common еще не собран, сначала собирается common.
5. Компилятор компилирует bin/game_service/main.cpp.
6. В main.cpp есть #include <library/common/version.h>.
7. game_service сам не задавал include-путь к library/common/include.
8. Но game_service связан с common через target_link_libraries(... common).
9. У common include-директория объявлена как PUBLIC.
10. Поэтому include-путь library/common/include автоматически доступен game_service.
11. Компилятор находит version.h и понимает, как вызывать version().
12. Компилятор делает объектный файл main.cpp.o.
13. Линкер соединяет main.cpp.o с библиотекой common.
14. В библиотеке common находится реализация version().
15. На выходе получается исполняемый файл game_service.
```

Итог:

```text
main.cpp знает про функцию из header-а
реализация функции лежит в common
линкер соединяет game_service с common
получается готовый бинарь game_service
```

### 1.11. Что будет, если что-то забыть

Если забыть `#include <library/common/version.h>`, компилятор не поймет вызов:

```cpp
game::common::version()
```

Проблема будет на этапе компиляции.

Если оставить include, но убрать зависимость:

```cmake
target_link_libraries(game_service
    PRIVATE
        common
)
```

то возможны две проблемы.

Первая: компилятор может не найти header:

```text
fatal error: library/common/version.h: No such file or directory
```

Вторая: если header каким-то образом найден, линкер может не найти реализацию:

```text
undefined reference to game::common::version(...)
```

Поэтому для использования библиотеки обычно нужны обе части:

```text
#include <...>              чтобы компилятор увидел объявления
target_link_libraries(...)  чтобы сборка получила include-пути и реализацию
```

### 1.12. Короткая схема

```text
version.h
  объявляет функции
  нужен компилятору при #include

version.cpp
  реализует функции
  компилируется в объектный файл

common
  библиотека из version.cpp
  публикует include/library/common/version.h

game_service/main.cpp
  подключает version.h
  вызывает version()

game_service
  бинарь
  линкуется с common
```

Руками вызывать компилятор и линкер на каждый файл неудобно. Поэтому мы используем CMake.

CMake не компилирует C++ сам. Он читает файлы `CMakeLists.txt` и генерирует инструкции для настоящей системы сборки. Потом уже эта система сборки запускает компилятор и линкер.

## 2. Команда cmake -S . -B build

Первый раз проект надо сконфигурировать:

```bash
cmake -S . -B build
```

Эта команда еще не собирает `.cpp` файлы. Она подготавливает папку сборки.

Разберем команду по частям.

```text
cmake
```

Это программа CMake.

```text
-S .
```

`-S` означает source directory, то есть папка с исходниками проекта.

`.` означает текущая папка.

Если ты находишься в корне проекта:

```text
/home/.../game-service
```

то `.` означает именно эту папку.

То есть:

```bash
cmake -S .
```

значит:

```text
Ищи исходники и корневой CMakeLists.txt в текущей папке.
```

```text
-B build
```

`-B` означает build directory, то есть папка, куда CMake положит файлы сборки.

`build` - имя этой папки.

То есть:

```bash
cmake -B build
```

значит:

```text
Создай или используй папку build для технических файлов сборки.
```

Итого:

```bash
cmake -S . -B build
```

значит:

```text
Возьми проект из текущей папки и подготовь сборку в папке build.
```

После этой команды появится папка:

```text
build/
```

В ней будут не исходники, а файлы, нужные системе сборки. Эту папку обычно не коммитят в git.

Почему исходники и build-директория разделены:

```text
исходники лежат в проекте
временные файлы сборки лежат в build
проект не засоряется .o, Makefile, cache-файлами и бинарями
```

Частые опции CMake:

```text
-S <path>              где лежат исходники
-B <path>              куда положить build-файлы
-G <generator>         какой генератор использовать
-DNAME=value           задать CMake-переменную
--build <build-dir>    собрать уже сконфигурированный проект
--target <name>        собрать конкретный target
```

Пример с переменной:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

`-D` задает настройку CMake. Здесь мы говорим: собрать Debug-конфигурацию.

В нашем проекте на старте достаточно:

```bash
cmake -S . -B build
```

## 3. Что такое Ninja и -G

Иногда команду пишут так:

```bash
cmake -S . -B build -G Ninja
```

`-G` означает generator.

Generator - это формат файлов сборки, которые CMake создаст внутри `build`.

CMake сам не компилирует код. Он генерирует файлы для другой системы сборки:

```text
Unix Makefiles
Ninja
Visual Studio project files
Xcode project files
```

На Linux без `-G` CMake часто выбирает `Unix Makefiles`.

С `-G Ninja` мы явно говорим:

```text
Сгенерируй сборку для Ninja.
```

Ninja - это быстрая система сборки. Она читает файлы, которые создал CMake, и запускает компилятор.

Упрощенно:

```text
CMake читает CMakeLists.txt
CMake создает build-файлы
Ninja или Make запускает компилятор и линкер
```

Если Ninja не установлен, не используй `-G Ninja`:

```bash
cmake -S . -B build
```

Если Ninja установлен, можно использовать:

```bash
cmake -S . -B build -G Ninja
```

## 4. Как собрать проект

После конфигурации собираем всё:

```bash
cmake --build build
```

Разберем:

```text
cmake
```

Запускаем CMake.

```text
--build build
```

Говорим CMake:

```text
Собери проект, который был подготовлен в папке build.
```

Почему тут снова пишется `build`?

Потому что на прошлом шаге мы сказали:

```bash
cmake -S . -B build
```

То есть CMake подготовил сборку именно в папке `build`. Теперь при сборке надо указать эту же папку:

```bash
cmake --build build
```

Если бы мы подготовили сборку в другую папку:

```bash
cmake -S . -B my-debug-build
```

то собирали бы так:

```bash
cmake --build my-debug-build
```

## 5. Что такое target

`target` - это именованная штука, которую умеет собирать CMake.

В нашем проекте target может быть:

```text
библиотека       common
бинарь           game_service
тестовый бинарь  common_ut
```

Библиотека - это переиспользуемый код. Например `common` или будущая `json`.

Бинарь - это исполняемый файл, который можно запустить. Например `game_service`.

Тестовый бинарь - это обычный исполняемый файл, но он нужен только для проверки кода. Например `common_ut`.

Собрать все targets проекта:

```bash
cmake --build build
```

Собрать один конкретный target:

```bash
cmake --build build --target common
```

Разберем:

```text
cmake --build build
```

Собрать проект из папки `build`.

```text
--target common
```

Но собрать не всё, а только target `common`.

Важно: CMake обычно собирает не папки, а targets.

Папка `library/common` - это место, где лежит код. А `common` - это target, который из этого кода собирается.

## 6. Что такое заголовки и почему CMake должен их искать

Когда в `.cpp` файле написано:

```cpp
#include <library/common/version.h>
```

компилятор должен найти файл `version.h`.

Но сам по себе он не знает, где искать header нашего проекта. Поэтому в `CMakeLists.txt` мы пишем:

```cmake
target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Это значит:

```text
Для target-а common добавь папку library/common/include в список мест, где компилятор ищет headers.
```

`${CMAKE_CURRENT_SOURCE_DIR}` - это текущая папка, где лежит данный `CMakeLists.txt`.

Если мы находимся в:

```text
library/common/CMakeLists.txt
```

то:

```text
${CMAKE_CURRENT_SOURCE_DIR}
```

примерно означает:

```text
/home/.../game-service/library/common
```

Поэтому:

```cmake
${CMAKE_CURRENT_SOURCE_DIR}/include
```

означает:

```text
/home/.../game-service/library/common/include
```

После этого такой include:

```cpp
#include <library/common/version.h>
```

ведет к файлу:

```text
library/common/include/library/common/version.h
```

То есть часть пути до `include` добавляет CMake, а часть после `include` пишется в `#include`.

## 7. Почему путь внутри include такой длинный

Можно было бы положить файл так:

```text
library/common/include/version.h
```

и писать:

```cpp
#include <version.h>
```

Но в большом проекте быстро появляются одинаковые имена:

```text
types.h
config.h
parser.h
status.h
```

Если все будут писать короткие include-ы, непонятно, чей именно `parser.h` подключается.

Поэтому внутри `include` мы кладем файлы с пространством имен проекта и библиотеки:

```text
library/common/include/library/common/version.h
library/json/include/library/json/json_value.h
library/http/include/library/http/headers.h
```

И в коде пишем явно:

```cpp
#include <library/common/version.h>
#include <library/json/json_value.h>
#include <library/http/headers.h>
```

Это длиннее, зато понятно и меньше конфликтов.

## 8. Когда нужно писать target_include_directories

Пишем `target_include_directories`, когда у target-а есть headers, которые надо подключать через `#include`.

Для библиотеки почти всегда будет так:

```cmake
target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Потому что у библиотеки есть публичные headers в `include/`.

Для простого бинаря с одним `main.cpp` обычно не нужно:

```text
bin/game_service/main.cpp
```

Если `main.cpp` подключает headers из библиотеки, например:

```cpp
#include <library/common/version.h>
```

то сам `game_service` не обязан вручную знать, где лежит `library/common/version.h`. Ему достаточно слинковаться с библиотекой:

```cmake
target_link_libraries(game_service
    PRIVATE
        common
)
```

А `common` уже сам объявил свои include-пути через `target_include_directories(common PUBLIC ...)`.

## 9. PRIVATE, PUBLIC, INTERFACE простыми словами

Эти слова отвечают на вопрос: кому нужна настройка или зависимость?

```text
PRIVATE   нужна только этому target-у
PUBLIC    нужна этому target-у и тем, кто от него зависит
INTERFACE нужна только тем, кто от него зависит
```

Пример `PRIVATE`:

```cmake
target_link_libraries(game_service
    PRIVATE
        common
)
```

`game_service` сам использует `common`, но никто другой не будет подключать `game_service` как библиотеку. Поэтому зависимость приватная.

Пример `PUBLIC`:

```cmake
target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Headers нужны самой библиотеке `common`, и они также нужны всем, кто будет использовать `common`.

Пример `INTERFACE`:

```cmake
add_library(game_game_core INTERFACE)

target_include_directories(game_game_core
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

`INTERFACE` используют для target-а, у которого нет своих `.cpp` файлов, но есть публичные headers или настройки, которые нужны другим target-ам.

Например `game_core` может сначала быть набором интерфейсов в headers:

```text
IGameModule
ITurnBasedGame
TGameResult
```

Если у библиотеки нет `.cpp`, её можно сделать `INTERFACE`:

```cmake
add_library(game_game_core INTERFACE)
```

Такой target сам почти нечего не компилирует, но другие библиотеки могут от него зависеть и получать его include-пути.

На старте можно запомнить три правила:

```text
У обычной библиотеки include-директория обычно PUBLIC.
У бинаря зависимости обычно PRIVATE.
Header-only библиотека часто INTERFACE.
```

## 10. Структура библиотеки

Основной пример - `library/common`:

```text
library/common/
  CMakeLists.txt
  include/
    library/
      common/
        version.h
  src/
    version.cpp
  tests/
    ut/
      CMakeLists.txt
      version_ut.cpp
```

Что где лежит:

```text
include/   публичные .h файлы
src/       .cpp файлы с реализацией
tests/ut/  unit-тесты этой библиотеки
```

## 11. Почему не класть .h и .cpp рядом

Можно класть рядом:

```text
version.h
version.cpp
```

Это не ошибка.

Но для библиотеки удобнее разделять публичный API и реализацию:

```text
include/  то, чем можно пользоваться снаружи
src/      внутренности библиотеки
```

Так проще понять, какие файлы являются публичным контрактом библиотеки, а какие являются деталями реализации.

## 12. CMakeLists.txt для библиотеки

Файл:

```text
library/common/CMakeLists.txt
```

Содержимое:

```cmake
add_library(common
    src/version.cpp
)

target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Разберем построчно.

```cmake
add_library(common
    src/version.cpp
)
```

Создает target-библиотеку с именем `common`.

`src/version.cpp` - файл, который надо скомпилировать внутрь этой библиотеки.

```cmake
target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Говорит, где искать headers этой библиотеки.

`PUBLIC` значит, что эти headers нужны и самой библиотеке, и тем, кто будет её использовать.

```cmake
add_subdirectory(tests/ut)
```

Подключает папку с unit-тестами этой библиотеки.

## 13. CMakeLists.txt для бинаря

Файл:

```text
bin/game_service/CMakeLists.txt
```

Содержимое:

```cmake
add_executable(game_service
    main.cpp
)

target_link_libraries(game_service
    PRIVATE
        common
)
```

Разберем.

```cmake
add_executable(game_service
    main.cpp
)
```

Создает target-бинарь `game_service` из файла `main.cpp`.

```cmake
target_link_libraries(game_service
    PRIVATE
        common
)
```

Говорит: чтобы собрать `game_service`, нужно подключить библиотеку `common`.

После сборки появится исполняемый файл `game_service`.

## 14. CMakeLists.txt для теста

Файл:

```text
library/common/tests/ut/CMakeLists.txt
```

Содержимое:

```cmake
add_executable(common_ut
    version_ut.cpp
)

target_link_libraries(common_ut
    PRIVATE
        common
        test_framework
)

add_test(NAME common_ut COMMAND common_ut)
```

Разберем.

```cmake
add_executable(common_ut
    version_ut.cpp
)
```

Создает тестовый бинарь `common_ut`.

```cmake
target_link_libraries(common_ut
    PRIVATE
        common
        test_framework
)
```

Говорит: тест проверяет библиотеку `common` и использует легкий встроенный
`test_framework`, поэтому нужно подключить обе библиотеки.

Сам `test_framework` устроен как обычная библиотека: публичные макросы и типы
лежат в `include/library/test_framework/test.h`, а registry, runner и
форматирование ошибок реализованы в `src/test.cpp`.

Сам тест выглядит так:

```cpp
#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/common/version.h>

TEST_CASE(version) {
    CHECK_EQ(game::common::version(), "0.1.0");
}
```

`NTEST_MAIN` добавляет готовую точку входа, `TEST_CASE` регистрирует отдельный
тест, а `CHECK`, `CHECK_EQ`, `CHECK_NE` и `CHECK_THROWS_AS` выполняют проверки.
При ошибке runner печатает имя теста, файл, строку и упавшее выражение.

```cmake
add_test(NAME common_ut COMMAND common_ut)
```

Регистрирует тест в CTest. После этого его можно запускать через:

```bash
ctest --test-dir build
```

### 14.1. Как устроен test_framework

Framework состоит из публичного header и реализации:

```text
library/test_framework/
  include/library/test_framework/test.h
  src/test.cpp
```

Жизненный цикл одного тестового executable выглядит так:

```text
TEST_CASE
  -> создает тестовую функцию и registration object
  -> registration object до main кладет имя и указатель на функцию в registry
  -> NTEST_MAIN вызывает run_all_tests()
  -> runner последовательно вызывает функции из registry
  -> CHECK при ошибке передает файл и строку через std::source_location
  -> runner печатает результат и возвращает exit code для CTest
```

`TEST_CASE(name)` создает обычную `static` функцию. Чтобы имена внутренних
функций не конфликтовали, framework добавляет к ним номер строки из `__LINE__`.
Рядом создается `TTestRegistration`, конструктор которого сохраняет пару
`{имя, указатель на функцию}` в общем vector.

Registry находится внутри функции `test_cases()` как function-local static.
Он создается при первом обращении и используется всеми test cases executable.

`CHECK(expression)` передает результат и текст выражения в `NTesting::check`.
Если результат ложный, `fail` формирует сообщение и бросает исключение. Runner
ловит его на границе test case, помечает тест как упавший и продолжает остальные.

`CHECK_THROWS_AS` реализован шаблоном в header, потому что компилятор должен видеть
реализацию шаблона в месте использования. Registry, runner и обычные функции
проверки реализованы в `src/test.cpp`.

В одном тестовом executable `NTEST_MAIN` нужно определять ровно в одном `.cpp`.
Он добавляет готовый `main`, который вызывает runner.

Полный разбор с раскрытием макросов, примером вывода и ограничениями лежит рядом
с библиотекой: [`library/test_framework/README.md`](../library/test_framework/README.md).

## 15. Зачем нужен корневой CMakeLists.txt

Корневой файл лежит здесь:

```text
CMakeLists.txt
```

Он является входной точкой всего проекта.

Когда мы запускаем:

```bash
cmake -S . -B build
```

мы уже знаем, что `-S .` означает “исходники лежат в текущей папке”. Поэтому CMake открывает файл:

```text
./CMakeLists.txt
```

То есть корневой `CMakeLists.txt`.

Корневой `CMakeLists.txt` задает общие настройки проекта:

```cmake
cmake_minimum_required(VERSION 3.20)

project(turn_forge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
```

Это значит:

```text
нужен CMake версии не ниже 3.20
проект называется turn_forge
проект написан на C++
используем стандарт C++20
включаем поддержку тестов
```

Потом корневой файл подключает крупные части проекта:

```cmake
add_subdirectory(library/test_framework)
add_subdirectory(library/common)
add_subdirectory(library/game_core)
add_subdirectory(library/http)
add_subdirectory(library/json)

add_subdirectory(bin/gateway_service)
add_subdirectory(bin/game_service)
add_subdirectory(bin/storage_service)
```

Это не значит, что корневой файл знает, как собирать каждую библиотеку. Он только говорит: зайди в эту папку и прочитай её `CMakeLists.txt`.

Например:

```cmake
add_subdirectory(library/common)
```

говорит CMake:

```text
Открой library/common/CMakeLists.txt и выполни инструкции оттуда.
```

## 16. Почему нет library/CMakeLists.txt

Можно было сделать так:

```text
CMakeLists.txt
library/CMakeLists.txt
library/common/CMakeLists.txt
library/http/CMakeLists.txt
library/json/CMakeLists.txt
```

Тогда корневой файл содержал бы:

```cmake
add_subdirectory(library)
add_subdirectory(bin/gateway_service)
add_subdirectory(bin/game_service)
add_subdirectory(bin/storage_service)
```

А `library/CMakeLists.txt` содержал бы:

```cmake
add_subdirectory(test_framework)
add_subdirectory(common)
add_subdirectory(game_core)
add_subdirectory(http)
add_subdirectory(json)
```

Это тоже нормальная схема.

Почему сейчас подключаем библиотеки прямо из корня?

Потому что проект пока маленький. Так меньше уровней и проще видеть весь стартовый набор target-ов в одном месте.

Когда библиотек станет много, можно добавить промежуточный файл:

```text
library/CMakeLists.txt
```

и перенести туда список библиотек. Это будет чистая организационная правка, не изменение архитектуры.

## 17. Текущая структура проекта

На старте держим минимальный скелет:

```text
bin/
  gateway_service/
  game_service/
  storage_service/

library/
  test_framework/
  common/
  game_core/
  http/
  json/

docs/
```

## 18. План папок проекта

Дальше проект будет расти примерно так:

```text
bin/
  gateway_service/
  game_service/
  storage_service/

library/
  common/
  json/
  game_core/
  async/
  net/
  http/
  websocket/
  rpc/
  game_runtime/
  db/

games/
  tic_tac_toe/
  connect_four/

db/
  migrations/

docker/

web/

docs/
```

Это план, а не список того, что нужно создать сразу. Новые папки добавляем тогда, когда появляется реальная задача под эту часть.

## 19. Как запустить тесты

После сборки:

```bash
ctest --test-dir build
```

Разберем:

```text
ctest
```

Это программа для запуска тестов, которая идет вместе с CMake.

```text
--test-dir build
```

Говорит CTest искать список тестов в build-директории.

Запустить только тесты `common`:

```bash
ctest --test-dir build -R common
```

`-R common` означает:

```text
запусти только тесты, имя которых подходит под common
```

Запустить только тесты `json`:

```bash
ctest --test-dir build -R json
```

### 19.1. Как тесты попадают в CI

CI не хранит отдельный список тестов. Он собирает проект и запускает CTest:

```bash
cmake --build build --parallel
ctest --test-dir build --output-on-failure --no-tests=error
```

Новый тест автоматически запустится в CI, если выполнены три условия:

1. Для него создан executable через `add_executable`.
2. Его `CMakeLists.txt` достижим из корневого CMake через `add_subdirectory`.
3. Executable зарегистрирован в CTest через `add_test`.

Сам workflow для каждого нового теста менять не нужно.

Сейчас CI запускает все тесты. Для маленького проекта это быстрее и надежнее,
чем поддерживать отдельную карту зависимостей между измененными файлами и тестами.
Когда набор тестов станет действительно долгим, его можно разделить с помощью
CTest labels или отдельных задач CI.

Локально можно сразу запускать только нужную часть. Имена тестов начинаются с
названия библиотеки:

```bash
cmake --build build --target json_parser_ut
ctest --test-dir build -R '^json_' --output-on-failure

cmake --build build --target event_loop_ut
ctest --test-dir build -R '^event_loop_' --output-on-failure
```

### 19.2. Строгая сборка и санитайзеры

Общие compiler options лежат в `cmake/CompilerOptions.cmake`. Обычная локальная
сборка включает `-Wall`, `-Wextra`, `-Wpedantic`, `-Wshadow` и `-Wformat=2`, но
не превращает warnings в ошибки.

В CI используется матрица из трех профилей:

```text
GCC / strict            warnings + -Werror
Clang / ASan + UBSan    warnings + -Werror + memory/undefined behavior checks
Clang / TSan            warnings + -Werror + data race checks
```

Запустить те же профили локально на Linux можно так:

```bash
# Строгая сборка
cmake -S . -B build-strict \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTURN_FORGE_WARNINGS_AS_ERRORS=ON
cmake --build build-strict --parallel
ctest --test-dir build-strict --output-on-failure

# AddressSanitizer + UndefinedBehaviorSanitizer
cmake -S . -B build-asan \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTURN_FORGE_WARNINGS_AS_ERRORS=ON \
    -DTURN_FORGE_SANITIZER=address-undefined
cmake --build build-asan --parallel
ctest --test-dir build-asan --output-on-failure

# ThreadSanitizer
cmake -S . -B build-tsan \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTURN_FORGE_WARNINGS_AS_ERRORS=ON \
    -DTURN_FORGE_SANITIZER=thread
cmake --build build-tsan --parallel
ctest --test-dir build-tsan --output-on-failure
```

ASan и TSan запускаются отдельно: эти runtimes нельзя надежно объединить в одном
процессе. MSan намеренно не включен, потому что ему нужна инструментированная
стандартная библиотека; для текущего учебного проекта это слишком тяжелая
инфраструктура.

## 20. Полный учебный образец: library/common

`library/common` — небольшая реальная библиотека проекта, на которой удобно увидеть
раскладку файлов, CMake и тестов.

Файл:

```text
library/common/include/library/common/version.h
```

объявляет функцию:

```cpp
std::string version();
```

Файл:

```text
library/common/src/version.cpp
```

реализует эту функцию.

Файл:

```text
library/common/tests/ut/version_ut.cpp
```

проверяет эту функцию.

Собрать и запустить только тест `common`:

```bash
cmake --build build --target common_ut
ctest --test-dir build -R common
```

Когда ученик будет делать `library/json`, он должен повторять эту же схему: публичные `.h` в `include/library/json`, реализация в `src`, unit-тесты в `tests/ut`, отдельные targets в CMake.

## 21. Как добавлять новую библиотеку

Например, хотим добавить `library/net`.

Создаем структуру:

```text
library/net/
  CMakeLists.txt
  include/library/net/socket.h
  src/socket.cpp
  tests/ut/CMakeLists.txt
  tests/ut/socket_ut.cpp
```

Пишем `library/net/CMakeLists.txt`:

```cmake
add_library(net
    src/socket.cpp
)

target_include_directories(net
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Пишем `library/net/tests/ut/CMakeLists.txt`:

```cmake
add_executable(net_ut
    socket_ut.cpp
)

target_link_libraries(net_ut
    PRIVATE
        net
        test_framework
)

add_test(NAME net_ut COMMAND net_ut)
```

Добавляем в корневой `CMakeLists.txt`:

```cmake
add_subdirectory(library/net)
```

После этого можно собрать библиотеку:

```bash
cmake --build build --target net
```

и её тесты:

```bash
cmake --build build --target net_ut
ctest --test-dir build -R net
```
