/* MIT License / Copyright 2017 Francisco Lopes da Silva */

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <linux/input.h>

// clang-format off
const struct input_event
syn       = {.type = EV_SYN , .code = SYN_REPORT   , .value = 0},
esc_up    = {.type = EV_KEY , .code = KEY_ESC      , .value = 0},
ctrl_up   = {.type = EV_KEY , .code = KEY_LEFTCTRL , .value = 0},
esc_down  = {.type = EV_KEY , .code = KEY_ESC      , .value = 1},
ctrl_down = {.type = EV_KEY , .code = KEY_LEFTCTRL , .value = 1};
// clang-format on

void print_usage(FILE *stream, const char *program) {
    // clang-format off
    fprintf(stream,
            "caps2esc - transforming the most useless key ever in the most useful one\n"
            "\n"
            "usage: %s [-h | [-m mode] [-t delay]]\n"
            "\n"
            "options:\n"
            "    -h         show this message and exit\n"
            "    -t         delay used for key sequences (default: 20000 microseconds)\n"
            "    -m mode    0: default\n"
            "                  - caps as esc/ctrl\n"
            "                  - esc as caps\n"
            "               1: minimal\n"
            "                  - caps as esc/ctrl\n"
            "               2: useful on 60%% layouts\n"
            "                  - caps as esc/ctrl\n"
            "                  - esc as grave accent\n"
            "                  - grave accent as caps\n",
            program);
    // clang-format on
}

int read_event(struct input_event *event) {
    return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

void write_event(const struct input_event *event) {
    if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
        exit(EXIT_FAILURE);
}

void write_event_with_mode(struct input_event *event, int mode) {
    if (event->type == EV_KEY)
        switch (mode) {
            case 0:
                if (event->code == KEY_ESC)
                    event->code = KEY_CAPSLOCK;
                break;
            case 2:
                switch (event->code) {
                    case KEY_ESC:
                        event->code = KEY_GRAVE;
                        break;
                    case KEY_GRAVE:
                        event->code = KEY_CAPSLOCK;
                        break;
                }
                break;
        }
    write_event(event);
}

int main(int argc, char *argv[]) {
    int mode = 0, delay = 20000;

    for (int opt; (opt = getopt(argc, argv, "ht:m:")) != -1;) {
        switch (opt) {
            case 'h':
                return print_usage(stdout, argv[0]), EXIT_SUCCESS;
            case 'm':
                mode = atoi(optarg);
                continue;
            case 't':
                delay = atoi(optarg);
                continue;
        }

        return print_usage(stderr, argv[0]), EXIT_FAILURE;
    }

    struct input_event input;
    enum { START, CAPSLOCK_HELD, CAPSLOCK_IS_CTRL } state = START;

    setbuf(stdin, NULL), setbuf(stdout, NULL);

    while (read_event(&input)) {
        if (input.type == EV_MSC && input.code == MSC_SCAN)
            continue;

        if (input.type != EV_KEY && input.type != EV_REL &&
            input.type != EV_ABS) {
            write_event(&input);
            continue;
        }

        switch (state) {
            case START:
                if (input.type == EV_KEY && input.code == KEY_CAPSLOCK &&
                    input.value)
                    state = CAPSLOCK_HELD;
                else
                    write_event_with_mode(&input, mode);
                break;
            case CAPSLOCK_HELD:
                if (input.type == EV_KEY && input.code == KEY_CAPSLOCK) {
                    if (input.value == 0) {
                        write_event(&esc_down);
                        write_event(&syn);
                        usleep(delay);
                        write_event(&esc_up);
                        state = START;
                    }
                } else if ((input.type == EV_KEY && input.value == 1) ||
                           input.type == EV_REL || input.type == EV_ABS) {
                    write_event(&ctrl_down);
                    write_event(&syn);
                    usleep(delay);
                    write_event_with_mode(&input, mode);
                    state = CAPSLOCK_IS_CTRL;
                } else
                    write_event_with_mode(&input, mode);
                break;
            case CAPSLOCK_IS_CTRL:
                if (input.type == EV_KEY && input.code == KEY_CAPSLOCK) {
                    input.code = KEY_LEFTCTRL;
                    write_event(&input);
                    if (input.value == 0)
                        state = START;
                } else
                    write_event_with_mode(&input, mode);
                break;
        }
    }
}
