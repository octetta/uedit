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
- Event-loop friendly: optionally wake on a POSIX file descriptor or Windows
  wait handle while editing a line.

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

## API

```c
typedef void (*uedit_event_cb)(void *user);

int uedit(const char *prompt, char *buf, int max_line);

int uedit_with_event(const char *prompt,
                     char *buf,
                     int max_line,
                     int event_fd,
                     void *event_handle,
                     uedit_event_cb on_event,
                     void *event_user);
```

`uedit_with_event` is the same editor with one additional wake source:

- On Linux/macOS, pass a selectable `event_fd`, or `-1` if unused.
- On Windows, pass a waitable `HANDLE` as `event_handle`, or `NULL` if unused.
- When the event source is ready, `on_event(event_user)` runs and the current
  input line is redrawn. The callback should service the event source, for
  example by draining a pipe, socket, or application event queue.
- `uedit(prompt, buf, max_line)` is a wrapper around `uedit_with_event` with no
  event source.

The event callback runs on the same thread that called `uedit_with_event`; no
background thread is created.

## POSIX select Example

This example uses a self-pipe as a wake source. A real application could use a
socket, timer fd, control-event fd, or any descriptor that can be used with
`select()`.

```c
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "uedit.h"

static void drain_event(void *user) {
    int fd = *(int *)user;
    char buf[64];
    while (read(fd, buf, sizeof(buf)) > 0) {
    }
    printf("\n[event serviced]\n");
}

int main(void) {
    int pipefd[2];
    char line[256];

    if (pipe(pipefd) != 0) return 1;

    fcntl(pipefd[0], F_SETFL, fcntl(pipefd[0], F_GETFL, 0) | O_NONBLOCK);
    fcntl(pipefd[1], F_SETFL, fcntl(pipefd[1], F_GETFL, 0) | O_NONBLOCK);

    /*
     * Demo only: queue one event before entering the editor. In a real program
     * another subsystem would write to pipefd[1] when work is ready.
     */
    write(pipefd[1], "x", 1);

    while (uedit_with_event("> ", line, sizeof(line),
                            pipefd[0], NULL, drain_event, &pipefd[0]) >= 0) {
        if (strcmp(line, "quit") == 0) break;
        printf("command: %s\n", line);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}
```

On Windows, pass a waitable object such as a manual-reset event created with
`CreateEvent` as `event_handle`. The callback should service the work and reset
the event when appropriate.

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
