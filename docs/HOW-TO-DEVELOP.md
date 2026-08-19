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
bin/heart_writer/main.cpp
library/example/include/game/example/heart.h
library/example/src/heart.cpp
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
library/example/include/game/example/heart.h
library/example/src/heart.cpp
bin/heart_writer/main.cpp
```

`heart.h` - публичный header библиотеки `example`.

`heart.cpp` - реализация библиотеки `example`.

`main.cpp` - код бинаря `heart_writer`, который использует библиотеку `example`.

### 1.2. Что такое header

Header обычно отвечает на вопрос:

```text
Что существует и как этим пользоваться?
```

Например файл:

```text
library/example/include/game/example/heart.h
```

содержит:

```cpp
#pragma once

#include <string>

namespace game::example {

std::string heart();
std::string heart_message(const std::string& name);

} // namespace game::example
```

Этот header говорит компилятору:

```text
Есть namespace game::example.
В нем есть функция heart().
Она не принимает аргументов и возвращает std::string.
В нем есть функция heart_message(...).
Она принимает const std::string& и возвращает std::string.
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
bin/heart_writer/main.cpp
```

он не читает автоматически:

```text
library/example/src/heart.cpp
```

Он не сканирует весь проект в поисках функции `heart_message()`.

Он видит только:

```text
1. сам main.cpp
2. файлы, которые подключены через #include
```

Поэтому в `main.cpp` написано:

```cpp
#include <game/example/heart.h>
```

Это значит:

```text
Перед компиляцией main.cpp открой header game/example/heart.h и подставь его содержимое в этот файл.
```

После этого компилятор видит объявление:

```cpp
std::string heart_message(const std::string& name);
```

И может проверить строку:

```cpp
game::example::heart_message("from heart_writer")
```

Компилятор понимает:

```text
такая функция объявлена
она принимает строку
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
library/example/src/heart.cpp
```

содержит:

```cpp
#include <game/example/heart.h>

namespace game::example {

std::string heart() {
    return "<3";
}

std::string heart_message(const std::string& name) {
    if (name.empty()) {
        return heart();
    }

    return heart() + " " + name;
}

} // namespace game::example
```

Тут уже есть настоящий код функций.

Почему `heart.cpp` тоже подключает `heart.h`?

Чтобы компилятор проверил, что реализация совпадает с объявлением.

Если в header написано:

```cpp
std::string heart();
```

а в `.cpp` случайно написать:

```cpp
int heart() {
    return 1;
}
```

компилятор увидит конфликт и сообщит об ошибке.

То есть header нужен не только пользователям библиотеки, но и самой реализации: он помогает держать объявление и реализацию синхронными.

### 1.5. Что происходит при #include

Строка:

```cpp
#include <game/example/heart.h>
```

для новичка может выглядеть как “импорт библиотеки”. Но технически это не совсем импорт.

На этапе препроцессинга C++ компилятор фактически вставляет содержимое header-а в текущий `.cpp` файл.

Упрощенно, было:

```cpp
#include <game/example/heart.h>

int main() {
    game::example::heart_message("from heart_writer");
}
```

После обработки include компилятор как будто видит:

```cpp
#include <string>

namespace game::example {

std::string heart();
std::string heart_message(const std::string& name);

}

