# LUA

- Tested with following versions of Zephyr RTOS:
  - v2.7.2
  - v3.1.0

- And following versions of LUA
  - 5.4.4

## Steps:

- ~~Link it with the application (and turn LUA into a zephyr module)~~
- ~~Run a simple script~~
- Check how to interact with the os and add support for it
  - Missing `_link`, `_unlink`, `_times`
  - `FS`, `fopen` (not supported according to this table https://docs.zephyrproject.org/latest/services/portability/posix.html#posix-device-io), so what does the actual `fopen` in my code.
- Add support for application API
- Find a way to dynamically load files from a sort of filesystem
- Concurrent scripts execution
- Compile LUA scripts into bytecode and run them (luac ?)

Ideas:
- Maybe disable some unecessary modules, like "file" module ?
  - See `luaL_openlibs()` and `loadedlibs`
- Check how to pass arguments to lua scripts, and to get values
- Check how to allocate heap/stack for a lua script

## Notes

- `lua_checkstack`

## Expected result

```
-- west build: running target run
[0/1] To exit from QEMU enter: 'CTRL+a, x'[QEMU] CPU: qemu32,+nx,+pae
SeaBIOS (version zephyr-v1.0.0-0-g31d4e0e-dirty-20200714_234759-fv-az50-zephyr)
Booting from ROM..*** Booting Zephyr OS build zephyr-v2.7.2  ***
[00:00:00.000,000] <inf> app_fs: Area 1 at 0x1000 on FLASH_SIMULATOR for 65536 bytes
[00:00:00.000,000] <inf> littlefs: LittleFS version 2.2, disk version 2.0
[00:00:00.000,000] <inf> littlefs: FS at FLASH_SIMULATOR:0x1000 is 64 0x400-byte blocks with 512 cycle
[00:00:00.000,000] <inf> littlefs: sizes: rd 16 ; pr 16 ; ca 64 ; la 32
[00:00:00.000,000] <err> littlefs: WEST_TOPDIR/modules/fs/littlefs/lfs.c:997: Corrupted dir pair at {0x0, 0x1}
[00:00:00.010,000] <wrn> littlefs: can't mount (LFS -84); formatting
[00:00:00.010,000] <inf> littlefs: /lfs mounted
[00:00:00.010,000] <inf> app_fs: /lfs mount: 0
[00:00:00.010,000] <inf> main: Adding file helloworld.lua to FS, at 0x26dfc0 [ 48 B ]
[00:00:00.010,000] <inf> main: Script /lfs/helloworld.lua written -> 48
[00:00:00.010,000] <inf> main: Adding file math.lua to FS, at 0x26dff0 [ 29664 B ]
[00:00:00.020,000] <inf> main: Script /lfs/math.lua written -> 29664
[00:00:00.020,000] <inf> main: Adding file script1.lua to FS, at 0x2753d0 [ 32 B ]
[00:00:00.020,000] <inf> main: Script /lfs/script1.lua written -> 32
[00:00:00.020,000] <inf> app_fs: /lfs: bsize = 16 ; frsize = 1024 ; blocks = 64 ; bfree = 32
[00:00:00.020,000] <inf> app_fs: Listing dir /lfs ...
[00:00:00.020,000] <inf> app_fs: [FILE] helloworld.lua (size = 48)
[00:00:00.020,000] <inf> app_fs: [FILE] math.lua (size = 29664)
[00:00:00.020,000] <inf> app_fs: [FILE] script1.lua (size = 32)
Hello from LUA running on Zephyr RTOS !
[00:00:00.040,000] <inf> main: Executed script rc: 0
1
2
3
[00:00:00.040,000] <inf> main: Executed script rc: 0
testing numbers and math lib
64-bit integers, 53-bit (mantissa) floats
testing order (floats cannot represent all integers)
testing -0 and NaN
testing 'math.random'
random seeds: 0, 7315996
float random range in 37100 calls: [0.000032, 0.999981]
integer random range in 64000 calls: [minint + 12ppm, maxint - 25ppm]
OK
[00:00:00.930,000] <inf> main: Executed script rc: 0
[00:00:00.940,000] <err> fs: file open error (-2)
[00:00:00.940,000] <inf> main: Executed script rc: 6
[00:00:00.940,000] <inf> main: App done, rc = 6
```
