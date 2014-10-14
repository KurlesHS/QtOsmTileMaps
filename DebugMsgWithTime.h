#ifndef DEBUGMSGWITHTIME_H
#define DEBUGMSGWITHTIME_H

#include <QTime>

class DebugMsgWithTime {
public:
    DebugMsgWithTime(QString debugMsg) : m_debugMsg(debugMsg) {
        t.start();
    }
    ~DebugMsgWithTime() {
        qDebug(qPrintable(QString("%0, elapsed %1 ms").arg(m_debugMsg).arg(t.elapsed())));
    }

private:
    QString m_debugMsg;
    QTime t;
};

#endif // DEBUGMSGWITHTIME_H
