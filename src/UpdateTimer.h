#pragma once
#include <QWidget>
#include <QTimer>

class UpdateTimer: public QWidget
{
    Q_OBJECT

    public:
        UpdateTimer(QWidget* parent = nullptr, int period=1000);
    public slots:
        void toggle();
        
    signals:
        void timeIsUp();
    
    private:
        QTimer* timer;

        void tick();
};