# Подключение к dev-тачке и клонирование репозитория

Этот гайд нужен, чтобы подключиться к удалённой dev-тачке `game-service-dev` и подтянуть репозиторий проекта `turn-forge`.

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

!! Важно !!

```text
game-service-dev      — приватный ключ, его никому не отправлять
game-service-dev.pub  — публичный ключ, его можно отправлять
```

Публичный ключ вывести так:

```bash
cat ~/.ssh/game-service-dev.pub
```

Или открыть в каком-нибудь текстовом редакторе.

Скопируй всю строку целиком. Она начинается примерно так:

```text
ssh-ed25519 AAAA...
```

Эту строку нужно отправить владельцу dev-тачки (хесояму). Он добавит ключ на сервер.

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

## 5. Создать ключ для работы с репозиторием уже на тачке

Как в прошлый раз создаем ключ:

```bash
ssh-keygen -t ed25519 -f ~/.ssh/github-turn-forge -C "game-service-dev-{USER_NAME}"
```

Заходим в [настройки репозитория](https://github.com/heesooyaam/turn-forge/settings/keys)
и нажимаем **Add deploy key**, вставляем свой публичный ключ и не забываем тыкнуть **Allow write access**

P.S.: если не получится это сделать, надо попрочить владельца репы

После этого на тачке надо создать, если еще нет, файл ~/.ssh/config
и в него дописать такой конфиг:

```sshconfig
Host github.com
    HostName ssh.github.com
    Port 443
    User git
    IdentityFile ~/.ssh/github-turn-forge
    IdentitiesOnly yes
```

## 6. Создать папку под проекты

На dev-тачке выполни:

```bash
mkdir -p ~/projects
cd ~/projects
```

## 6. Подтянуть репозиторий `turn-forge`

Репозиторий проекта называется:

```text
turn-forge
```

Клонировать его нужно так:

```bash
git clone git@github.com:heesooyaam/turn-forge.git
```

Потом перейти в папку проекта:

```bash
cd turn-forge
git status
```

Если репозиторий склонировался и `git status` работает, значит всё ок.

## 10. Важно про ключи

Никогда никому не отправляй приватные ключи:

```text
~/.ssh/game-service-dev
~/.ssh/github-turn-forge
```

Отправлять можно только публичные ключи:

```text
~/.ssh/game-service-dev.pub
~/.ssh/github-turn-forge.pub
```
