# Подключение к dev-тачке и клонирование репозитория

Этот гайд нужен, чтобы подключиться к удалённой dev-тачке `game-service-dev` и подтянуть репозиторий проекта `game-service`.

## 1. Сгенерировать SSH-ключ на своём компьютере

На своём ноутбуке или ПК открой терминал и выполни:

```bash
ssh-keygen -t ed25519 -f ~/.ssh/game-service-dev -C "game-service-dev"
```

Когда спросит `passphrase`, можно нажать `Enter`, либо задать пароль на ключ.

После этого появятся два файла:

```text
~/.ssh/game-service-dev
~/.ssh/game-service-dev.pub
```

Важно:

```text
game-service-dev      — приватный ключ, его никому не отправлять
game-service-dev.pub  — публичный ключ, его можно отправлять
```

Публичный ключ вывести так:

```bash
cat ~/.ssh/game-service-dev.pub
```

Скопируй всю строку целиком. Она начинается примерно так:

```text
ssh-ed25519 AAAA...
```

Эту строку нужно отправить владельцу dev-тачки. Он добавит ключ на сервер.

## 2. Дождаться добавления ключа

После добавления ключа тебе должны сказать:

```text
username на dev-тачке
IP-адрес dev-тачки
```

Например:

```text
username: USER_NAME
host: 158.160.6.108
```

## 3. Настроить SSH config

На своём компьютере открой файл:

```bash
nano ~/.ssh/config
```

Добавь туда:

```sshconfig
Host game-service-dev
    HostName 158.160.6.108
    User USER_NAME
    IdentityFile ~/.ssh/game-service-dev
    IdentitiesOnly yes
```

Вместо `USER_NAME` подставь имя пользователя, которое тебе скажут.

Выдай нужные права:

```bash
chmod 700 ~/.ssh
chmod 600 ~/.ssh/game-service-dev
chmod 600 ~/.ssh/config
```

## 4. Подключиться к dev-тачке

Теперь можно подключаться короткой командой:

```bash
ssh game-service-dev
```

После входа проверь, что ты на сервере:

```bash
whoami
hostname
pwd
```

Каждый раз для входа на dev-тачку с ноута будешь писать:

```bash
ssh game-service-dev
```

## 5. Получить доступ к GitHub-репозиторию

Для работы с репозиторием нужен свой GitHub-аккаунт.

Владелец репозитория должен добавить твой GitHub-аккаунт в collaborators:

```text
Repository -> Settings -> Collaborators -> Add people
```

Права должны быть:

```text
Write
```

Не нужно выдавать `Admin` или `Maintain`.

Важно: для разработки мы не используем deploy key с write access.

Deploy key — это ключ репозитория или сервера, а не конкретного человека. Для нормальной работы через Pull Request нужно, чтобы GitHub понимал, какой именно пользователь пушит изменения.

## 6. Создать SSH-ключ для GitHub уже на dev-тачке

После входа на dev-тачку создай отдельный ключ для GitHub:

```bash
ssh-keygen -t ed25519 -f ~/.ssh/github-game-service -C "game-service-dev-USER_NAME"
```

Вместо `USER_NAME` можно подставить свой username на dev-тачке или GitHub username.

После этого появятся два файла:

```text
~/.ssh/github-game-service
~/.ssh/github-game-service.pub
```

Публичный ключ вывести так:

```bash
cat ~/.ssh/github-game-service.pub
```

Скопируй всю строку целиком.

Теперь зайди в свой GitHub-аккаунт:

```text
GitHub -> Settings -> SSH and GPG keys -> New SSH key
```

Вставь туда содержимое файла:

```text
~/.ssh/github-game-service.pub
```

Важно:

```text
~/.ssh/github-game-service      — приватный ключ, никому не отправлять
~/.ssh/github-game-service.pub  — публичный ключ, его можно добавлять в GitHub
```

## 7. Настроить SSH config для GitHub на dev-тачке

На dev-тачке создай или открой файл:

```bash
nano ~/.ssh/config
```

Добавь туда:

```sshconfig
Host github.com
    HostName ssh.github.com
    Port 443
    User git
    IdentityFile ~/.ssh/github-game-service
    IdentitiesOnly yes
```

Выдай права:

```bash
chmod 700 ~/.ssh
chmod 600 ~/.ssh/github-game-service
chmod 600 ~/.ssh/config
```

Проверь, что GitHub видит тебя как правильного пользователя:

```bash
ssh -T git@github.com
```

Должно быть что-то такое:

```text
Hi YOUR_GITHUB_USERNAME! You've successfully authenticated, but GitHub does not provide shell access.
```

Если там написан чужой username, значит используется не тот ключ.

## 8. Создать папку под проекты

На dev-тачке выполни:

```bash
mkdir -p ~/projects
cd ~/projects
```

## 9. Подтянуть репозиторий `game-service`

Репозиторий проекта называется:

```text
game-service
```

Клонировать его нужно так:

```bash
git clone git@github.com:heesooyaam/game-service.git
```

Потом перейти в папку проекта:

```bash
cd game-service
git status
```

Если репозиторий склонировался и `git status` работает, значит всё ок.

## 10. Проверить Git user

Git user нужен для подписи коммитов. Это не то же самое, что GitHub-аккаунт для push, но его тоже нужно настроить.

На dev-тачке внутри репозитория выполни:

```bash
git config user.name "YOUR_NAME"
git config user.email "YOUR_EMAIL"
```

Например:

```bash
git config user.name "qwerty1337"
git config user.email "qwerty1337@example.com"
```

Проверить:

```bash
git config user.name
git config user.email
```

## 11. Как правильно работать с ветками

Основная ветка проекта называется:

```text
trunk
```

В неё напрямую не пушим.

Перед началом задачи:

```bash
git switch trunk
git pull
git switch -c USER_NAME/ISSUE_NUMBER-short-task-name
```

Например:

```bash
git switch -c qwerty1337/12-http-parser
```

После изменений:

```bash
git status
git add .
git commit -m "Add HTTP parser"
git push -u origin qwerty1337/12-http-parser
```

Потом нужно открыть Pull Request на GitHub:

```text
qwerty1337/12-http-parser -> trunk
```

В `trunk` изменения попадают только через Pull Request и review.

## 12. Немного про тачку

```text
vCPU: 16
RAM: 16 ГБ
SSD: 200 ГБ
```

Такая тачка стоила бы 21'141 руб. / мес., что слишком дорого.

Чтобы тачка стоила дешевле, можно сделать её "прерываемой" — такой, которая работает не более 24 часов и может быть остановлена Compute Cloud в любой момент.

После остановки она не удаляется, все её данные сохраняются, а чтобы продолжить работу, нужно запустить её повторно.

На такие виртуалки большая скидка, и с теми же характеристиками она стоит примерно 8k в месяц.

Так что если тачка выключится — не бомби, просто перезапусти её. Если это будет происходить слишком часто, обсудим, что делать.

## 13. Ещё раз про ключи

Никогда никому не отправляй приватные ключи:

```text
~/.ssh/game-service-dev
~/.ssh/github-game-service
```

Отправлять и добавлять куда-либо можно только публичные ключи:

```text
~/.ssh/game-service-dev.pub
~/.ssh/github-game-service.pub
```

Важно:

```text
game-service-dev.pub
```

отправляется владельцу dev-тачки, чтобы он дал доступ к серверу.

```text
github-game-service.pub
```

добавляется в твой личный GitHub-аккаунт, чтобы ты мог работать с репозиторием от своего имени.
