#pragma once

#include <QObject>
#include <QVector>
#include <QBasicTimer>
#include <QProcess>
#include <QMutex>

struct PingData
{
    QString name;
    QString ip;
    QString lastStatus;
};

class PingThread;

class Pinger : public QObject
{
    Q_OBJECT

public:
    explicit Pinger(QObject* parent = nullptr);
    ~Pinger() override;

    void addHost(const QString& host, const QString& address);
    void start();

    PingThread* startPingThread(int idx);

signals:
    void pingComplete(const QString& ip, const QString& state);

private:
    int findHost(const QString& ip);

    void onPingComplete(const QString& ip, const QString& state);
    void stopAll();

private:
    QVector<PingData> _hosts;
    QVector<PingThread*> _threadPool;
};
