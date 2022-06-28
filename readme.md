# Zephyr RTOS and QEMU emulator

- [AWS specific readme](./aws.md)

## Installation

1. Import this project using west

```
west init -m https://github.com/lucasdietrich/zephyr-qemu-dev --mr main my-workspace
```

2. Update Zephyr RTOS version in `west.yml`

- Set `revision` to `main`, `2.7.2` or `3.0.0` for example
- Run `west update` to synchronize git repositories

3. Install net-tools (https://github.com/zephyrproject-rtos/net-tools) if using network

## Build, run, debug

Using west:
- For QEMU x86: `west build -b qemu_x86`
- For QEMU ARM Cortex M3: `west build -b qemu_cortex_m3`

Run:
- `west build -t run`

## VS Code

This project is well integrated into VS Code. Simply install `actboy168.tasks` extension which expose tasks.

Open tasks panel with `Ctrl + Maj + B`.

## Debug (VS Code)

Run the debug server 
- `ninja debugserver -C build -v`
- Select debug configuration among `(gdb) QEMU x86` or `(gdb) QEMU ARM`
- Press `F5`

## Networking (if needed)

NAT/masquerading on host to access Internet :
```
sudo iptables -t nat -A POSTROUTING -j MASQUERADE -s 192.0.2.1
sudo sysctl -w net.ipv4.ip_forward=1
```

In order to run/debug the networking application :
- Run `../net-tools/loop-socat.sh`
- Run `sudo ../net-tools/loop-slip-tap.sh`

And keep the consoles open over sessions

Run or debug the application normally

## Illustration

![zephyr_qemu_networking_debug.png](./pics/zephyr_qemu_networking_debug.png)

## Troubleshooting

- If previous ninja process still listenning to the serial port 1234, find it with `sudo netstat -anpe | grep "1234" | grep "LISTEN"` or `sudo lsof -i :1234` and kill it.

- `preLaunchTask` is not working in `launch.json`

- Don't forget to activate python `venv` if using `west`

- Clean `build` folder if strange error appear

- If closing `loop-slip-tap.sh` by error, kill processes locking the fd using `sudo lsof | grep slip` and relaunch

- Make sure to change manifest path to `zephyr-qemu-dev` in workspace `.west/config` file.

## Ressources

- [Zephyr CMake Package - Zephyr Build Configuration CMake package](https://docs.zephyrproject.org/latest/guides/zephyr_cmake_package.html#cmake-build-config-package)
- [Application Development](https://docs.zephyrproject.org/latest/application/index.html)
- [Zephyr Example Application](https://github.com/zephyrproject-rtos/example-application)
- [Build and Configuration Systems - Build System (CMake)](https://docs.zephyrproject.org/latest/guides/build/index.html#cmake-details)
- [VS Code integration #21119](https://github.com/zephyrproject-rtos/zephyr/issues/21119)
- [West (Zephyr’s meta-tool)](https://docs.zephyrproject.org/latest/guides/west/index.html)
- [West Manifests](https://docs.zephyrproject.org/latest/guides/west/manifest.html)
- [Moving to West](https://docs.zephyrproject.org/latest/guides/west/moving-to-west.html)
- [Workspaces](https://docs.zephyrproject.org/latest/guides/west/workspaces.html)
  - [T2: Star topology, application is the manifest repository](https://docs.zephyrproject.org/latest/guides/west/workspaces.html#west-t2)
- [Zephyr RTOS Development in Linux](https://github.com/bus710/zephyr-rtos-development-in-linux)
- [All Configuration Options](https://docs.zephyrproject.org/2.6.0/reference/kconfig/index-all.html)
- [VS Code - Variables Reference](https://code.visualstudio.com/docs/editor/variables-reference)
- [QEMU wiki](https://wiki.qemu.org/Main_Page)
- [GDB: The GNU Project Debugger](https://www.sourceware.org/gdb/)
- [VS Code - Debugging](https://code.visualstudio.com/docs/editor/debugging)
- [NAT/masquerading on host to access Internet](https://docs.zephyrproject.org/latest/guides/networking/qemu_setup.html#id11)
