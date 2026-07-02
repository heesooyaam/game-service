# План проекта Game Service

## 1. Общая идея

Проект называется **Game Service**.

Это учебная C++ backend-платформа для онлайн-игр. Первая игра — крестики-нолики, но основная цель не сама игра, а реализация серверной инфраструктуры:

* TCP;
* non-blocking sockets;
* event loop;
* корутины поверх event loop;
* HTTP;
* WebSocket;
* JSON;
* внутренний RPC между сервисами;
* микросервисная архитектура;
* PostgreSQL;
* matchmaking;
* рейтинг;
* история игр;
* расширяемый game runtime.

Идея проекта: сделать не просто `tic_tac_toe_server`, а маленькую платформу, на базе которой можно реализовывать разные игры.

---

## 2. Главная учебная цель

Проект должен показать ученику, как устроен backend изнутри:

* как сервер принимает TCP-соединения;
* как работает HTTP;
* как происходит WebSocket Upgrade;
* как идут сообщения по WebSocket;
* как устроен внутренний RPC;
* как сервисы общаются друг с другом;
* как обрабатывать много соединений через event loop;
* как поверх event loop строятся корутины;
* как отделять транспортную логику от продуктовой;
* как хранить данные через отдельный storage-сервис;
* как расширять систему новой игрой без копипасты.

---

## 3. Стек

Основной стек:

```text
C++20
CMake
Linux
PostgreSQL
libpqxx или другой готовый C++ PostgreSQL client
Docker Compose
GitHub
GitHub Issues
GitHub Projects
tmux / VS Code Remote SSH
```

Клиент можно сделать максимально простым и навайбкодить отдельно. Главный фокус — C++ backend.

---

## 4. Что пишем руками

Пишем руками:

```text
TCP server/client
non-blocking sockets
event loop
task queue
timer queue
coroutine runtime
HTTP parser
HTTP response builder
WebSocket handshake
WebSocket frame parser/writer
JSON parser/writer
JSON RPC over TCP
game runtime
matchmaking
rating logic
history logic
```

Не пишем руками:

```text
PostgreSQL wire protocol
TLS implementation
password hashing algorithm
cryptography
production-grade HTTP implementation
production-grade WebSocket implementation
production-grade JSON parser
```

Для PostgreSQL берём готовый клиент, но SQL пишем руками.

---

## 5. Основные сервисы

В первой версии делаем три C++ сервиса:

```text
gateway_service
game_service
storage_service
```

И отдельно:

```text
PostgreSQL
```

Общая схема:

```text
web client
   |
   | HTTP / WebSocket
   v
gateway_service
   |
   | handmade JSON RPC over TCP
   v
game_service
   |
   | handmade JSON RPC over TCP
   v
storage_service
   |
   | libpqxx / PostgreSQL client
   v
PostgreSQL
```

Клиент общается только с `gateway_service`.

---

## 6. gateway_service

`gateway_service` — внешний вход в систему.

Он отвечает за:

```text
TCP accept
HTTP parsing
HTTP routing
HTTP response building
WebSocket Upgrade
WebSocket frame reading/writing
хранение активных WebSocket-сессий
маппинг user_id -> session
проверку session token через storage_service
маршрутизацию команд в game_service
отправку событий от game_service клиентам
```

`gateway_service` не должен знать:

```text
правила конкретной игры
SQL
схему базы
как считается рейтинг
как сохраняется история
```

Внешние HTTP endpoints:

```text
GET  /health
POST /register
POST /login
GET  /profile
GET  /leaderboard
GET  /history
GET  /ws
```

WebSocket-команды:

```text
game.find_match
game.cancel_matchmaking
game.make_move
game.resign
game.get_state
```

---

## 7. HTTP в проекте

HTTP точно используется.

Клиент общается с `gateway_service` по HTTP для обычных запросов:

```text
register
login
profile
leaderboard
history
health
```

WebSocket тоже начинается с HTTP-запроса:

```text
GET /ws HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: ...
Sec-WebSocket-Version: 13
```

Сервер отвечает:

```text
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: ...
```

После этого соединение становится WebSocket.

---

## 8. HTTPS / безопасность

В v1 используем:

```text
client -> gateway_service: HTTP / WS
gateway_service -> game_service: plain JSON RPC over TCP
gateway_service -> storage_service: plain JSON RPC over TCP
game_service -> storage_service: plain JSON RPC over TCP
```

