#include <stdio.h>
#include "uedit.h"

int main(void) {
    char buf[256];

    printf("Simple uedit example. Type 'quit' to exit.\n\n");

    while (1) {
        int r = uedit("> ", buf, sizeof(buf));

        if (r == -1) {
            /* Ctrl-D */
            printf("(EOF)\n");
            break;
        }

        if (buf[0] == '\0') {
            /* Empty line — just re-prompt */
            continue;
        }

        if (strcmp(buf, "quit") == 0)
            break;

        printf("You said: %s\n", buf);
    }

    printf("Goodbye.\n");
    return 0;
}
