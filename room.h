#ifndef ROOM_H
#define ROOM_H

#include <vector>
#include <string>
#include <memory>
#include "jsonparser.h"
using namespace std;

enum class RoomType
{
    Normal, Task
};

enum class TileType
{
    Empty, Wall, EmptyOrWall, Portal, Task
};

class Room
{
public:
    Room(string id, int height, int width, vector<string> portals);
    virtual RoomType getType() const = 0;
    virtual void generate();
    const string& getId() const { return id; }
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    const vector<string>& getPortals() { return portals; }
    TileType getGridAt(int y, int x) const { return grid[y][x]; }  // 获取指定位置的格子类型
    string getPortalAt(int y, int x) const { return gridPortal[y][x]; }  // 获取指定位置的传送门IDz

protected:
    string id;
    int height, width;
    vector<string> portals;
    vector<vector<TileType>> grid;
    vector<vector<string>> gridPortal;
};

class NormalRoom: public Room
{
public:
    NormalRoom(string id, int height, int width, vector<string> portals);
    RoomType getType() const { return RoomType::Normal; }
};

class TaskRoom: public Room
{
public:
    TaskRoom(string id, int height, int width, vector<string> portals, int requiredExp, vector<string> tasks,int isTreasure,int isKeys);
    RoomType getType() const { return RoomType::Task; }
    void generate();
    string getTaskAt(int y, int x) const { return gridTask[y][x]; }
    void removeTaskAt(int y, int x);
    int getRequiredExp() const { return requiredExp; }
    void setRequiredExp(int value) { requiredExp = value; }
    void setIsTreasure(int value) { isTreasure = value; }
    int getIsTreasure() const { return isTreasure; }
    void setIsKeys(int value){ isKeys = value; }
    int getIsKeys() const{ return isKeys; }

protected:
    int requiredExp;
    vector<string> tasks; // 任务的id
    vector<vector<string>> gridTask;
    int isTreasure;//如果没拣到就是0
    int isKeys;
};

using RoomPtr = shared_ptr<Room>;

RoomPtr makeRoom(const JsonObject& roomObject);

#endif // ROOM_H
