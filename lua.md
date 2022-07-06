# LUA

## Steps:

- ~~Link it with the application (and turn LUA into a zephyr module)~~
- ~~Run a simple script~~
- Check how to interact with the os and add support for it
  - Missing `_link`, `_unlink`, `_times`
  - `FS`
- Add support for application API
- Find a way to dynamically load files from a sort of filesystem
- Concurrent scripts execution
- Compile LUA scripts into bytecode and run them (luac ?)

## Notes

- `lua_checkstack`

## Expected result

```
[0/1] To exit from QEMU enter: 'CTRL+a, x'[QEMU] CPU: cortex-m3
qemu-system-arm: warning: nic stellaris_enet.0 has no peer
Timer with period zero, disabling
*** Booting Zephyr OS build zephyr-v2.7.2  ***
LUA starting ...
Lua 5.4.4  Copyright (C) 1994-2022 Lua.org, PUC-Rio
Hello from LUA running on Zephyr RTOS !
LUA done ... : 0
```
