# Signal-based Integer Transfer

This project provides two small C utilities for transferring 32-bit signed integers between two Unix-like processes using POSIX user signals:

- `sender`: sends an integer bit-by-bit to a receiver process and waits for per-bit acknowledgements.
- `receiver`: rebuilds the integer from incoming signals and acknowledges each bit back to the sender.

## Building

1. Ensure a POSIX-compatible environment with a C compiler (e.g., `gcc`) and standard development tools.
2. Run `make` to build both binaries:

```sh
make
```

This produces the `sender` and `receiver` executables in the repository root.

## Running

1. Start the receiver first. It prints its process ID so the sender can target it:

```sh
./receiver
```

2. From another terminal, launch the sender with the receiver PID and the integer to transmit:

```sh
./sender <receiver_pid> <integer>
```

The sender transmits the integer bitwise using `SIGUSR1` (bit 0) and `SIGUSR2` (bit 1). The receiver acknowledges each bit; once 32 bits are reconstructed, it prints the received value and continues waiting for more messages.

## Minimal file set for sharing

To reproduce the project on another machine, include at least these files:

- `sender.c`
- `receiver.c`
- `Makefile`
- `README.md` (recommended so others know how to build/run)

With the Makefile and source files present, running `make` should rebuild the binaries on another POSIX system with a compatible C toolchain. Sharing only the two `.c` files is possible, but the recipient would need to compile manually (e.g., `gcc sender.c -o sender` and `gcc receiver.c -o receiver`) and might miss usage details; including the Makefile and README avoids that friction.

## Notes

- The programs rely solely on the standard C library and POSIX signals—no external dependencies are required.
- Both sender and receiver remain running until terminated (e.g., with `Ctrl+C`). The receiver continuously accepts additional integers.
