#pragma once

#include <gpiod.h>
#include <string>

class LedController {
public:
    explicit LedController(int gpioLine = 17, const std::string &chipName = "/dev/gpiochip0");
    ~LedController();

    bool isValid() const { return valid_; }
    void set(bool on);
    void toggle();
    bool state() const { return state_; }

private:
    struct gpiod_chip *chip_ = nullptr;
    struct gpiod_line *line_ = nullptr;
    bool valid_ = false;
    bool state_ = false;
};
