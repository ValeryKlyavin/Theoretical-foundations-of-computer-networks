#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <QObject>
#include <QByteArray>


class PacketManager : public QObject
{
    Q_OBJECT
    static bool fcb;
    PacketManager();
public:
    static QByteArray createPacket(char source, char dest, QChar data[7]);
    static QString toBinaryString(QByteArray& arr);
    static QString toHexString(QByteArray& arr);
    static QString toString(QByteArray& arr);
    static QByteArray processPacketWithBitstuffing(QByteArray& arr);
    static QByteArray processPacketWithDebitstuffing(QByteArray& arr);
    inline static void setFCB(bool value) { fcb = value; }
};

#endif // PACKETMANAGER_H
