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
    millisecondPeriod = 1000 / newFramerate;
    timer->setInterval(millisecondPeriod);
}