int main() {
    game::example::heart_message("from heart_writer");
}
```

Именно поэтому компилятору надо знать, где искать header-файлы.

Если он не знает, где лежит `game/example/heart.h`, будет ошибка вида:

```text
fatal error: game/example/heart.h: No such file or directory
```

### 1.6. Что такое объектный файл

Объектный файл - это промежуточный результат компиляции одного `.cpp` файла.

Когда компилятор компилирует:

```text
library/example/src/heart.cpp
```

он получает примерно такой файл:

```text
heart.cpp.o
```

Когда компилятор компилирует:

```text
bin/heart_writer/main.cpp
```

он получает примерно такой файл:

```text
main.cpp.o
```

Объектный файл уже содержит машинный код, но это еще не обязательно готовая программа.

Например `main.cpp.o` может содержать код `main()`, но внутри него остается ссылка:

```text
где-то должна быть функция game::example::heart_message(...)
```

А `heart.cpp.o` содержит реализацию этой функции.

Чтобы получить готовый бинарь, эти объектные файлы надо соединить.

Этим занимается линкер.

### 1.7. Что делает линкер

Линкер соединяет скомпилированные объектные файлы и библиотеки.

Упрощенно:

```text
main.cpp.o говорит: я вызываю game::example::heart_message(...)
heart.cpp.o говорит: я реализую game::example::heart_message(...)
линкер соединяет вызов с реализацией
```

Если реализация не найдена, будет ошибка линковки.

Например, если `main.cpp` вызвал `heart_message()`, но мы забыли слинковать бинарь с `game_example`, компиляция `main.cpp` может пройти, а линковка упадет.

Типичная идея ошибки будет такая:

```text
undefined reference to game::example::heart_message(...)
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

В нашем примере `library/example` собирается в target:

```text
game_example
```

Сейчас внутри него один `.cpp`:

```text
library/example/src/heart.cpp
```

Но позже библиотека может содержать много файлов:

```text
src/heart.cpp
src/format.cpp
src/writer.cpp
```

CMake соберет их в одну библиотеку, и другие бинарники смогут подключать её одной зависимостью:

```cmake
target_link_libraries(heart_writer
    PRIVATE
        game_example
)
```

### 1.9. Пошагово: как собирается library/example

CMake-файл библиотеки:

```text
library/example/CMakeLists.txt
```

содержит:

```cmake
add_library(game_example
    src/heart.cpp
)

target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Что происходит при сборке target-а `game_example`:

```bash
cmake --build build --target game_example
```

Шаги примерно такие:

```text
1. CMake видит target game_example.
2. CMake знает, что game_example собирается из src/heart.cpp.
3. Система сборки запускает компилятор для heart.cpp.
4. Компилятор открывает heart.cpp.
5. В heart.cpp есть #include <game/example/heart.h>.
6. Компилятор ищет этот header в include-путях target-а game_example.
7. include-путь был задан через target_include_directories(... PUBLIC .../include).
8. Компилятор находит library/example/include/game/example/heart.h.
9. Компилятор проверяет объявления из heart.h.
10. Компилятор компилирует heart.cpp в объектный файл heart.cpp.o.
11. Система сборки упаковывает объектный файл в библиотеку game_example.
```

Итог:

```text
game_example собран
внутри есть реализация heart() и heart_message()
наружу доступен public header game/example/heart.h
```

### 1.10. Пошагово: как собирается bin/heart_writer

Бинарь лежит здесь:

```text
bin/heart_writer/main.cpp
```

Он содержит:

```cpp
#include <iostream>

#include <game/example/heart.h>

int main() {
    std::cout << game::example::heart_message("from heart_writer") << '\n';
    return 0;
}
```

CMake-файл бинаря:

```text
bin/heart_writer/CMakeLists.txt
```

содержит:

```cmake
add_executable(heart_writer
    main.cpp
)

