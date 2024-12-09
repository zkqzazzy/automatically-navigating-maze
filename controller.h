#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <qevent.h>
#include <qpixmap.h>
#include <queue>
#include <string>
#include "jsonparser.h"
#include "room.h"
#include "portal.h"
#include "task.h"
#include "player.h"
using namespace std;

class Controller
{
public:
    Controller() {};
    ~Controller() {};
    void newGame(const std::string& configPath);
    void render(QPixmap& canvas);   // 渲染游戏画面到QPixmap
    void handleKeyPress(QKeyEvent *e);  // 处理键盘按键
    void handleMousePress(QMouseEvent *e);    // 处理鼠标按下
    bool handleTickEvent();  // 处理循环，更新状态
    float getFps() const { return 30.; }    // 获取帧率
    int getTileSize() const { return 16; }    // 获取格子大小
    // void onTimeout(int remainingTime);
    string getRoomId();
    bool isTaskRoom();
    bool getTreasure();
    bool getKeys();

protected:
    JsonObject config;
    string start, end; // 开始/结束传送门的id
    unordered_map<string, RoomPtr> rooms; // 房间id -> 房间对象
    unordered_map<string, PortalPtr> portals; // 传送门id -> 传送门对象
    unordered_map<string, TaskPtr> tasks; //
    Player player;

private:
    void playerMoveTo(int x, int y);

    queue<pair<int, int>> pendingMoves;  //待处理的移动队列
    float lastTickTime;  //上一次游戏循环的时间
};

#endif // CONTROLLER_H
