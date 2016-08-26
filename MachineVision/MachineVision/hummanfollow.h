#ifndef HUMMANFOLLOW_H
#define HUMMANFOLLOW_H

//---------------------------------//
#include <QtCore/qobject.h>
#include <QObject>
//定时器类
#include <QTime>
#include <QTimer>
#include <QtWidgets>

class Timer_Motion : public QObject
{
public:
    int timer_id;
    unsigned int success_flag;
    unsigned int usr_flag;

    void start_timer(void);
    void stop_timer(void);
    void clr_flag(void);
    unsigned int return_flag(void);

    ~Timer_Motion();

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // HUMMANFOLLOW_H
