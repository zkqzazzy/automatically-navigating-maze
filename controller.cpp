#include <fstream>
#include <qpainter.h>
#include <string>
#include <any>
#include <QProcess>
#include <QMessageBox>
#include "controller.h"
#include "blankfillingdialog.h"
#include "multiplechoicedialog.h"
#include "utils.h"
using namespace std;

void Controller::newGame(const string& configPath)
{
    rooms.clear();
    portals.clear();

    // 解析json配置文件，结果保存到config
    ifstream fileStream(configPath);
    if (!fileStream.is_open()) {
        throw runtime_error("打开文件失败: " + configPath);
    }
    string jsonString = string(
        (istreambuf_iterator<char>(fileStream)),
        istreambuf_iterator<char>()
        );  //读取配置文件内容到jsonString
    JsonParser parser;
    config = any_cast<JsonObject>(parser.parse(jsonString));
    printJson(config);

    // 传送门
    JsonArray portalArray = any_cast<JsonArray>(config.at("portals"));
    for (const auto& portalData : portalArray) {
        const JsonObject& portalObject = any_cast<JsonObject>(portalData);
        auto portalPtr = makePortal(portalObject);
        portals.emplace(portalPtr->getId(), portalPtr);//portals是一个映射,加入键值对
    }
    for (const auto& [id, portalPtr] : portals) {
        if(portalPtr->getDestination().length() == 0) {
            continue;//只对有目的地的传送门进行后续的双向性检查
        }
        // 判断是否双向
        if(portals.at(portalPtr->getDestination())->getDestination() == id) {
            portalPtr->setBidirectional(true);
        }
    }

    // 房间
    /*创建并存储所有的房间对象，并为每个房间生成内容，同时设置房间内传送门的位置和所属房间*/
    JsonArray roomArray = any_cast<JsonArray>(config.at("rooms"));
    for (const auto& roomData : roomArray) {
        const JsonObject& roomObject = any_cast<JsonObject>(roomData);
        auto roomPtr = makeRoom(roomObject);
        roomPtr->generate();
        for(const string& portalId : roomPtr->getPortals()) {
            portals[portalId]->setRoom(roomPtr->getId()); // 设置传送门所属的房间
        }
        for(int y = 0; y < roomPtr->getHeight(); y++) {
            for(int x = 0; x < roomPtr->getWidth(); x++) {
                if(roomPtr->getGridAt(y, x) == TileType::Portal) {
                    PortalPtr portalPtr = portals[roomPtr->getPortalAt(y, x)];
                    portalPtr->setY(y);
                    portalPtr->setX(x);
                }
            }
        }
        rooms.emplace(roomPtr->getId(), roomPtr);
    }

    // 任务
    JsonArray taskArray = any_cast<JsonArray>(config.at("tasks"));
    for (const auto& taskData : taskArray) {
        const JsonObject& taskObject = any_cast<JsonObject>(taskData);
        auto taskPtr = makeTask(taskObject);
        tasks.emplace(taskPtr->getId(), taskPtr);
    }

    // 起点和终点传送门
    start = any_cast<string>(config.at("start"));
    end = any_cast<string>(config.at("end"));

    // 初始化玩家
    const auto& startPortal = portals.at(start);
    player.setRoom(startPortal->getRoom());
    player.setX(startPortal->getX());
    player.setY(startPortal->getY());

    lastTickTime = 0;
}

