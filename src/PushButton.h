#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <iostream>

class PushButton: public QWidget
{
    Q_OBJECT

    public:
        explicit PushButton(
            const QString &name,
            QWidget* parent = nullptr
        );
        ~PushButton();
        void onPress();

    signals:
        void buttonPressed();
    private:
        QLabel* nameLabel;
        QPushButton* button;
        QVBoxLayout* layout;
};