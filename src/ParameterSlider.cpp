#include "ParameterSlider.h"

//ParameterSlider::ParameterSlider(const QString& name, QWidget* parent)
ParameterSlider::ParameterSlider(   const QString& name,
                                    float min,
                                    float max,
                                    float defaultVal,
                                    int steps,
                                    QWidget* parent)
    : minimum(min), maximum(max), defaultValue(defaultVal), steps(steps), QWidget(parent)
{
    layout    = new QVBoxLayout(this);
    nameLabel = new QLabel(name);
    slider    = new QSlider(Qt::Horizontal);
    valueLabel= new QLabel(QString::number(defaultValue,'f',3));


    // Connect the default and custom slider functions
    connect(slider, &QSlider::valueChanged, this, &ParameterSlider::onSliderValueChanged);

    // Configure slider (0 to steps)
    slider->setRange(0, steps);
    slider->setSingleStep(1);
    setValue(defaultValue);

    // Add widgets to layout
    layout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(nameLabel);
    layout->addWidget(slider);
    layout->addWidget(valueLabel);
}
ParameterSlider::~ParameterSlider() {
    delete slider;
    delete valueLabel;
    delete layout; 
}


void ParameterSlider::emitDefaultValue()
{
    setValue(defaultValue);
}

void ParameterSlider::onSliderValueChanged(int inValue) {
    // Convert from slider steps to float value.
    float floatValue = sliderStepToValue(inValue);

    valueLabel->setText(QString::number(floatValue, 'f', 3));
    // Send out a Qt signal
    emit sliderHasChanged(floatValue);
}

void ParameterSlider::setValue(float inFloatValue) {
    int sliderValue = sliderFloatValueToStep(inFloatValue);
    slider->setValue(sliderValue);
    onSliderValueChanged(sliderValue);
}

float ParameterSlider::sliderStepToValue(int sliderStep)
{
    float value = minimum + (((float) sliderStep)/steps)*(maximum - minimum);
    return value;
}

int ParameterSlider::sliderFloatValueToStep(float inFloatValue)
{
    int intValue = (inFloatValue - minimum) * (steps / (maximum - minimum));
    return intValue;
}