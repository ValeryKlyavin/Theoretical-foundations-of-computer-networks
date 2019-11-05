#include "PacketManager.h"
#include <bitset>
#include <QBitArray>
#include <QDebug>
#include <QtMath>
#include <time.h>

bool PacketManager::errorSimulation = false;

PacketManager::PacketManager()
{

}

QByteArray PacketManager::createPacket(char source, char dest, QChar data[7])
{
    QByteArray arr;
    char c = 0x0E;
    arr.append(c);
    arr.append(dest);
    arr.append(source);
    for(int i = 0; i < 7; i++)
        arr.append(data[i]);
    char crc = 0;
    arr.append(crc);
    crc = getCRC(arr);
    arr.chop(1);
    arr.append(crc);
    return arr;
}

QString PacketManager::toBinaryString(QByteArray &arr)
{
    QString bitsStr;
    for(int i=0; i<arr.count(); ++i)
    {
        std::bitset<8> fieldBits(arr.at(i));
        bitsStr += QString::fromStdString(fieldBits.to_string());
    }
    return bitsStr;
}

QString PacketManager::toHexString(QByteArray &arr)
{
    QString str;
    int left, right;
    for(auto i : arr)
    {
        left = (i >> 4) & 0x0F;
        right = i & 0x0F;
        str += QString::number(left, 16);
        str += QString::number(right, 16);
        str += " ";
    }
    return str;
}

QString PacketManager::toString(QByteArray &arr)
{
    QString str;
    for(auto i : arr)
    {
        str.append(i);
    }
    return str;
}

QByteArray PacketManager::processPacketWithBitstuffing(QByteArray &arr)
{
    QByteArray unstuffed(arr);
    unstuffed.remove(0, 1);

    QString bitsStr = toBinaryString(unstuffed);

    bitsStr.replace("0000111", "00001111");

    QBitArray bits(bitsStr.size());

    for(int i = 0; i < bitsStr.size(); i++)
        bits[i] = (bitsStr.at(i) == '0') ? false : true;

    if(errorSimulation)
    {
        qsrand(time(0));
        int random = qrand() % bitsStr.size();
        bits[random] = bits[random] ^ 1;
    }

    QByteArray stuffed;
    for(int b=0; b < bits.count();++b)
        stuffed[b/8] = (stuffed.at(b / 8) | ((bits[b] ? 1 : 0) << (7 - (b % 8))));

    stuffed.insert(0, arr.at(0));
    return stuffed;
}

QByteArray PacketManager::processPacketWithDebitstuffing(QByteArray &arr)
{
    QByteArray frameWithoutFlag(arr);
    frameWithoutFlag.remove(0, 1);

    QString bitsStr = toBinaryString(frameWithoutFlag);

    bitsStr.replace("00001111", "0000111");

    QBitArray bits(bitsStr.size());

    for(int i = 0; i < bitsStr.size(); i++)
        bits[i] = (bitsStr.at(i) == '0') ? false : true;

    QByteArray resultFrame;
    //resultFrame.resize(12);

    for(int b = 0; b < bits.count(); ++b)
        resultFrame[b / 8] = (resultFrame.at(b / 8) | ((bits[b] ? 1 : 0) << (7 - (b % 8))));
    resultFrame.insert(0, arr.at(0));
    resultFrame.resize(11);
    return resultFrame;
}

QString xorWithGenerator(QString a)
{
    QString result = "";
    const QString generator = "10000011"; // 131
    for (int i = 0; i < 8; i++)
        result += (a[i] == generator[i]) ? '0' : '1';
    return result;
}

char PacketManager::getCRC(QByteArray &arr)
{
    QString str = toBinaryString(arr);
    while(str[0] == '0')
        str.remove(0,1);
    QString nextOctet;
    while (str.length() > 8)
    {
        nextOctet = str.mid(0, 8);
        str.remove(0,8);
        str = xorWithGenerator(nextOctet) + str;
        while(str.at(0) == '0')
            str.remove(0,1);
    }
    return (str.toInt(nullptr, 2) < 131 ? str : xorWithGenerator(str)).toInt(nullptr, 2);
}

bool PacketManager::checkForErrors(QByteArray &arr)
{
    return getCRC(arr) != 0;
}

void PacketManager::correctError(QByteArray &arr)
{
    char c;
    auto length = arr.length();
    for (int i = 0; i < length; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            c = 1 << j;
            arr[i] = arr[i] ^ c;
            if (!checkForErrors(arr))
                return;
            arr[i] = arr[i] ^ c;
        }
    }
}