Важно: если гонять логины, пароли и токены по обычному HTTP через публичный интернет, это небезопасно.

Поэтому v1 считается учебной/dev-версией.

Минимальные меры безопасности для v1:

```text
наружу открыт только gateway_service
game_service слушает только localhost или docker internal network
storage_service слушает только localhost или docker internal network
PostgreSQL не торчит наружу
доступ к dev-тачке только по SSH
пароли в БД не хранятся, только password_hash
session token в БД не хранится в открытом виде, только token_hash
токены не логируются
```

Для публичного запуска нужен HTTPS/WSS.

Переезд на HTTPS/WSS:

```text
browser
  |
  | HTTPS / WSS
  v
reverse proxy
  |
  | HTTP / WS
  v
gateway_service
```

TLS termination делает reverse proxy:

```text
nginx
caddy
traefik
```

C++-код TLS руками не реализует.

Этапы:

```text
v1:
  HTTP / WS напрямую, только dev/internal usage

v2:
  HTTPS / WSS через reverse proxy
  gateway_service всё ещё принимает обычный HTTP / WS

v3:
  при желании — TLS/mTLS между внутренними сервисами
```

v3 не нужен для первой версии.

---

## 9. game_service

`game_service` — сервис игрового runtime.

Он отвечает за:

```text
matchmaking
active rooms
game_id -> room
user_id -> active game
обработку ходов
проверку очередности хода
вызов правил конкретной игры
завершение игры
отправку событий в gateway_service
сохранение результата через storage_service
```

На первом этапе `game_service` поддерживает turn-based игры.

Игры подключаются как модули:

```text
games/tic_tac_toe
games/connect_four
```

То есть сначала:

```text
один game_service
несколько game modules
```

Не делаем отдельный сервис на каждую игру в первой версии.

---

## 10. storage_service

`storage_service` — сервис постоянного хранения данных.

Он единственный ходит в PostgreSQL.

Он отвечает за:

```text
пользователей
регистрацию
логин
сессии
проверку токенов
рейтинг
историю игр
создание game record
сохранение ходов
завершение игры
транзакции
SQL-запросы
```

Остальные сервисы не ходят в PostgreSQL напрямую.

`storage_service` не должен быть удалённым SQL-клиентом.

Плохо:

```json
{
  "query": "SELECT * FROM users"
}
```

Хорошо:

```json
{
  "method": "storage.LoginUser",
  "payload": {
    "username": "alice",
    "password": "12345"
  }
}
```

Минимальные RPC-методы:

```text
storage.CreateUser
storage.LoginUser
storage.ValidateSession
storage.GetUserProfile
storage.GetLeaderboard
storage.CreateGame
storage.SaveMove
storage.FinishGame
storage.GetUserGameHistory
```

---

## 11. Внутренний RPC

Между сервисами в v1 используем handmade JSON RPC over TCP.

Формат:

```text
[4 bytes body_size][json body]
```

Request:

```json
{
  "type": "request",
  "request_id": "req-123",
  "method": "game.MakeMove",
  "payload": {
    "user_id": 42,
    "game_id": 1001,
    "move": {
      "cell": 4
    }
  }
}
```

Response:

```json
{
  "type": "response",
  "request_id": "req-123",
  "ok": true,
  "payload": {
    "accepted": true
  }
}
```

Error:

```json
{
  "type": "response",
  "request_id": "req-123",
  "ok": false,
  "error": {
    "code": "invalid_move",
    "message": "Cell is already occupied"
  }
}
```

Event:

```json
{
  "type": "event",
  "event": "game.StateUpdated",
  "payload": {
    "game_id": 1001,
    "players": [42, 55],
    "state": {
      "board": ["x", null, null, null, "o", null, null, null, null],
      "next_turn_user_id": 42
    }
  }
}
```

gRPC/protobuf на первом этапе не используем.

Позже можно сделать отдельный этап: заменить RPC к `storage_service` на gRPC/protobuf, чтобы ученик сравнил handmade RPC и промышленный RPC.

---

## 12. Структура репозитория

Репозиторий:

```text
Game Service/
  apps/
    gateway_service/
    game_service/
    storage_service/

  libs/
    async/
    net/
    http/
    websocket/
    json/
    rpc/
    game_core/
    game_runtime/
    common/
    db/

  games/
    tic_tac_toe/
    connect_four/

  db/
    migrations/

  docker/
    docker-compose.yml

  web/

  tests/

  docs/
    architecture.md
    protocol.md
    code_style.md
```

