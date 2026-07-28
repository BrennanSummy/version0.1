#include "PushButton.h"

// QWidget is the base class, and we must initialize a base QWidget object accordingly.
PushButton::PushButton(
            const QString &name,
            QWidget* parent):
            QWidget(parent)
{
    // Initialize
    layout      = new QVBoxLayout(this);
    nameLabel   = new QLabel(name);
    button      = new QPushButton();

    // Connect the default and custom button functions
    connect(button, &QPushButton::clicked, this, &PushButton::onPress);

    // Add to layout
    layout->addWidget(nameLabel);
    layout->addWidget(button);
}

PushButton::~PushButton()
{
    delete button;
    delete nameLabel;
    delete layout;
}
void PushButton::onPress()
{
    //std::cout << "button pressed" <<std::endl;
    emit buttonPressed();
}