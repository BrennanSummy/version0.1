#include "UpdateTimer.h"

UpdateTimer::UpdateTimer(QWidget* parent, int period):QWidget(parent)
{
    timer = new QTimer();
    timer->setInterval(period);
    timer->setSingleShot(false);

    connect(timer,&QTimer::timeout, this, &UpdateTimer::tick);
}

void UpdateTimer::toggle()
{
    if(timer->isActive())
    {
        timer->stop();
    }
    else
    {
        timer->start();
    }
}
void UpdateTimer::tick()
{
    emit timeIsUp();
}

void UpdateTimer::changeInterval(float newFramerate)
{
    int multiplier = 60/newFramerate;
    float basePeriod = 1000/60;
    //millisecondPeriod = 1000 / newFramerate;
    millisecondPeriod = basePeriod*multiplier;
    timer->setInterval(millisecondPeriod);
}