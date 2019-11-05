#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <QObject>
#include <QByteArray>


class PacketManager : public QObject
{
    Q_OBJECT
    static bool errorSimulation;
    PacketManager();
public:
    static QByteArray createPacket(char source, char dest, QChar data[7]);
    static QString toBinaryString(QByteArray& arr);
    static QString toHexString(QByteArray& arr);
    static QString toString(QByteArray& arr);
    static QByteArray processPacketWithBitstuffing(QByteArray& arr);
    static QByteArray processPacketWithDebitstuffing(QByteArray& arr);
    static char getCRC(QByteArray& arr);
    static bool checkForErrors(QByteArray& arr);
    static void correctError(QByteArray& arr);

    inline static void setErrorSimulation(bool value) { errorSimulation = value; }
};

#endif // PACKETMANAGER_H
