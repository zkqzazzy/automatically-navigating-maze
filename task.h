#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>
#include <memory>
#include "jsonparser.h"
using namespace std;

enum TaskType
{
    BlankFilling, MultipleChoice, WuZiQi, Treasure, Keys
};

class Task
{
public:
    Task(string id, int exp, string question, string answer);
    virtual TaskType getType() const = 0;
    const string& getId() const { return id; }
    int getExp() const { return exp; }
    // bool isSolved() const { return solved; }
    // void setSolved(bool value) { solved = value; }
    const string& getQuestion() const { return question; }
    const string& getAnswer() const { return answer; }

protected:
    string id;
    int exp; // 答对后获得的经验值
    // bool solved;
    string question; // 问题
    string answer; // 答案
};

class BlankFillingTask: public Task
{
public:
    BlankFillingTask(string id, int exp, string question, string answer);
    TaskType getType() const { return TaskType::BlankFilling; }
};

class MultipleChoiceTask: public Task
{
public:
    MultipleChoiceTask(string id, int exp, string question, string answer, vector<string> choices);
    TaskType getType() const { return TaskType::MultipleChoice; }
    const vector<string>& getChoices() const { return choices; }

protected:
    vector<string> choices; // 选项
};

class WuZiQiTask: public Task
{
public:
    WuZiQiTask(string id, int exp, string question, string answer);
    TaskType getType() const{ return TaskType::WuZiQi; }
};

class TreasureTask: public Task
{
public:
    TreasureTask(string id, int exp, string question, string answer);
    TaskType getType() const{ return TaskType::Treasure; }
};

class KeysTask: public Task
{
public:
    KeysTask(string id, int exp, string question, string answer);
    TaskType getType() const{ return TaskType::Keys; }
};


using TaskPtr = shared_ptr<Task>;

TaskPtr makeTask(const JsonObject& taskObject);

#endif // TASK_H
