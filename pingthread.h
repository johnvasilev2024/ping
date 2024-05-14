#pragma once

#include <QObject>
#include <QThread>

class PingThread : public QThread
{
    Q_OBJECT

public:
    explicit PingThread(QObject* parent = nullptr);

    void run() override;

    Q_INVOKABLE void do_ping(const QString& host);
    Q_INVOKABLE void stop_thread();

signals:
    void ping_done(const QString& host, const QString& result);
    void thread_finished();

private:
    QString _host;
    bool _stopRequested = false;
};
