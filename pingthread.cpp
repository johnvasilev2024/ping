#include "pingthread.h"

#if defined(Q_OS_WIN32)
#include "ping_win32.h"
#elif defined(Q_OS_LINUX)
#include "ping_linux.h"
#endif

#include <QDebug>

PingThread::PingThread(QObject* parent)
    : QThread(parent)
{
}

void PingThread::run()
{
    while (!_stopRequested)
    {
        if (!_host.isEmpty())
        {
            int res = ping_host(_host);
            QString time;
            if (res == -1)
                time = tr("Нет связи");
            else
                time = tr("%1 мс").arg(res);

            emit ping_done(_host, time);
        }

        // Make sleep breakable
        for (int i = 0; i < 100; i++)
        {
            if (_stopRequested)
                break;
            msleep(15);
        }
    }
    emit thread_finished();
}

void PingThread::do_ping(const QString& host)
{
    _host = host;
}

void PingThread::stop_thread()
{
    _stopRequested = true;
}
