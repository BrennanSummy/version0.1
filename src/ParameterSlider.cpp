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

    // Configure slider (-100 to 100 for -1.0 to 1.0)
    value = 0;
    slider->setRange(-100, 100);
    slider->setSingleStep(1);
    slider->setValue(value);

    // Connect slider to value display
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
    float floatValue = inValue / 100.0f;
    value = floatValue;
    valueLabel->setText(QString::number(floatValue, 'f', 2));
    // Send out a Qt signal
    emit sliderHasChanged(value);
}

void ParameterSlider::setValue(float inValue) {
    int sliderValue = static_cast<int>(inValue * 100.0f);
    slider->setValue(sliderValue);
    onSliderValueChanged(sliderValue);
}
