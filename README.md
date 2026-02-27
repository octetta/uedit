# uedit

A single-header C library for interactive line editing in the terminal.
Drop `uedit.h` into your project and get cursor movement, insert/delete,
and basic history with no dependencies.

## Features

- Move cursor left/right, jump to start/end (Ctrl-A / Ctrl-E)
- Insert and delete characters anywhere in the line
- Recall the last entered line with the Up arrow
- Works on Linux, macOS, and Windows

*this is an intentionally low-feature option that scratches my own itch*
If you need more features or better written code, I **highly** recommend
one of these two projects:

- [`linenoise`](https://github.com/antirez/linenoise)
- [`bestline`](https://github.com/jart/bestline)

## Usage

```c
#include "uedit.h"

char buf[256];
int r = uedit("> ", buf, sizeof(buf));
```

`uedit` blocks until the user presses Enter or Ctrl-D.

**Returns** the number of characters in `buf` on Enter (`>= 0`), `-1` on Ctrl-D / EOF.

## Keybindings

| Key | Action |
|-----|--------|
| Left / Right | Move cursor |
| Home / End, Ctrl-A / Ctrl-E | Jump to start / end |
| Backspace / Delete | Delete character |
| Up arrow | Recall last line |
| Enter | Confirm |
| Ctrl-D | Delete character at cursor, or EOF if line is empty |

## Building

No build step required — just include the header. Compile your program as usual:

```sh
gcc -o myapp myapp.c
```

## Notes

- History is a single slot (the last non-empty line). It is shared across all `uedit()` calls in the same program.
- `max_line` should be the full size of your buffer. At most `max_line - 1` characters will be stored.
- Only printable ASCII (32–126) is accepted. Multibyte / Unicode input is not supported.
