# How To Develop

Этот документ кратко объясняет структуру проекта, сборку через CMake и написание тестов.

## 1. Структура проекта

```text
bin/                    исполняемые сервисы
library/                переиспользуемые библиотеки
  common/               общие типы и утилиты
  game_core/            интерфейсы игровой логики
  http/                 HTTP-типы
  json/                 JSON parser, value и serializer
  test_framework/       легкий встроенный test framework
docs/                   документация
```

У обычной библиотеки три основные части:

```text
library/<name>/
  include/              публичные headers
  src/                  реализация
  tests/ut/             unit-тесты
  CMakeLists.txt        правила сборки
```

Публичный header описывает API библиотеки. Файл `.cpp` содержит реализацию. Другие
targets подключают header через `#include`, а реализацию получают через линковку.

## 2. Корневой CMakeLists.txt

Корневой файл задает общие настройки:

```cmake
cmake_minimum_required(VERSION 3.20)

project(turn_forge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
```

Подпроекты подключаются через `add_subdirectory`:

```cmake
add_subdirectory(library/test_framework)
add_subdirectory(library/common)
add_subdirectory(library/game_core)
add_subdirectory(library/http)
add_subdirectory(library/json)
```

Порядок важен: target должен быть объявлен до того, как другой target начнет на него
ссылаться.

## 3. CMakeLists.txt библиотеки

Библиотека с `.cpp` файлами создается через `add_library`:

```cmake
add_library(common
    src/version.cpp
)

target_include_directories(common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

`PUBLIC` означает, что include-директория доступна самой библиотеке и всем targets,
которые с ней линкуются.

Если одна библиотека использует другую:

```cmake
target_link_libraries(http
    PUBLIC
        common
)
```

`PUBLIC` передает зависимость дальше потребителям `http`. `PRIVATE` подключает её
только к текущему target.

Header-only библиотека создается как `INTERFACE`:

```cmake
add_library(game_game_core INTERFACE)

target_include_directories(game_game_core
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

## 4. Добавление новой библиотеки

Допустим, нужна библиотека `net`:

```text
library/net/
  include/library/net/socket.h
  src/socket.cpp
  tests/ut/socket_ut.cpp
  tests/ut/CMakeLists.txt
  CMakeLists.txt
```

В `library/net/CMakeLists.txt`:

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

В корневой `CMakeLists.txt` добавляется:

```cmake
add_subdirectory(library/net)
```

## 5. Встроенный test framework

В проекте используется маленький framework без внешних зависимостей. Публичные
макросы и типы лежат в `include/library/test_framework/test.h`, а registry, runner
и форматирование ошибок — в `src/test.cpp`. Чтобы framework добавил готовый `main`,
определите `NTEST_MAIN` до include:

```cpp
#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/http/version.h>

TEST_CASE(default_http_version) {
    const NHttp::THttpVersion version;

    CHECK_EQ(version.major, 1);
    CHECK_EQ(version.minor, 1);
}
```

Один тестовый executable должен определять `NTEST_MAIN` только в одном `.cpp` файле.

Доступные проверки:

```cpp
CHECK(value > 0);
CHECK_EQ(actual, expected);
CHECK_NE(actual, unexpected);
CHECK_THROWS_AS(parse(input), TParseError);
STATIC_CHECK(sizeof(int) >= 4);
```

`CHECK` и связанные макросы останавливают только текущий test case. Runner продолжает
остальные тесты и в конце выводит сводку. При падении показываются имя test case,
файл, строка и выражение.

## 6. CMakeLists.txt теста

```cmake
add_executable(http_version_ut
    http_version_ut.cpp
)

target_link_libraries(http_version_ut
    PRIVATE
        http
        test_framework
)

add_test(NAME http_version_ut COMMAND http_version_ut)
```

Здесь тест линкуется с проверяемой библиотекой и `test_framework`. `add_test`
регистрирует executable в CTest.

## 7. Сборка

Настроить build-директорию:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

Собрать весь проект:

```bash
cmake --build build --parallel
```

Собрать один target:

```bash
cmake --build build --target http_class_ut
```

Build-директория не хранится в Git. Если конфигурация сломалась после больших
изменений CMake, можно удалить только `build/` и настроить её заново.

## 8. Запуск тестов

Запустить все тесты:

```bash
ctest --test-dir build --output-on-failure
```

Запустить тесты по имени:

```bash
ctest --test-dir build -R http --output-on-failure
```

Сам тестовый executable тоже можно запустить напрямую, чтобы увидеть каждый test
case:

```bash
./build/library/http/tests/ut/http_class_ut
```

## 9. Перед pull request

Минимальная локальная проверка:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure --no-tests=error
```

После push те же шаги выполняет Linux CI. В `trunk` изменения попадают только через
pull request.

## 10. Короткие правила

- Публичные headers кладем в `include/library/<name>/`.
- Реализацию кладем в `src/`.
- Unit-тесты кладем в `tests/ut/`.
- Новый target подключаем через CMake, а не ручными командами компилятора.
- Для тестов используем `test_framework`, не `assert` и не самописные `check`.
- Не коммитим `build/`, бинарные файлы и временные логи.