void Controller::render(QPixmap& canvas)
{
    int tileSize = getTileSize();//10
    canvas.fill(QColor(0, 0, 0));//涂黑，RGB
    QPainter painter(&canvas);
    painter.setPen(Qt::NoPen);
    //红色的星星
    QPixmap pixRedStar("C:/Users/86150/Desktop/maze-zkq/maze/image/redStar.jpg");
    //任务门蓝色
    QPixmap pixBlueRoom("C:/Users/86150/Desktop/maze-zkq/maze/image/blueRoom.jpg");
    //入口门黄色
    QPixmap pixYellowRoom("C:/Users/86150/Desktop/maze-zkq/maze/image/yellowRoom.jpg");
    //出口门红色
    QPixmap pixRedRoom("C:/Users/86150/Desktop/maze-zkq/maze/image/redRoom.jpg");
    //钥匙
    QPixmap pixKeys("C:/Users/86150/Desktop/maze-zkq/maze/image/keys.jpg");
    //宝箱
    QPixmap pixTreasure("C:/Users/86150/Desktop/maze-zkq/maze/image/treasure.jpg");

    RoomPtr room = rooms.at(player.getRoom());
    auto taskRoom = static_pointer_cast<TaskRoom>(room);
    for(int y = 0; y < room->getHeight(); y++) {
        for(int x = 0; x < room->getWidth(); x++) {
            TaskPtr task;
            PortalPtr portal;
            switch(room->getGridAt(y, x)) {
            case TileType::Empty:
                painter.setBrush(QColor(0xFF, 0xFF, 0xFF));//白色
                painter.drawRect(x * tileSize, y * tileSize, tileSize, tileSize);
                break;
            case TileType::Wall:
                painter.setBrush(QColor(0x03, 0xB5, 0xFF));//蓝色
                painter.drawRect(x * tileSize, y * tileSize, tileSize, tileSize);
                break;
            case TileType::Portal:
                portal = portals.at(room->getPortalAt(y, x));
                if(portal->isBidirectional()) { // 双向
                    // painter.setBrush(QColor(0x25, 0x4F, 0xEA));//深蓝色
                    painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixBlueRoom);
                } else if(portal->getDestination().empty()) { // 单向入
                    // painter.setBrush(QColor(0xEA, 0xB9, 0x25));//橘黄色
                    painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixYellowRoom);
                } else { // 单向出
                    // painter.setBrush(QColor(0x1E, 0xB5, 0x26));//绿色
                    painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixRedRoom);
                }
                // painter.drawRect(x * tileSize, y * tileSize, tileSize, tileSize);
                break;
            case TileType::Task:
                //todo 加上钥匙和宝箱
                task = tasks.at(taskRoom->getTaskAt(y, x));
                if(task->getType() == TaskType::Treasure){
                    // if(y >= x){
                    //     painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixKeys);
                    // }else
                    // {
                    painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixTreasure);
                    // }
                } else if(task->getType() == TaskType::Keys){
                    painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixKeys);
                } else {
                    // painter.setBrush(QColor(0xFF, 0x00, 0x00));//红色
                    painter.drawPixmap(x * tileSize, y * tileSize, tileSize, tileSize, pixRedStar);
                    // painter.drawRect(x * tileSize, y * tileSize, tileSize, tileSize);
                }
                break;
            default:
                break;
            }
        }
    }

    // player
    painter.setBrush(QColor(0x00, 0x00, 0x00));//待美化
    painter.drawEllipse(player.getX() * tileSize, player.getY() * tileSize, tileSize, tileSize);

    painter.end();
}

void Controller::handleKeyPress(QKeyEvent *e)
{
    while(!pendingMoves.empty())pendingMoves.pop();
    int x = player.getX();
    int y = player.getY();
    switch (e->key()) {
    case Qt::Key_Left:
        playerMoveTo(x - 1, y);
        break;
    case Qt::Key_Right:
        playerMoveTo(x + 1, y);
        break;
    case Qt::Key_Up:
        playerMoveTo(x, y - 1);
        break;
    case Qt::Key_Down:
        playerMoveTo(x, y + 1);
        break;
    default:
        break;
    }
}

