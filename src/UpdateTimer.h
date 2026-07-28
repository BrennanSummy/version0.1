#pragma once
#include <QWidget>
#include <QTimer>

class UpdateTimer: public QWidget
{
    Q_OBJECT

    public:
        UpdateTimer(QWidget* parent = nullptr, int period=1000);
        void changeInterval(float newFramerate);
    public slots:
        void toggle();
        
    signals:
        void timeIsUp();
    
    private:
        QTimer* timer;
        int     millisecondPeriod;

        void tick();
};