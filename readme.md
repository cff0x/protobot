# ProtoBot - modular and extensible Discord bot

**This bot is WIP!**

It works, kind of. But it may eat your cat, destroy the universe or do other unexpected things. Do not expect anything
to work yet. Use it as a reference for learning or whatever, it's up to you.

I just uploaded this code because I like sharing and there isn't much public code related to D++ bots, so it may
even help you, idk. I took some inspiration from the official DPP documentations.

The bot will be kept open-source but I will not publish all my modules.

**Do not reach out to me for support, yet. When the time comes, I will let you know here.**

### Requirements
- CMake
- [D++ Discord Library](https://github.com/brainboxdotcc/DPP)
- [spdlog](https://github.com/gabime/spdlog)
- [sqlpp11](https://github.com/rbock/sqlpp11)
- [CLI11](https://github.com/CLIUtils/CLI11) (included in /thirdparty/CLI11)
- A PostgreSQL database (other backends can (and will) be added)
- Python (for event-related code generation - see `generate_events` custom CMake target)

This bot currently only works on Unix-based systems. Windows support is planned.  More info on how to build, use and developing modules will follow.

Disclaimer: *Some of the code may look shitty or be shitty, so keep in mind: **this is just a WIP**. Have fun!*

For a simple build, just use CMake (provided you have all requirements met):
```
cmake -B ./build
cmake --build build
```

This will build the `libprotobot` shared library, the bot application and all modules inside `/modules/`.