target_link_libraries(heart_writer
    PRIVATE
        game_example
)
```

Что происходит при сборке:

```bash
cmake --build build --target heart_writer
```

Шаги примерно такие:

```text
1. CMake видит target heart_writer.
2. CMake знает, что heart_writer собирается из main.cpp.
3. CMake видит, что heart_writer зависит от game_example.
4. Если game_example еще не собран, сначала собирается game_example.
5. Компилятор компилирует bin/heart_writer/main.cpp.
6. В main.cpp есть #include <game/example/heart.h>.
7. heart_writer сам не задавал include-путь к library/example/include.
8. Но heart_writer связан с game_example через target_link_libraries(... game_example).
9. У game_example include-директория объявлена как PUBLIC.
10. Поэтому include-путь library/example/include автоматически доступен heart_writer.
11. Компилятор находит heart.h и понимает, как вызывать heart_message().
12. Компилятор делает объектный файл main.cpp.o.
13. Линкер соединяет main.cpp.o с библиотекой game_example.
14. В библиотеке game_example находится реализация heart_message().
15. На выходе получается исполняемый файл heart_writer.
```

Итог:

```text
main.cpp знает про функцию из header-а
реализация функции лежит в game_example
линкер соединяет heart_writer с game_example
получается готовый бинарь heart_writer
```

### 1.11. Что будет, если что-то забыть

Если забыть `#include <game/example/heart.h>`, компилятор не поймет вызов:

```cpp
game::example::heart_message("from heart_writer")
```

Проблема будет на этапе компиляции.

Если оставить include, но убрать зависимость:

```cmake
target_link_libraries(heart_writer
    PRIVATE
        game_example
)
```

то возможны две проблемы.

Первая: компилятор может не найти header:

```text
fatal error: game/example/heart.h: No such file or directory
```

Вторая: если header каким-то образом найден, линкер может не найти реализацию:

```text
undefined reference to game::example::heart_message(...)
```

Поэтому для использования библиотеки обычно нужны обе части:

```text
#include <...>              чтобы компилятор увидел объявления
target_link_libraries(...)  чтобы сборка получила include-пути и реализацию
```

### 1.12. Короткая схема

```text
heart.h
  объявляет функции
  нужен компилятору при #include

heart.cpp
  реализует функции
  компилируется в объектный файл

game_example
  библиотека из heart.cpp
  публикует include/game/example/heart.h

heart_writer/main.cpp
  подключает heart.h
  вызывает heart_message()

heart_writer
  бинарь
  линкуется с game_example
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
библиотека       game_example
бинарь           game_service
тестовый бинарь  game_example_ut
```

Библиотека - это переиспользуемый код. Например `game_example` или будущая `game_json`.

Бинарь - это исполняемый файл, который можно запустить. Например `game_service`.

Тестовый бинарь - это обычный исполняемый файл, но он нужен только для проверки кода. Например `game_example_ut`.

Собрать все targets проекта:

```bash
cmake --build build
```

Собрать один конкретный target:

```bash
cmake --build build --target game_example
```

Разберем:

```text
cmake --build build
```

Собрать проект из папки `build`.

```text
--target game_example
```

Но собрать не всё, а только target `game_example`.

Важно: CMake обычно собирает не папки, а targets.

Папка `library/example` - это место, где лежит код. А `game_example` - это target, который из этого кода собирается.

## 6. Что такое заголовки и почему CMake должен их искать

Когда в `.cpp` файле написано:

```cpp
#include <game/example/heart.h>
```

компилятор должен найти файл `heart.h`.

Но сам по себе он не знает, где искать header нашего проекта. Поэтому в `CMakeLists.txt` мы пишем:

```cmake
target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Это значит:

```text
Для target-а game_example добавь папку library/example/include в список мест, где компилятор ищет headers.
```

`${CMAKE_CURRENT_SOURCE_DIR}` - это текущая папка, где лежит данный `CMakeLists.txt`.

Если мы находимся в:

```text
library/example/CMakeLists.txt
```

то:

```text
${CMAKE_CURRENT_SOURCE_DIR}
```

примерно означает:

```text
/home/.../game-service/library/example
```

Поэтому:

```cmake
${CMAKE_CURRENT_SOURCE_DIR}/include
```

означает:

```text
/home/.../game-service/library/example/include
```

После этого такой include:

```cpp
#include <game/example/heart.h>
```

ведет к файлу:

```text
library/example/include/game/example/heart.h
```

То есть часть пути до `include` добавляет CMake, а часть после `include` пишется в `#include`.

## 7. Почему путь внутри include такой длинный

