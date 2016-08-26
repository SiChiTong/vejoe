/*

*/
#include <QtCore/qobject.h>
//定时器类
#include <QObject>
#include <QTimer>
#include <QtWidgets>

#include "hummanfollow.h"

//timer
//Timer_Motion::Timer_Motion(int timer_ids)
//{
//    success_flag = 0;
//    timer_id = timer_ids;
//}

void Timer_Motion::start_timer(void)
{
    if(usr_flag == 0)
    {
        timer_id = startTimer(5000);
        usr_flag = 1;
    }
}

void Timer_Motion::stop_timer(void)
{
    if(usr_flag == 1)
    killTimer(timer_id);
    usr_flag = 0;
    success_flag = 0;
}

void Timer_Motion::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timer_id)
    {
        usr_flag = 0;
        success_flag = 1;
    }
}

void Timer_Motion::clr_flag(void)
{
    killTimer(timer_id);
    usr_flag = 0;
    success_flag = 0;
}

unsigned int Timer_Motion::return_flag(void)
{
    return success_flag;
}

Timer_Motion::~Timer_Motion()
{

}
