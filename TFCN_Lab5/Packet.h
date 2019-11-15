#ifndef PACKET_H
#define PACKET_H
#include <QString>

class Packet
{
    char type;
    char source;
    char destination;
    char accessControl;
    QString data;
public:
    inline char getType() { return type; }
    inline void setType(char ty) { type = ty; }

    inline char getSourceAddress() { return  source; }
    inline void setSourceAddress(char addr) { source = addr; }

    inline char getDestinationAddress() { return destination; }
    inline void setDestinationaddress(char addr) { destination = addr; }

    inline void setAccessControl(char c) { accessControl = c; }
    inline char getAccessControl() { return accessControl; }

    inline void setData(QString c) { data = c; }
    inline QString getData() { return data; }
    Packet() {}
    ~Packet() {}
};

#endif // PACKET_H
