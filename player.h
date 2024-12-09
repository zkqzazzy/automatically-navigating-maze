#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

class Player
{
public:
    Player();
    const string& getRoom() const { return roomId; }
    void setRoom(const string& value) { roomId = value; }
    int getX() const { return x; }
    void setX(int value) { x = value; }
    int getY() const { return y; }
    void setY(int value) { y = value; }

private:
    string roomId;
    int x, y;
};

#endif // PLAYER_H
