# xidle-daemon

A small X11 daemon that runs a command after the system has been idle for a specified amount of time.

It uses `XScreenSaverQueryInfo` to query the current idle time, so it doesn't rely on any external tools.

I was having issues with `xautolock`, so I began looking through its source code to understand how it detected idle time. After discovering `XScreenSaverQueryInfo`, I realized the core functionality I needed was simple enough to implement in a couple hundred lines of C++ so I wrote this instead.

This isn't intended to replace mature projects like `xautolock`; it's just a minimal daemon that does exactly what I need.

- Lightweight (single C++ source file)
- Intended to run as a `systemd --user` service
- Executes a command once after the idle timeout
- X11 only

## Building

Requirements:

- X11 development headers
- XScreenSaver development headers

Compile with:

```bash
g++ -O2 -std=c++17 main.cpp -o xidle-daemon -lX11 -lXss
```

## Usage

Edit the command in the source to whatever you want to run after the idle timeout, then compile.

Example:

```cpp
std::system("/path/to/script");
```

Install the binary somewhere in your `PATH` (for example `~/.local/bin`).

Example `systemd --user` service:

```ini
[Unit]
Description=X11 idle daemon
After=graphical-session.target

[Service]
Type=simple
ExecStart=%h/.local/bin/xidle-daemon
Restart=always
RestartSec=2

[Install]
WantedBy=default.target
```

Enable it with:

```bash
systemctl --user enable --now xidle-daemon.service
```

## Notes

- The daemon checks the idle time every few seconds, so the command may execute up to one polling interval after the configured timeout.