Можно было бы положить файл так:

```text
library/example/include/heart.h
```

и писать:

```cpp
#include <heart.h>
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
library/example/include/game/example/heart.h
library/json/include/game/json/json_value.h
library/http/include/game/http/http_parser.h
```

И в коде пишем явно:

```cpp
#include <game/example/heart.h>
#include <game/json/json_value.h>
#include <game/http/http_parser.h>
```

Это длиннее, зато понятно и меньше конфликтов.

## 8. Когда нужно писать target_include_directories

Пишем `target_include_directories`, когда у target-а есть headers, которые надо подключать через `#include`.

Для библиотеки почти всегда будет так:

```cmake
target_include_directories(game_example
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
#include <game/common/version.h>
```

то сам `game_service` не обязан вручную знать, где лежит `game/common/version.h`. Ему достаточно слинковаться с библиотекой:

```cmake
target_link_libraries(game_service
    PRIVATE
        game_common
)
```

А `game_common` уже сам объявил свои include-пути через `target_include_directories(game_common PUBLIC ...)`.

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
        game_common
)
```

`game_service` сам использует `game_common`, но никто другой не будет подключать `game_service` как библиотеку. Поэтому зависимость приватная.

Пример `PUBLIC`:

```cmake
target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

Headers нужны самой библиотеке `game_example`, и они также нужны всем, кто будет использовать `game_example`.

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

Основной пример - `library/example`:

```text
library/example/
  CMakeLists.txt
  include/
    game/
      example/
        heart.h
  src/
    heart.cpp
  tests/
    ut/
      CMakeLists.txt
      heart_ut.cpp
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
heart.h
heart.cpp
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
library/example/CMakeLists.txt
```

Содержимое:

```cmake
add_library(game_example
    src/heart.cpp
)

target_include_directories(game_example
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Разберем построчно.

```cmake
add_library(game_example
    src/heart.cpp
)
```

Создает target-библиотеку с именем `game_example`.

`src/heart.cpp` - файл, который надо скомпилировать внутрь этой библиотеки.

```cmake
target_include_directories(game_example
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
        game_common
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
        game_common
)
```

Говорит: чтобы собрать `game_service`, нужно подключить библиотеку `game_common`.

После сборки появится исполняемый файл `game_service`.

## 14. CMakeLists.txt для теста

Файл:

```text
library/example/tests/ut/CMakeLists.txt
```

Содержимое:

```cmake
add_executable(game_example_ut
    heart_ut.cpp
)

target_link_libraries(game_example_ut
    PRIVATE
        game_example
        test_framework
)

add_test(NAME game_example_ut COMMAND game_example_ut)
```

Разберем.

```cmake
add_executable(game_example_ut
    heart_ut.cpp
)
```

Создает тестовый бинарь `game_example_ut`.

```cmake
target_link_libraries(game_example_ut
    PRIVATE
        game_example
        test_framework
)
```

Говорит: тест проверяет библиотеку `game_example` и использует легкий встроенный
`test_framework`, поэтому нужно подключить обе библиотеки.

Сам тест выглядит так:

```cpp
#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <game/example/heart.h>

TEST_CASE(heart) {
    CHECK_EQ(game::example::heart(), "<3");
}

TEST_CASE(heart_message) {
    CHECK_EQ(game::example::heart_message("json"), "<3 json");
}
```

`NTEST_MAIN` добавляет готовую точку входа, `TEST_CASE` регистрирует отдельный
тест, а `CHECK`, `CHECK_EQ`, `CHECK_NE` и `CHECK_THROWS_AS` выполняют проверки.
При ошибке runner печатает имя теста, файл, строку и упавшее выражение.

```cmake
add_test(NAME game_example_ut COMMAND game_example_ut)
```

Регистрирует тест в CTest. После этого его можно запускать через:

```bash
ctest --test-dir build
```

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
add_subdirectory(library/example)
add_subdirectory(library/game_core)
add_subdirectory(library/json)

add_subdirectory(bin/gateway_service)
add_subdirectory(bin/heart_writer)
add_subdirectory(bin/game_service)
add_subdirectory(bin/storage_service)
```

