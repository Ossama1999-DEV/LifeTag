#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QScreen>

#include "ledController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("LED GPIO 17 - Qt");

    // Layout vertical plein écran
    auto *layout = new QVBoxLayout(&window);
    layout->setContentsMargins(0, 0, 0, 0);  // pas de marge autour
    layout->setSpacing(10);                  // un petit espace entre label et bouton

    auto *statusLabel = new QLabel("LED: OFF");
    auto *button = new QPushButton("Allumer la LED");

    // Police bien grosse
    QFont labelFont = statusLabel->font();
    labelFont.setPointSize(28);         // adapte si trop petit/grand
    statusLabel->setFont(labelFont);
    statusLabel->setAlignment(Qt::AlignCenter);

    QFont buttonFont = button->font();
    buttonFont.setPointSize(32);        // encore plus gros pour le bouton
    button->setFont(buttonFont);

    // Le bouton doit prendre un max de place
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(statusLabel);
    layout->addWidget(button);

    LedController led(17);
    if (!led.isValid()) {
        statusLabel->setText("Erreur: GPIO non dispo");
        button->setEnabled(false);
    }

    QObject::connect(button, &QPushButton::clicked, [&]() {
        led.toggle();
        if (led.state()) {
            statusLabel->setText("LED: ON");
            button->setText("Éteindre la LED");
        } else {
            statusLabel->setText("LED: OFF");
            button->setText("Allumer la LED");
        }
    });

    // 👉 Plein écran sur l’écran courant
    window.showFullScreen();

    return app.exec();
}