void Controller::handleMousePress(QMouseEvent *e)
{
    while(!pendingMoves.empty())pendingMoves.pop();
    QPoint pos = e->pos(); // mouse点击坐标
    int dstX = pos.x() / getTileSize();
    int dstY = pos.y() / getTileSize();
    qDebug("clicked at (%d, %d)", dstX, dstY);

    RoomPtr room = rooms.at(player.getRoom());//在边界？
    if(!(0 <= dstX && dstX < room->getWidth() && 0 <= dstY && dstY < room->getHeight())) {
        return;
    }
    if(room->getGridAt(dstY, dstX) == TileType::Wall) {//在wall？
        return;
    }

    int srcX = player.getX();
    int srcY = player.getY();//begin坐标
    vector<pair<int, int>> path;
    bool found = false;
    function<void(int x, int y, int prex, int prey)> dfs;//效率不高,待优化
    dfs = [&](int x, int y, int prex, int prey) {
        if(found)return;
        path.emplace_back(x, y);
        if(x == dstX && y == dstY) {
            found = true;
            return;
        }
        for(int i = 0; i < 4; i++) {
            int yy = y + NEIGHBOR[i].first;
            int xx = x + NEIGHBOR[i].second;
            if(room->getGridAt(yy, xx) == TileType::Wall)continue;
            if(xx == prex && yy == prey)continue;
            dfs(xx, yy, x, y);
            if(found)return;
        }
        path.pop_back();//回溯
    };
    dfs(srcX, srcY, -1, -1);

    for(int i = 1; i < path.size(); i++) {//加入路径队列
        pendingMoves.push(path[i]);
    }
}

bool Controller::handleTickEvent()//循环处理移动
{
    if(pendingMoves.empty())return false;
    auto [x, y] = pendingMoves.front();
    pendingMoves.pop();
    playerMoveTo(x, y);
    return true;
}