Это не значит, что корневой файл знает, как собирать каждую библиотеку. Он только говорит: зайди в эту папку и прочитай её `CMakeLists.txt`.

Например:

```cmake
add_subdirectory(library/example)
```

говорит CMake:

```text
Открой library/example/CMakeLists.txt и выполни инструкции оттуда.
```

## 16. Почему нет library/CMakeLists.txt

Можно было сделать так:

```text
CMakeLists.txt
library/CMakeLists.txt
library/common/CMakeLists.txt
library/example/CMakeLists.txt
library/json/CMakeLists.txt
```

Тогда корневой файл содержал бы:

```cmake
add_subdirectory(library)
add_subdirectory(bin/gateway_service)
add_subdirectory(bin/heart_writer)
add_subdirectory(bin/game_service)
add_subdirectory(bin/storage_service)
```

А `library/CMakeLists.txt` содержал бы:

```cmake
add_subdirectory(common)
add_subdirectory(example)
add_subdirectory(game_core)
add_subdirectory(json)
```

Это тоже нормальная схема.

Почему сейчас подключаем `library/common`, `library/example`, `library/json` прямо из корня?

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
  heart_writer/
  game_service/
  storage_service/

library/
  test_framework/
  common/
  example/
  game_core/
  json/

docs/
```

## 18. План папок проекта

Дальше проект будет расти примерно так:

```text
bin/
  gateway_service/
  heart_writer/
  game_service/
  storage_service/

library/
  common/
  example/
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

Запустить только тесты примера:

```bash
ctest --test-dir build -R example
```

`-R example` означает:

```text
запусти только тесты, имя которых подходит под example
```

Запустить только тесты `json`, когда они появятся:

```bash
ctest --test-dir build -R json
```

## 20. Полный учебный пример: library/example

В проекте есть маленькая полностью реализованная библиотека `library/example`. Она не относится к продуктовой логике, а нужна как образец раскладки файлов, CMake и тестов.

Файл:

```text
library/example/include/game/example/heart.h
```

объявляет функции:

```cpp
std::string heart();
std::string heart_message(const std::string& name);
```

Файл:

```text
library/example/src/heart.cpp
```

реализует эти функции.

Файл:

```text
library/example/tests/ut/heart_ut.cpp
```

проверяет эти функции.

Собрать и запустить только тест примера:

```bash
cmake --build build --target game_example_ut
ctest --test-dir build -R example
```

Когда ученик будет делать `library/json`, он должен повторять эту же схему: публичные `.h` в `include/game/json`, реализация в `src`, unit-тесты в `tests/ut`, отдельные targets в CMake.

## 21. Как добавлять новую библиотеку

Например, хотим добавить `library/net`.

Создаем структуру:

```text
library/net/
  CMakeLists.txt
  include/game/net/socket.h
  src/socket.cpp
  tests/ut/CMakeLists.txt
  tests/ut/socket_ut.cpp
```

Пишем `library/net/CMakeLists.txt`:

```cmake
add_library(game_net
    src/socket.cpp
)

target_include_directories(game_net
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(tests/ut)
```

Пишем `library/net/tests/ut/CMakeLists.txt`:

```cmake
add_executable(game_net_ut
    socket_ut.cpp
)

target_link_libraries(game_net_ut
    PRIVATE
        game_net
)

add_test(NAME game_net_ut COMMAND game_net_ut)
```

Добавляем в корневой `CMakeLists.txt`:

```cmake
add_subdirectory(library/net)
```

После этого можно собрать библиотеку:

```bash
cmake --build build --target game_net
```

и её тесты:

```bash
cmake --build build --target game_net_ut
ctest --test-dir build -R net
```
