# R-TYPE Rewrite in C++

This project is a rewrite of the classic arcade game, R-TYPE, in C++ with added multiplayer functionality via server-client communication.

## Features
- Multiplayer mode, with support for multiple players to connect and play together
- Server-client communication, allowing players to play together in real-time
- Cross-platform compatibility, with support for Windows, Linux
- Complete Entity Component System (ECS)

## Installation

You can download the latest release [here](https://github.com/Thibb1/B-CPP-500-rtype/releases/latest)

## Compile from source

### Requirements
- A C++17 compiler
- cmake for Linux or Visual Studio 22 for Windows

Clone the repository and compile the source code using the following commands:
### Linux
```
git clone git@github.com:EpitechPromo2025/B-CPP-500-LIL-5-2-rtype-romain.leemans.git
cd B-CPP-500-LIL-5-2-rtype-romain.leemans
./linux_build
```
### Windows
```
git clone git@github.com:EpitechPromo2025/B-CPP-500-LIL-5-2-rtype-romain.leemans.git
cd B-CPP-500-LIL-5-2-rtype-romain.leemans
./windows_build
```
## Play the game
`./bin/client` to play the game as a client

`./bin/server` to launch the server **(REQUIRED TO PLAY)**

## Network Protocol

Feel free to check how the server work [right here](server/Protocol.md)
