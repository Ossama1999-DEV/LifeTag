#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE 17   // GPIO 17

int main() {
    struct gpiod_chip *chip;
    struct gpiod_line *line;

    chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        perror("Erreur ouverture chip GPIO");
        return 1;
    }

    line = gpiod_chip_get_line(chip, GPIO_LINE);
    if (!line) {
        perror("Erreur ligne GPIO");
        gpiod_chip_close(chip);
        return 1;
    }

    if (gpiod_line_request_output(line, "blink_led", 0) < 0) {
        perror("Erreur configuration GPIO");
        gpiod_chip_close(chip);
        return 1;
    }

    printf("Blink LED sur GPIO %d (Ctrl + C pour arrêter)\n", GPIO_LINE);

    while (1) {
        gpiod_line_set_value(line, 1); // ON
        usleep(500000);                // 0.5 sec

        gpiod_line_set_value(line, 0); // OFF
        usleep(500000);                // 0.5 sec
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);

    return 0;
}