---

## 13. Асинхронная модель

Асинхронность — одна из центральных частей проекта.

Модель:

```text
один event loop = один поток
один event loop = много соединений
одна пользовательская сессия всегда живёт в одном event loop
одна game room всегда живёт в одном game loop
```

Главное правило:

```text
Объект можно изменять только из его owning event loop.
Из другого потока можно только сделать loop.Post(...).
```

Это уменьшает количество mutex’ов и делает владение состоянием понятнее.

---

## 14. libs/async

`libs/async` реализуется сразу после базовых сокетов.

Нужно реализовать:

```text
TEventLoop
TEventLoopGroup
TPoller
TEpollPoller
TTaskQueue
TTimerQueue
loop.Post(...)
wakeup через eventfd или pipe
timers
```

`TEventLoop` делает:

```text
ждёт события от epoll
обрабатывает readable/writable sockets
выполняет pending tasks
обрабатывает timers
```

Blocking операции внутри event loop запрещены.

---

## 15. Корутины

Корутины не откладываем в конец.

Новый порядок:

```text
1. blocking TCP echo
2. non-blocking sockets + EventLoop
3. minimal coroutine runtime
4. HTTP
5. JSON
6. WebSocket
7. RPC with coroutine API
8. storage_service
9. game_core + tic_tac_toe
10. online game
```

Корутины не заменяют event loop. Они живут поверх него.

Минимально нужно:

```text
TTask<T>
TTask<void>
TDetachedTask
Spawn(...)
ScheduleOn(event_loop)
SleepFor(...)
co_await rpc_call позже
co_await db_pool.Submit(...) позже
```

Верхнеуровневые обработчики должны быть coroutine-based.

Желаемый стиль:

```cpp
TTask<void> TGatewaySession::handle_make_move(TJsonValue request) {
    auto auth = co_await storage_client_.validate_session(session_token_);

    if (!auth.valid) {
        send_error(request, "unauthorized");
        co_return;
    }

    auto response = co_await game_client_.make_move(
        auth.user_id,
        request["payload"]["game_id"].as_int(),
        request["payload"]["move"]
    );

    send_response(request, response.to_json());
}
```

Не хотим тащить callback hell:

```cpp
void TGatewaySession::handle_make_move(TJsonValue request) {
    storage_client_.validate_session(session_token_, [this, request](auto auth) {
        game_client_.make_move(auth.user_id, request, [this, request](auto response) {
            send_response(request, response.to_json());
        });
    });
}
```

Правило для корутин:

```text
После co_await корутина должна продолжить выполнение в owning event loop.
```

Если `TGatewaySession` живёт в `event_loop_2`, то после RPC-ответа coroutine continuation должна вернуться в `event_loop_2`.

---

## 16. Асинхронность в gateway_service

Модель:

```text
acceptor thread
  -> event_loop_0: client sessions
  -> event_loop_1: client sessions
  -> event_loop_2: client sessions
  -> event_loop_3: client sessions
```

Новое соединение назначается в loop по round-robin.

Одна WebSocket-сессия всегда обрабатывается в одном и том же потоке.

Это позволяет не ставить mutex на:

```text
read buffer
write buffer
WebSocket parser state
session state
```

---

## 17. Асинхронность в game_service

Модель:

```text
game_loop_0 -> rooms 1, 4, 8
game_loop_1 -> rooms 2, 5, 9
game_loop_2 -> rooms 3, 6, 10
```

Одна game room живёт в одном game loop.

Это позволяет не ставить mutex на состояние одной игры.

---

## 18. Асинхронность в storage_service

PostgreSQL-клиент, скорее всего, blocking.

Поэтому:

```text
rpc event loop
  -> db worker pool
  -> blocking PostgreSQL query
  -> post result back to rpc event loop
  -> send RPC response
```

SQL-запросы нельзя выполнять прямо в event loop.

---

## 19. HTTP subset

Поддерживаем учебный subset HTTP.

Нужно:

```text
request line
method
path
HTTP version
headers
Content-Length
body
response status line
response headers
```

В v1 не нужно:

```text
chunked transfer
multipart
полноценный keep-alive
полный HTTP/1.1
```

---

## 20. WebSocket subset

Поддерживаем учебный subset WebSocket.

Нужно:

```text
HTTP Upgrade
101 Switching Protocols
Sec-WebSocket-Accept
text frames
close frames
ping/pong
mask/unmask клиентских сообщений
```