void Controller::playerMoveTo(int x, int y)
{
    RoomPtr room = rooms.at(player.getRoom());
    if(!(0 <= x && x < room->getWidth() && 0 <= y && y < room->getHeight())) {
        return;
    }
    auto destType = room->getGridAt(y, x);
    if(destType == TileType::Wall) {
        return;
    } else if(destType == TileType::Empty) {
        player.setX(x);
        player.setY(y);
    } else if(destType == TileType::Portal) {
        PortalPtr portal = portals.at(room->getPortalAt(y, x));
        if(portal->getId() == end) {
            QMessageBox::information(nullptr, "", "胜利");
            return;
        }
        if(portal->getDestination().empty()) {
            player.setX(x);
            player.setY(y);
        } else {
            PortalPtr destPortal = portals.at(portal->getDestination());
            if(room->getType() == RoomType::Task) {
                auto taskRoom = static_pointer_cast<TaskRoom>(room);
                if(taskRoom->getRequiredExp() <= 0) {
                    player.setRoom(destPortal->getRoom());
                    player.setX(destPortal->getX());
                    player.setY(destPortal->getY());
                }
            } else {
                player.setRoom(destPortal->getRoom());
                player.setX(destPortal->getX());
                player.setY(destPortal->getY());
            }
        }
    } else if(destType == TileType::Task) {
        while(!pendingMoves.empty()) {
            pendingMoves.pop();
        }
        auto taskRoom = static_pointer_cast<TaskRoom>(room);
        TaskPtr task = tasks.at(taskRoom->getTaskAt(y, x));
        QString question(task->getQuestion().c_str());
        if(task->getType() == TaskType::MultipleChoice) {
            auto multipleChoice = static_pointer_cast<MultipleChoiceTask>(task);
            QStringList options;
            for(int i = 0; i < multipleChoice->getChoices().size(); i++) {
                options.append(QString(multipleChoice->getChoices()[i].c_str()));
            }
            MultipleChoiceDialog dialog(question, options);
            if (dialog.exec() == QDialog::Accepted) {
                QString choice = dialog.getUserChoice();
                qInfo() << choice;
                if(choice == QString(multipleChoice->getAnswer().c_str())) {
                    taskRoom->setRequiredExp(taskRoom->getRequiredExp() - multipleChoice->getExp());
                    taskRoom->removeTaskAt(y, x);
                }
            }
        } else if(task->getType() == TaskType::BlankFilling) {
            auto blankFilling = static_pointer_cast<BlankFillingTask>(task);
            QString question(blankFilling->getQuestion().c_str());
            BlankFillingDialog dialog(question);
            if (dialog.exec() == QDialog::Accepted) {
                QString choice = dialog.getUserInput();
                qInfo() << choice;
                if(choice == QString(blankFilling->getAnswer().c_str())) {
                    taskRoom->setRequiredExp(taskRoom->getRequiredExp() - blankFilling->getExp());
                    taskRoom->removeTaskAt(y, x);
                }
            }
        }else if(task->getType() == TaskType::WuZiQi){
            auto wuZiqi = static_pointer_cast<WuZiQiTask>(task);
            // QProcess process;
            // // 设置要执行的外部程序路径和参数
            // QString program = "E:/maze/WuZiQi/WuZiQi/x64/Release/WuZiQi.exe";// 这里以调用记事本为例
            // // QStringList arguments;
            // process.start(program);
            // if (!process.waitForStarted()) {
            //     std::cerr << "无法启动程序" << std::endl;
            //     return;
            // }
            // // 等待程序结束（可选，根据需求）
            // if(process.waitForFinished()){

            // }

            int result = system("C:/Users/86150/Desktop/maze-zkq/WuZiQi/WuZiQi/x64/Release/WuZiQi.exe");
            if (result == -1) {
                std::cerr << "无法执行命令" << std::endl;
            }
            else {
                taskRoom->setRequiredExp(taskRoom->getRequiredExp() - wuZiqi->getExp());
                taskRoom->removeTaskAt(y, x);
                std::cout << "notepad.exe已经结束" << std::endl;
            }
        } else if(task->getType() == TaskType::Keys){
            auto treasure = static_pointer_cast<KeysTask>(task);
            taskRoom->setRequiredExp(taskRoom->getRequiredExp() - treasure->getExp());
            taskRoom->setIsKeys(1);
            taskRoom->removeTaskAt(y, x);
        } else if(task->getType() == TaskType::Treasure) {
            auto treasure = static_pointer_cast<TreasureTask>(task);
            taskRoom->setRequiredExp(taskRoom->getRequiredExp() - treasure->getExp());
            taskRoom->setIsTreasure(1);
            taskRoom->removeTaskAt(y, x);
        }
    }
}

string Controller::getRoomId(){
    RoomPtr room = rooms.at(player.getRoom());
    string id = room->getId();
    return id;
}

bool Controller::isTaskRoom(){
    RoomPtr room = rooms.at(player.getRoom());
    if(room->getType() == RoomType::Task)
        return true;
    return false;
}

bool Controller::getTreasure(){
    RoomPtr room = rooms.at(player.getRoom());
    auto taskRoom = static_pointer_cast<TaskRoom>(room);
    if(taskRoom->getIsTreasure() == 1){
       return true;
    }
    return false;
}

bool Controller::getKeys(){
    RoomPtr room = rooms.at(player.getRoom());
    auto taskRoom = static_pointer_cast<TaskRoom>(room);
    if(taskRoom->getIsKeys() == 1){
        return true;
    }
    return false;
}
// void Controller::onTimeout(int remainingTime){
//     --remainingTime;
//     if (remainingTime > 0)   // 更新界面显示剩余时间
//     {
//         // 构建多行文本
//         // todo id不知道如何拿 还有是否拣到东西
//         QString text = tr("房间id：XX\n剩余时间：%1秒\n是否拣到钥匙：XX\n是否拣到宝箱：XX")
//                            .arg(remainingTime);
//         ui->timeLabel->setText(text);
//     }
//     else
//     {
//         timer->stop(); // 停止计时器
//         QMessageBox::warning(this, tr("游戏失败"), tr("时间到了！游戏失败！"));
//     }
// }


