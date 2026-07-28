#include "ParameterSlider.h"
#include <QVBoxLayout>
#include <QSlider>

ParameterSlider::ParameterSlider(const QString& name, QWidget* parent)
    : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    nameLabel = new QLabel(name);
    slider = new QSlider(Qt::Horizontal);
    valueLabel = new QLabel("initial value");

    // Configure slider (1 to 100 for 0.1 to 10)
    value = 1;
    slider->setRange(1, 100);
    slider->setSingleStep(1);
    slider->setValue(value);

    // Connect the default and custom slider functions
    connect(slider, &QSlider::valueChanged, this, &ParameterSlider::onSliderValueChanged);

    // Add widgets to layout
    layout->addWidget(nameLabel);
    layout->addWidget(slider);
    layout->addWidget(valueLabel);
}
ParameterSlider::~ParameterSlider() {
    delete slider;
    delete valueLabel;
    delete layout; 
}

void ParameterSlider::onSliderValueChanged(int inValue) {
    // Convert from slider steps to float value.
    float floatValue = inValue / 10.0f;
    value = floatValue;
    valueLabel->setText(QString::number(floatValue, 'f', 2));
    // Send out a Qt signal
    emit sliderHasChanged(value);
}

void ParameterSlider::setValue(float inValue) {
    int sliderValue = static_cast<int>(inValue);
    slider->setValue(sliderValue);
    onSliderValueChanged(sliderValue);
}
