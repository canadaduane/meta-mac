#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <getopt.h>
#include <linux/input.h>

#define DEFAULT_PROGNAME "swap_super_alt"
#define OPTSTR ":lrh"
#define USAGE_FMT  "%s -- swaps SUPER and ALT modifer keys\n"\
    "\n    -l: swap left super with left alt (only)"\
    "\n    -r: swap right super with right alt (only)\n"

typedef struct {
  int           swap_left;
  int           swap_right;
} options_t;

typedef struct input_event event_t;

void write_event(event_t* event) {
    fwrite(event, sizeof(event_t), 1, stdout);
}

void event_loop(int swap_left, int swap_right) {
    // Don't buffer stdin or stdout--we want instant reactions
    setbuf(stdin, NULL), setbuf(stdout, NULL);

    event_t shift_event;
    event_t meta_pressed_event;

    event_t event;
    while (fread(&event, sizeof(event), 1, stdin) == 1) {
        // Skip EV_MSC
        if (event.type == EV_MSC && event.code == MSC_SCAN) continue;

        // Pass non-key events through
        if (event.type != EV_KEY) {
            write_event(&event);
            continue;
        }

        if (swap_left && event.code == KEY_LEFTALT) {
            event.code = KEY_LEFTMETA;
            write_event(&event);
        } else if (swap_left && event.code == KEY_LEFTMETA) {
            event.code = KEY_LEFTALT;
            write_event(&event);
        } else if (swap_right && event.code == KEY_RIGHTALT) {
            event.code = KEY_RIGHTMETA;
            write_event(&event);
        } else if (swap_right && event.code == KEY_RIGHTMETA) {
            event.code = KEY_RIGHTALT;
            write_event(&event);
        } else {
            write_event(&event);
        }
    }
}

void usage(char *progname, int opt) {
    fprintf(stderr, USAGE_FMT, progname ? progname : DEFAULT_PROGNAME);
    exit(EXIT_FAILURE);   
}

int main(int argc, char* argv[]) {
    int opt;
    options_t options = {
        .swap_left = 1,
        .swap_right = 1
    };

    while ((opt = getopt(argc, argv, OPTSTR)) != EOF)
        switch(opt) {
            case 'l':
                options.swap_left = 1;
                options.swap_right = 0;
                break;
            case 'r':
                options.swap_left = 0;
                options.swap_right = 1;
                break;
            case 'h':
            default:
                usage(basename(argv[0]), opt);
            break;
       }

    event_loop(options.swap_left, options.swap_right);
}