В v1 не нужно:

```text
fragmented messages
continuation frames
binary frames
compression
extensions
subprotocol negotiation
полный контроль всех close codes
полный контроль UTF-8
```

---

## 21. JSON subset

Пишем учебный JSON parser/writer.

Нужно:

```text
null
true / false
numbers
strings
arrays
objects
базовые escape-последовательности
```

В v1 можно не поддерживать полностью:

```text
\uXXXX
surrogate pairs
streaming parser
arbitrary precision numbers
все edge cases стандарта
сложную диагностику ошибок
```

---

## 22. game_core и game_runtime

Важно не зацементировать архитектуру только под turn-based игры.

Делаем нейтральный верхний уровень:

```cpp
enum class EGameMode {
    TurnBased,
    Realtime
};

class IGameModule {
public:
    virtual ~IGameModule() = default;

    virtual std::string game_type() const = 0;
    virtual EGameMode mode() const = 0;
};
```

Для первой версии реализуем turn-based runtime:

```cpp
class ITurnBasedGame : public IGameModule {
public:
    virtual TJsonValue create_initial_state(
        const std::vector<TPlayerId>& players
    ) const = 0;

    virtual TMoveValidationResult validate_move(
        const TJsonValue& state,
        TPlayerId player,
        const TJsonValue& move
    ) const = 0;

    virtual TJsonValue apply_move(
        const TJsonValue& state,
        TPlayerId player,
        const TJsonValue& move
    ) const = 0;

    virtual TGameResult get_result(
        const TJsonValue& state
    ) const = 0;
};
```

Крестики-нолики реализуются как:

```text
TTicTacToeGame : ITurnBasedGame
```

---

## 23. Роутинг по играм

Роутинг по внешним командам происходит в `gateway_service`.

Пример:

```text
POST /login -> storage.LoginUser
WS game.find_match -> game.FindMatch
WS game.make_move -> game.MakeMove
```

Роутинг по конкретным играм происходит в `game_service`.

Клиент отправляет:

```json
{
  "type": "game.find_match",
  "request_id": "req-1",
  "payload": {
    "game_type": "tic_tac_toe"
  }
}
```

`gateway_service` просто пересылает это в `game_service`.

`game_service` делает:

```text
game_type = tic_tac_toe
берём matchmaking queue для tic_tac_toe
создаём room с game_type = tic_tac_toe
достаём game module из TGameRegistry
```

При ходе `game_type` можно не передавать. Достаточно `game_id`.

```text
game_id -> room
room.game_type -> TGameRegistry
TGameRegistry -> конкретный game module
```

---

## 24. TicTacToe module

Первая игра — крестики-нолики.

Правила:

```text
поле 3x3
два игрока
X ходит первым
нельзя ходить в занятую клетку
нельзя ходить не в свой ход
победа по строке, столбцу или диагонали
ничья, если поле заполнено и победителя нет
```

Пример состояния:

```json
{
  "board": ["x", null, null, null, "o", null, null, null, null],
  "x_player_id": 42,
  "o_player_id": 55,
  "next_turn_user_id": 42
}
```

Пример хода:

```json
{
  "cell": 4
}
```

---

## 25. Возможное расширение до realtime-игр

Realtime-игру с шайбой, прямоугольным полем и плашками можно добавить позже.

Сейчас это делать не надо.

Главное — не назвать все базовые сущности так, будто проект навсегда только turn-based.

Правильная формулировка:

```text
В первой версии реализован runtime для пошаговых игр.
Архитектура допускает добавление realtime runtime поверх того же gateway/storage/RPC слоя.
```

Для realtime позже добавятся:

```text
IRealtimeGame
TRealtimeRoom
tick loop
game.input
state snapshots
частая рассылка состояния по WebSocket
```

---

## 26. Kafka

Kafka не нужна в v1.

Если добавлять позже, то не в критичный путь игры.

Не надо:

```text
ход игрока -> Kafka -> game_service -> Kafka -> ответ игроку
```

Основной путь должен остаться прямым:

```text
gateway_service -> RPC -> game_service
```

Kafka можно добавить для событий:

```text
game_service -> Kafka -> analytics_service
game_service -> Kafka -> async consumers
```

Topics:

```text
game_events
user_events
rating_events
```

Events:

```text
GameStarted
MoveMade
GameFinished
UserRegistered
RatingUpdated
```

