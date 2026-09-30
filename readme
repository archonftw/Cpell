# Cpell

**A Unix shell built from scratch in C++.**

Cpell is a Linux shell project focused on understanding how shells interact with processes, terminals, signals, file descriptors, and the kernel.

> Built for learning Unix internals through implementation rather than abstraction.

---

## Features

|   | Feature                    |
| - | -------------------------- |
| ✓ | Execute external commands  |
| ✓ | Built-in commands          |
| ✓ | Environment variables      |
| ✓ | Input / output redirection |
| ✓ | Background processes       |
| ✓ | Job control                |
| ✓ | Process groups             |
| ✓ | Signal handling            |
| ✓ | Command history            |
| ✓ | Tab autocompletion         |

### Built-ins

```text
cd
export
unset
env
jobs
bg
fg
help
exit
```

### Redirection

```bash
echo "hello" > file.txt
echo "world" >> file.txt
cat < file.txt
```

### Job Control

```bash
sleep 100 &
jobs
fg 1
bg 1
```

Cpell uses Unix process groups and terminal control to manage foreground and background jobs.

---

## Architecture

```text
                    Cpell
                      │
              ┌───────▼───────┐
              │    Readline   │
              └───────┬───────┘
                      │
              ┌───────▼───────┐
              │     Parser    │
              └───────┬───────┘
                      │
              ┌───────▼───────┐
              │ Variable Exp. │
              └───────┬───────┘
                      │
                ┌─────▼─────┐
                │  Builtin? │
                └──┬─────┬──┘
                   │     │
                  Yes     No
                   │     │
                   │   fork()
                   │     │
                   │   setpgid()
                   │     │
                   │   execvp()
                   │     │
                   │   waitpid()
                   │
                   ▼
                Execute
```

### Job Control

Cpell manages interactive jobs using:

* `fork()`
* `execvp()`
* `setpgid()`
* `tcsetpgrp()`
* `waitpid()`
* Unix signals

This allows foreground processes to receive terminal input and signals while the shell maintains control of background jobs.

---

## Build

### Requirements

* Linux
* C++17
* GNU Make
* GNU Readline

Ubuntu:

```bash
sudo apt update
sudo apt install g++ make libreadline-dev
```

### Compile

```bash
git clone https://github.com/archonftw/Cpell.git
cd Cpell
make
```

Run:

```bash
./cpell
```

Or:

```bash
make run
```

Clean:

```bash
make clean
```

---

## Project Structure

```text
Cpell/
├── Commands/
├── Execute/
├── Parsing/
├── utils/
├── main.cpp
├── Makefile
└── README.md
```

---

## Roadmap

* [ ] Pipelines
* [ ] Improved parser
* [ ] Quoted strings
* [ ] Escape characters
* [ ] Proper `SIGCHLD` handling
* [ ] Job completion notifications
* [ ] More built-ins
* [ ] Automated tests
* [ ] Better POSIX compatibility

---

## Why?

Cpell started as a way to learn what actually happens when you type:

```bash
ls
```

instead of treating the shell as a black box.

The project explores the path from:

```text
Command
   ↓
Shell
   ↓
fork()
   ↓
Process Group
   ↓
execvp()
   ↓
Linux Kernel
   ↓
Program
```

and everything that happens around it.

---

## License

MIT License
