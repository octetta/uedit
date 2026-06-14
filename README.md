# uedit

[![CodeFactor](https://www.codefactor.io/repository/github/octetta/uedit/badge)](https://www.codefactor.io/repository/github/octetta/uedit)

A single-header C library for interactive line editing in the terminal.
Drop `uedit.h` into your project and get cursor movement, insert/delete,
multi-line history, and standard REPL shortcuts with no dependencies.

## Features

- Multi-line history: stores the last 100 entries by default in a circular buffer.
- Standard navigation: move cursor left/right, jump to start/end (ctrl-A / ctrl-E).
- Advanced editing: clear line (ctrl-U), kill-to-end (ctrl-K), clear screen (ctrl-L).
- Configurable: adjust history depth at runtime.
- Cross-platform: Native support Linux, macOS, and Windows.

*this is an intentionally low-feature option that scratches my own itch*
If you need more features or better written code, I **highly** recommend
one of these two projects:

- [`linenoise`](https://github.com/antirez/linenoise)
- [`bestline`](https://github.com/jart/bestline)

## Usage

```c
#include "uedit.h"

// optional: change history size (default is 100)
uedit_config_history(250);
char buf[1024];
int r = uedit("> ", buf, sizeof(buf));
```

`uedit` blocks until the user presses Enter or ctrl-D.

**Returns** the number of characters in `buf` on Enter (`>= 0`), `-1` on ctrl-D / EOF.

## Keybindings

| Key | Action |
|-----|--------|
| Left / Right | Move cursor |
| Home / End | Jump to start / end of line |
| ctrl-A / ctrl-E | Jump to start / end of line |
| Up / Down | Cycle through command history |
| Backspace / Delete | Delete character |
| ctrl-U | Clear entire line |
| ctrl-K | Kill text from cursor to end of line |
| ctrl-L | Clear screen and redraw prompt |
| ctrl-D | Delete character at cursor, or EOF if line is empty |
| Enter | Confirm |

## Building

No build step required — just include the header. Compile your program as usual:

```sh
gcc -o myapp myapp.c
```

## Notes

- History management: History is a circular buffer. It is shared across all `uedit()` calls in the same program.
- Buffer safety: `max_line` should be the full size of your buffer. At most `max_line - 1` characters will be stored.
- Character support: Only printable ASCII (32–126) is accepted. Multibyte / Unicode input is not supported.
