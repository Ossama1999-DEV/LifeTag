#include "ledController.h"
#include <stdexcept>
#include <iostream>

LedController::LedController(int gpioLine, const std::string &chipName) {
    chip_ = gpiod_chip_open(chipName.c_str());
    if (!chip_) {
        std::cerr << "Erreur: impossible d'ouvrir " << chipName << std::endl;
        return;
    }

    line_ = gpiod_chip_get_line(chip_, gpioLine);
    if (!line_) {
        std::cerr << "Erreur: impossible d'obtenir la ligne GPIO " << gpioLine << std::endl;
        gpiod_chip_close(chip_);
        chip_ = nullptr;
        return;
    }

    if (gpiod_line_request_output(line_, "qt_led_gui", 0) < 0) {
        std::cerr << "Erreur: impossible de configurer GPIO en sortie" << std::endl;
        gpiod_chip_close(chip_);
        chip_ = nullptr;
        line_ = nullptr;
        return;
    }

    valid_ = true;
    state_ = false;
}

LedController::~LedController() {
    if (line_) {
        gpiod_line_set_value(line_, 0); // éteindre proprement
        gpiod_line_release(line_);
    }
    if (chip_) {
        gpiod_chip_close(chip_);
    }
}

void LedController::set(bool on) {
    if (!valid_) return;
    if (gpiod_line_set_value(line_, on ? 1 : 0) < 0) {
        std::cerr << "Erreur: gpiod_line_set_value" << std::endl;
        return;
    }
    state_ = on;
}

void LedController::toggle() {
    set(!state_);
}
