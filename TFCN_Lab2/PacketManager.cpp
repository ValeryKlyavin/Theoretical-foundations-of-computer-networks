#include "PacketManager.h"
#include <bitset>
#include <QBitArray>

bool PacketManager::fcb = false;

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
    arr.append(fcb);
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

    for(int b = 0; b < bits.count(); ++b)
        resultFrame[b / 8] = (resultFrame.at(b / 8) | ((bits[b] ? 1 : 0) << (7 - (b % 8))));

    resultFrame.insert(0, arr.at(0));
    resultFrame.resize(11);
    return resultFrame;
}
