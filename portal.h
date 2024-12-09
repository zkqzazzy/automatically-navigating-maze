#ifndef PORTAL_H
#define PORTAL_H

#include <string>
#include <memory>
#include "jsonparser.h"
using namespace std;

class Portal
{
public:
    Portal(string id, string destination);
    const string& getId() const { return id; }
    const string& getDestination() const { return destination; }
    const string& getRoom() const { return roomId; }
    void setRoom(const string& value) { roomId = value; }
    int getX() const { return x; }
    void setX(int value) { x = value; }
    int getY() const { return y; }
    void setY(int value) { y = value; }
    bool isBidirectional() const { return bidirectional; }
    void setBidirectional(bool value) { bidirectional = value; }

protected:
    string id;
    string destination; // 到达的传送门id
    string roomId; // 所属的房间的id
    int x, y; // 坐标
    bool bidirectional; // 是否是双向的
};

using PortalPtr = shared_ptr<Portal>;

PortalPtr makePortal(const JsonObject& portalObject);

#endif // PORTAL_H
