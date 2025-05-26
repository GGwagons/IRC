# IRC Server

A simple IRC (Internet Relay Chat) server implemented in C++. Supports multiple clients, channels, core IRC commands, and includes a bonus IRC bot.

## Features

- **Multi-client support** using `epoll` for efficient I/O
- **Channel management:** create, join, leave, set topic, invite-only, keys, user limits
- **User authentication:** `NICK`, `USER`, `PASS`
- **Standard IRC commands:** `JOIN`, `PART`, `PRIVMSG`, `QUIT`, `KICK`, `INVITE`, `MODE`, `TOPIC`, `PING`, etc.
- **Numeric reply codes** for protocol compliance
- **Bonus:** IRC bot with fun commands (roll dice, random facts, etc.)

## Project Structure

```
.
├── include/        # Header files
├── src/            # Source files
│   ├── commands/   # Command implementations
│   ├── utils/      # Utility functions
│   └── bonus/      # Bonus bot implementation
├── Makefile        # Build instructions
├── README.md       # This file
└── ft_irc.pdf      # Project specification
```

## Build Instructions

To build the server:

```sh
make
```

To build the bonus IRC bot:

```sh
make bonus
```

## Usage

### Start the IRC Server

```sh
./ircserv <port> <password>
```

- `<port>`: Port number to listen on (1024-65535)
- `<password>`: Password required for clients to connect

### Start the IRC Bot (Bonus)

```sh
./ircbot <server-port> <server-password>
```

- The bot connects to the server and joins a channel named after itself.

## Supported IRC Commands

- `PASS <password>`
- `NICK <nickname>`
- `USER <username> * 0 :<realname>`
- `JOIN <channel>[,<channel>...] [<key>[,<key>...]]`
- `PART <channel>[,<channel>...] [<reason>]`
- `PRIVMSG <target> :<message>`
- `QUIT [:<message>]`
- `KICK <channel> <user>[,<user>...] [<reason>]`
- `INVITE <nickname> <channel>`
- `MODE <target> [<modes> [<mode params>...]]`
- `TOPIC <channel> [<topic>]`
- `PING <token>`

## Notes

- Only local channels (starting with `&`) are supported.
- The server uses `epoll` for scalability.
- The IRC bot responds to commands like `roll`, `fact`, and `cat` in its channel.

## License

This project is for educational purposes.