Лучший первый вариант Kafka — аналитика, чтобы падение Kafka не ломало игру.

---

## 27. База данных

Минимальная схема:

```sql
CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    rating INTEGER NOT NULL DEFAULT 1000,
    created_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE sessions (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL REFERENCES users(id),
    token_hash TEXT NOT NULL,
    expires_at TIMESTAMP NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now()
);

CREATE TABLE games (
    id BIGSERIAL PRIMARY KEY,
    game_type TEXT NOT NULL,
    status TEXT NOT NULL,
    winner_user_id BIGINT REFERENCES users(id),
    started_at TIMESTAMP NOT NULL DEFAULT now(),
    finished_at TIMESTAMP
);

CREATE TABLE game_players (
    game_id BIGINT NOT NULL REFERENCES games(id),
    user_id BIGINT NOT NULL REFERENCES users(id),
    symbol TEXT NOT NULL,
    rating_before INTEGER NOT NULL,
    rating_after INTEGER,
    PRIMARY KEY (game_id, user_id)
);

CREATE TABLE moves (
    id BIGSERIAL PRIMARY KEY,
    game_id BIGINT NOT NULL REFERENCES games(id),
    user_id BIGINT NOT NULL REFERENCES users(id),
    move_number INTEGER NOT NULL,
    move_json TEXT NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT now()
);
```

---

## 28. Полный сценарий игры

Регистрация:

```text
client
  -> HTTP POST /register
gateway_service
  -> RPC storage.CreateUser
storage_service
  -> INSERT users
PostgreSQL
```

Логин:

```text
client
  -> HTTP POST /login
gateway_service
  -> RPC storage.LoginUser
storage_service
  -> проверяет пароль
  -> создаёт session
  -> возвращает session_token
```

WebSocket connect:

```text
client
  -> GET /ws + Upgrade: websocket
gateway_service
  -> 101 Switching Protocols
  -> соединение становится WebSocket
```

Поиск игры:

```text
client
  -> WS game.find_match
gateway_service
  -> RPC storage.ValidateSession
  -> RPC game.FindMatch
game_service
  -> кладёт игрока в очередь
```

Match found:

```text
game_service
  -> создаёт GameRoom
  -> event game.MatchFound
gateway_service
  -> отправляет WS event обоим клиентам
```

Ход:

```text
client
  -> WS game.make_move
gateway_service
  -> RPC game.MakeMove
game_service
  -> TTicTacToeGame.validate_move
  -> TTicTacToeGame.apply_move
  -> TTicTacToeGame.get_result
  -> event game.StateUpdated
gateway_service
  -> WS event клиентам
```

Завершение игры:

```text
game_service
  -> RPC storage.FinishGame
storage_service
  -> transaction:
       update games
       insert moves
       update ratings
gateway_service
  -> WS game.finished
```

---

## 29. Code style

Фиксируем стиль:

```text
Classes: TPascalCase
Structs: TPascalCase
Type aliases: TPascalCase
Interfaces: IPascalCase
Enums: EPascalCase
Enum values: PascalCase
Functions: snake_case
Methods: snake_case
Variables: snake_case
Private fields: snake_case_
Constants: k_snake_case
Macros: SCREAMING_SNAKE_CASE
Namespaces: snake_case
Files: snake_case.h / snake_case.cpp
```

Примеры:

```cpp
class TEventLoop;
class TTcpServer;
class THttpParser;
class TWebSocketFrame;
class TRpcClient;
class TTicTacToeGame;

struct THttpRequest;
struct THttpResponse;
struct TPlayerId;
struct TGameId;

class IGameModule;
class ITurnBasedGame;

enum class EGameMode {
    TurnBased,
    Realtime
};
```

Методы:

```cpp
void start();
void stop();
void send_response(const THttpResponse& response);
void register_game(std::unique_ptr<IGameModule> game);
```

Поля:

```cpp
class TClientSession {
public:
    void start();
    void send_message(std::string message);

private:
    TEventLoop& loop_;
    int socket_fd_ = -1;
    TBuffer read_buffer_;
    TBuffer write_buffer_;
    std::optional<TUserId> user_id_;
};
```

Форматирование:

```cpp
if (condition) {
    do_something();
} else {
    do_another_thing();
}

for (int i = 0; i < count; ++i) {
    process(i);
}
```

Правила:

```text
4 spaces
без tabs
braces обязательны всегда
opening brace на той же строке
#pragma once
указатели и ссылки около типа: const TUser& user, TSession* session
```

---

## 30. GitHub workflow

Репозиторий:

```text
Game Service
```

GitHub Project:

```text
Game Service Roadmap
```

Статусы:

```text
Backlog
Ready
In Progress
Review
Changes Requested
Done
```

Labels:

```text
area/async
area/net
area/http
area/websocket
area/json
area/rpc
area/game
area/storage
area/db
area/tests
area/docs

type/feature
type/bug
type/refactoring
type/test
type/research

difficulty/easy
difficulty/medium
difficulty/hard
```

Milestones:

```text
M1: TCP + EventLoop
M2: Minimal coroutines
M3: HTTP + JSON
M4: WebSocket
M5: RPC between services
M6: Storage + PostgreSQL
M7: TicTacToe offline logic
M8: Online TicTacToe
M9: Rating + History
M10: Second game
```

Workflow ученика:

```text
1. Берёт задачу из Ready.
2. Переносит в In Progress.
3. Создаёт branch.
4. Пишет код.
5. Открывает Pull Request.
6. Переносит задачу в Review.
7. Исправляет замечания.
8. После merge задача уходит в Done.
```

Branch naming:

```text
heesooyaam/12-http-parser
qwerty52/15-event-loop
<USER_NAME>/22-websocket-close
```

PR description:

```text
Closes #12

What was done:
- ...

How tested:
- ...
```

---

## 31. Remote development

Разработка идёт на удалённой Linux-тачке, не локально.

Схема:

```text
ученик -> SSH -> dev machine
ученик -> tmux / VS Code Remote SSH
код лежит на dev machine
git push идёт с dev machine в GitHub
```

На dev-тачке нужно:

```text
git
cmake
ninja
clang/gcc
gdb
valgrind
docker
docker compose
PostgreSQL client
tmux
clang-format
clang-tidy
sanitizers
```

Желательно создать отдельного Linux-пользователя для ученика.

GitHub остаётся местом для:

```text
issues
project board
pull requests
code review
истории проекта
```

Dev-тачка — место для:

```text
написания кода
сборки
запуска сервисов
тестов
PostgreSQL
docker-compose
tmux-сессий
```

---

## 32. Roadmap разработки

### Этап 1. Инфраструктура репозитория

Задачи:

```text
создать repo
создать GitHub Project
настроить labels
настроить milestones
создать начальный CMake project
добавить структуру папок
добавить README
добавить docs/architecture.md
добавить docs/code_style.md
```

Результат: пустой, но структурированный проект.

---

### Этап 2. Blocking TCP echo

Задачи:

```text
socket wrapper
blocking TCP server
echo response
ручная проверка через nc
```

Результат: ученик понимает базовые сокеты.

---

### Этап 3. Non-blocking sockets + EventLoop

Задачи:

```text
non-blocking mode
epoll
TEventLoop
loop.Post(...)
wakeup fd
single-thread echo server
multi-loop echo server
```

Результат: один поток обслуживает много соединений.

---

### Этап 4. Minimal coroutine runtime

Задачи:

```text
TTask<T>
TTask<void>
TDetachedTask
Spawn(...)
ScheduleOn(event_loop)
SleepFor(...)
гарантия resume в owning event loop
```

Результат: дальше верхнеуровневые обработчики можно писать через co_await, а не через callback hell.

---

### Этап 5. HTTP

Задачи:

```text
THttpRequest
THttpResponse
THttpParser
HTTP response builder
GET /health
POST /echo
coroutine-friendly handlers
```

Результат: curl работает.

---

### Этап 6. JSON

Задачи:

```text
TJsonValue
object/array/string/number/bool/null
JSON parser
JSON writer
parser errors
unit tests
```

Результат: можно принимать и отправлять JSON.

---

### Этап 7. WebSocket

Задачи:

```text
HTTP Upgrade
101 Switching Protocols
Sec-WebSocket-Accept
frame parser
frame writer
mask/unmask
text frames
ping/pong
close
```

Результат: браузер подключается по WebSocket и получает echo.

---

### Этап 8. RPC

Задачи:

```text
length-prefixed codec
RPC request/response
request_id
pending requests
RPC dispatcher
RPC events
timeouts
coroutine API для RPC calls
gateway -> mock service
```

Результат: сервисы общаются через handmade JSON RPC over TCP, а верхнеуровневый код может делать `co_await rpc.call(...)`.

