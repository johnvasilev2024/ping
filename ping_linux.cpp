#include "ping_base.h"

#if defined(Q_OS_LINUX)

#include <QElapsedTimer>
#include <QDebug>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <cstring>

int ping(const char *ip_addr) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);

    struct icmphdr icmp_hdr;
    memset(&icmp_hdr, 0, sizeof(icmp_hdr));
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.un.echo.id = getpid();
    icmp_hdr.un.echo.sequence = 1;

    QElapsedTimer timer;
    timer.start();

    ssize_t sent = sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (sent < 0) {
        perror("sendto");
        close(sockfd);
        return -1;
    }

    char buf[1024];
    socklen_t addr_len = sizeof(addr);
    ssize_t received = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &addr_len);
    if (received < 0) {
        perror("recvfrom");
        close(sockfd);
        return -1;
    }

    qint64 elapsed_time = timer.nsecsElapsed() / 1000000; // Convert nanoseconds to milliseconds
    qDebug() << "Ping time:" << elapsed_time << "ms";

    close(sockfd);
    return elapsed_time;
}

#endif
