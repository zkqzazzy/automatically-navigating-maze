#include <cassert>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <iostream>
#include "room.h"
#include "utils.h"
using namespace std;

Room::Room(string id, int height, int width, vector<string> portals):
    id(std::move(id)), // 移动语义，避免调用复制构造函数
    height(height),
    width(width),
    portals(std::move(portals))
{
    assert(height % 2 == 1 && "必须是奇数");
    assert(width % 2 == 1 && "必须是奇数");
    // generate();
}

void Room::generate()
{
    grid.assign(height, vector<TileType>(width));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                grid[y][x] = TileType::Wall;
            } else if(x % 2 == 1 && y % 2 == 1) {
                grid[y][x] = TileType::Empty;
            } else if(x % 2 == 1 || y % 2 == 1) {
                grid[y][x] = TileType::EmptyOrWall;
            } else {
                grid[y][x] = TileType::Wall;
            }
        }
    }
    gridPortal.assign(height, vector<string>(width));

    vector<pair<int, int>> edges; // 候选边
    for(int i = 0; i < portals.size(); i++) {
        while(true) {
            int y = randomInt(0, height / 2) * 2 + 1;//确保结果是奇数
            int x = randomInt(0, width / 2) * 2 + 1;
            int j = randomInt(0, 4);
            int yy = y + NEIGHBOR[j].first;//计算传送门相邻单元格
            int xx = x + NEIGHBOR[j].second;
            if(grid[yy][xx] != TileType::EmptyOrWall)continue;
            grid[y][x] = TileType::Portal;
            grid[yy][xx] = TileType::Empty;
            for(int k = 0; k < 4; k++) {//检查传送门周围其他方向
                if(k == j)continue;
                if(grid[y + NEIGHBOR[k].first][x + NEIGHBOR[k].second] == TileType::EmptyOrWall) {
                    grid[y + NEIGHBOR[k].first][x + NEIGHBOR[k].second] = TileType::Wall;
                }//确保传送门周围只有一个出口,这才是真正的迷宫游戏
            }
            gridPortal[y][x] = portals[i];
            break;
        }
    }

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            if(grid[y][x] == TileType::EmptyOrWall) {
                edges.emplace_back(y, x);
            }
        }
    }
    // randomShuffle(edges.begin(), edges.end());
    random_device rd;
    shuffle(edges.begin(), edges.end(), default_random_engine(rd()));//随机洗牌，确保“真”随机
    DisjointSetUnion dsu(height * width);
    for(auto [y, x] : edges) {
        int p1, p2;
        if(y % 2 == 1) {
            p1 = y * width + x - 1;
            p2 = y * width + x + 1;
        } else {
            p1 = (y - 1) * width + x;
            p2 = (y + 1) * width + x;
        }
        if(dsu.getRootId(p1) == dsu.getRootId(p2)) {//if已经连通，中间设置为墙壁
            grid[y][x] = TileType::Wall;
        } else {//if未连通，中间设置为empty，使其联通
            grid[y][x] = TileType::Empty;
            dsu.join(p1, p2);
        }
    }
}

NormalRoom::NormalRoom(string id, int height, int width, vector<string> portals):
    Room(id, height, width, portals)
{

}

TaskRoom::TaskRoom(string id, int height, int width, vector<string> portals, int requiredExp, vector<string> tasks,int isTreasure,int isKeys):
    Room(id, height, width, portals),
    requiredExp(requiredExp),
    tasks(std::move(tasks)),
    isTreasure(isTreasure),
    isKeys(isKeys)
{

}

void TaskRoom::generate() {
    Room::generate();
    gridTask.assign(height, vector<string>(width));
    for(int i = 0; i < tasks.size(); i++) {//放任务点
        while(true) {
            int y = randomInt(1, height - 1);
            int x = randomInt(1, width - 1);
            if(grid[y][x] != TileType::Empty)continue;
            grid[y][x] = TileType::Task;
            gridTask[y][x] = tasks[i];
            break;
        }
    }
}

void TaskRoom::removeTaskAt(int y, int x) {
    gridTask[y][x].clear();
    grid[y][x] = TileType::Empty;
}

RoomPtr makeRoom(const JsonObject& roomObject)
{
    string id = any_cast<string>(roomObject.at("id"));

    RoomType type;
    string typeStr = any_cast<string>(roomObject.at("type"));
    if(typeStr == "normal")type = RoomType::Normal;
    else if(typeStr == "task")type = RoomType::Task;
    else throw runtime_error("房间类型错误");

    int height = (int)any_cast<unsigned int>(roomObject.at("height"));
    int width = (int)any_cast<unsigned int>(roomObject.at("width"));

    const JsonArray& portalsArray = any_cast<JsonArray>(roomObject.at("portals"));
    vector<string> portals = toStringList(portalsArray);

    if(type == RoomType::Normal) {
        return RoomPtr(new NormalRoom(id, height, width, portals));
    }

    int requiredExp = (int)any_cast<unsigned int>(roomObject.at("required_exp"));

    const JsonArray& taskArray = any_cast<JsonArray>(roomObject.at("tasks"));
    vector<string> tasks = toStringList(taskArray);

    return RoomPtr(new TaskRoom(id, height, width, portals, requiredExp, tasks, 0, 0));
}
