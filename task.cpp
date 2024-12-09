#include "task.h"
#include "utils.h"

Task::Task(string id, int exp, string question, string answer):
    id(std::move(id)),
    exp(exp),
    // solved(false),
    question(std::move(question)),
    answer(std::move(answer))
{

}

BlankFillingTask::BlankFillingTask(string id, int exp, string question, string answer):
    Task(id, exp, question, answer)
{

}

MultipleChoiceTask::MultipleChoiceTask(string id, int exp, string question, string answer, vector<string> choices):
    Task(id, exp, question, answer),
    choices(std::move(choices))
{

}

//五子棋
WuZiQiTask::WuZiQiTask(string id, int exp, string question, string answer):
    Task(id, exp, question, answer)
{

}

//宝箱
TreasureTask::TreasureTask(string id, int exp, string question, string answer):
   Task(id, exp, question, answer)
{

}

//钥匙
KeysTask::KeysTask(string id, int exp, string question, string answer):
    Task(id, exp, question, answer)
{

}

TaskPtr makeTask(const JsonObject& taskObject)
{
    TaskType type;
    string typeStr = any_cast<string>(taskObject.at("type"));
    if(typeStr == "blank_filling")type = TaskType::BlankFilling;
    else if(typeStr == "multiple_choice")type = TaskType::MultipleChoice;
    else if(typeStr == "wu_zi_qi") type = TaskType::WuZiQi; //五子棋
    else if(typeStr == "treasure") type = TaskType::Treasure;//宝箱
    else if(typeStr == "keys") type = TaskType::Keys;
    else throw runtime_error("任务类型错误");

    string id = any_cast<string>(taskObject.at("id"));
    int exp = (int)any_cast<unsigned int>(taskObject.at("exp"));
    // string question = any_cast<string>(taskObject.at("question"));
    // string answer = any_cast<string>(taskObject.at("answer"));
    string question = "";
    string answer = "";

    if(type == TaskType::BlankFilling) {
        question = any_cast<string>(taskObject.at("question"));
        answer = any_cast<string>(taskObject.at("answer"));
        if(question == ""){
            int a = randomInt(1,10);
            int b = randomInt(1,10);
            question = to_string(a) + "*" + to_string(b) + "=";
            int c = a * b;
            answer = to_string(c);
        }
        return TaskPtr(new BlankFillingTask(id, exp, question, answer));
    }

    //五子棋
    if(type == TaskType::WuZiQi){
        return TaskPtr(new WuZiQiTask(id, exp, question, answer));
    }

    //宝箱和钥匙
    if(type == TaskType::Treasure){
        return TaskPtr(new TreasureTask(id, exp, question, answer));
    }

    //钥匙
    if(type == TaskType::Keys){
        return TaskPtr(new KeysTask(id, exp, question, answer));
    }

    question = any_cast<string>(taskObject.at("question"));
    answer = any_cast<string>(taskObject.at("answer"));
    vector<string> choices;
    //如果为空就直接进行随机
    if(question == ""){
        int a = randomInt(1,50);
        int b = randomInt(1,50);
        question = to_string(a) + "+" + to_string(b) + "=";
        int c = a + b;
        choices.push_back(to_string(c));
        choices.push_back(to_string(a));
        choices.push_back(to_string(b));
        answer = to_string(c);
        return TaskPtr(new MultipleChoiceTask(id, exp, question, answer, choices));
    }else{
        const JsonArray& choicesArray = any_cast<JsonArray>(taskObject.at("choices"));
        choices = toStringList(choicesArray);

        return TaskPtr(new MultipleChoiceTask(id, exp, question, answer, choices));
    }
}
