#include "pinger.h"
#include "pingthread.h"

#if defined(Q_OS_WIN32)
#include "ping_win32.h"
#elif defined(Q_OS_LINUX)
#include "ping_linux.h"
#endif

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QThread>

#include <algorithm>

Pinger::Pinger(QObject* parent)
    : QObject(parent)
{
}

Pinger::~Pinger()
{
    stopAll();
}

void Pinger::addHost(const QString& host, const QString& address)
{
    if (host.isEmpty() || address.isEmpty())
        return;

    PingData d;
    d.name = host;
    d.ip = address;
    _hosts.append(d);
}

void Pinger::start()
{
    for (int i = 0; i < _hosts.size(); i++)
    {
        auto thread = startPingThread(i);
        _threadPool.append(thread);
    }
}

void Pinger::stopAll()
{
    for (int i = 0; i < _threadPool.size(); i++)
    {
        QMetaObject::invokeMethod(_threadPool[i], "stop_thread");
        _threadPool[i]->wait();
    }
}

PingThread* Pinger::startPingThread(int idx)
{
    PingThread* thread = new PingThread(this);

    connect(thread, &PingThread::ping_done, this, &Pinger::onPingComplete, Qt::QueuedConnection);
    connect(thread, &PingThread::thread_finished, thread, &PingThread::deleteLater);

    QMetaObject::invokeMethod(thread, "do_ping", Qt::QueuedConnection, Q_ARG(QString, _hosts[idx].ip));
    thread->start(QThread::LowestPriority);

    return thread;
}

int Pinger::findHost(const QString& ip)
{
    for (int i = 0; i < _hosts.size(); i++)
    {
        if (_hosts[i].ip == ip)
            return i;
    }
    return -1;
}

void Pinger::onPingComplete(const QString& ip, const QString& state)
{
    emit pingComplete(ip, state);
}
