#pragma once

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>
#include <string>

class ParameterSlider : public QWidget {
    Q_OBJECT
public:
    //explicit ParameterSlider(const QString& name, QWidget* parent = nullptr); 
    explicit ParameterSlider(   const QString& name,
                                float min,
                                float max,
                                float defaultVal,
                                int steps,
                                QWidget* parent = nullptr); 
    ~ParameterSlider();
    void     setValue(float value);
    void     onSliderValueChanged(int value);
    void     emitDefaultValue();
    float    sliderStepToValue(int sliderStep);
    int      sliderFloatValueToStep(float inFloatValue);
    float    value;

signals:
    void sliderHasChanged(float newValue);

private:
    float           minimum;
    float           maximum;
    float           defaultValue;
    int             steps;
    QLabel*         nameLabel;
    QSlider*        slider;
    QLabel*         valueLabel;
    QVBoxLayout*    layout;
};