---

### Этап 9. storage_service

Задачи:

```text
подключить PostgreSQL
миграции
users
sessions
register
login
validate session
repositories
DB worker pool
co_await db_pool.Submit(...)
```

Результат: регистрация и логин работают через `gateway_service -> storage_service`.

---

### Этап 10. game_core + tic_tac_toe offline

Задачи:

```text
IGameModule
ITurnBasedGame
TGameRegistry
TTurnBasedRoom
TTicTacToeGame
unit tests на правила
```

Результат: крестики-нолики полностью работают без сети.

---

### Этап 11. game_service online

Задачи:

```text
matchmaking
active rooms
game.FindMatch
game.MakeMove
game.Resign
events to gateway
сохранение игры через storage
```

Результат: два пользователя играют через WebSocket.

---

### Этап 12. Rating + History

Задачи:

```text
storage.FinishGame
транзакция завершения игры
обновление рейтинга
история игр
leaderboard
HTTP endpoints для profile/history/leaderboard
```

Результат: после игры обновляется рейтинг и появляется история.

---

### Этап 13. Вторая turn-based игра

Задачи:

```text
добавить connect_four
зарегистрировать в TGameRegistry
сделать отдельный matchmaking queue по game_type
проверить, что gateway/storage почти не меняются
```

Результат: доказательство расширяемости архитектуры.

---

### Этап 14. HTTPS/WSS через reverse proxy

Задачи:

```text
добавить nginx/caddy/traefik
настроить TLS termination
снаружи открыть HTTPS/WSS
внутри оставить HTTP/WS до gateway_service
описать схему в docs/security.md
```

Результат: публичный клиент ходит по HTTPS/WSS, C++ gateway не реализует TLS руками.

---

### Этап 15. Optional: Kafka

Если останется время:

```text
поднять Kafka в docker-compose
добавить producer в game_service
публиковать GameStarted, MoveMade, GameFinished
добавить простой analytics_service
не ставить Kafka в критичный путь игры
```

Результат: ученик понимает event streaming отдельно от RPC.

---

### Этап 16. Optional: realtime game

Если останется время:

```text
добавить IRealtimeGame
добавить TRealtimeRoom
добавить tick loop
добавить game.input
добавить state snapshots
реализовать простую pong/air-hockey игру
```

Результат: фреймворк поддерживает не только turn-based, но и realtime runtime.

---

## 33. Что не делать в v1

Не делаем:

```text
Kubernetes
Redis
Kafka в критичном пути
TLS руками в C++
полноценный HTTP/1.1
полноценный WebSocket RFC
production-grade JSON
reconnect/resume
spectator mode
чат
турниры
несколько инстансов одного сервиса
балансировку
отдельный сервис на каждую игру
полноценный frontend
```

---

## 34. Главные архитектурные правила

```text
1. Клиент общается только с gateway_service.
2. gateway_service реализует HTTP и WebSocket.
3. gateway_service не знает правил игр и схемы БД.
4. game_service не знает HTTP/WebSocket.
5. game_service отвечает за matchmaking и active rooms.
6. storage_service единственный ходит в PostgreSQL.
7. storage_service отдаёт доменные RPC-методы, а не удалённый SQL.
8. Конкретные игры подключаются через game modules.
9. Одна пользовательская сессия живёт в одном event loop.
10. Одна game room живёт в одном game loop.
11. Blocking операции нельзя выполнять в event loop.
12. Из другого потока нельзя менять объект напрямую, только через loop.Post(...).
13. Корутины живут поверх event loop.
14. После co_await continuation возвращается в owning event loop.
15. В первой версии делаем turn-based runtime, но не называем всю архитектуру turn-only.
16. В v1 HTTP/WS без TLS допустим только для dev/internal окружения.
17. Для публичного запуска нужен HTTPS/WSS через reverse proxy.
```

---

## 35. Краткое описание для README

Game service is a C++ backend platform for online games.

The project focuses on building backend infrastructure from scratch: non-blocking networking, event loops, coroutines, HTTP, WebSocket, JSON, internal RPC, service-to-service communication, game runtime, matchmaking, rating, game history and PostgreSQL storage.

The first implemented game is Tic-Tac-Toe. The architecture is designed so that new game modules can be added without rewriting the gateway, storage layer or transport infrastructure.

The first version implements a turn-based game runtime, while the architecture leaves room for adding realtime games later.
