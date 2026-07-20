#pragma once

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

class ParameterSlider : public QWidget {
    Q_OBJECT
public:
    explicit ParameterSlider(const QString& name, QWidget* parent = nullptr); 
    ~ParameterSlider();
    void setValue(float value);
    void onSliderValueChanged(int value);
    float value;

signals:
    void sliderHasChanged(float newValue);

private:
    QLabel* nameLabel;
    QSlider* slider;
    QLabel* valueLabel;
    QVBoxLayout* layout;
};
