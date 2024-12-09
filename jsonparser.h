#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

using JsonObject = unordered_map<string, any>;
using JsonArray  = vector<any>;

class JsonParser {
public:
    any parse(const string& json);

protected:
    size_t index;//跟踪解析过程中的当前位置
    string input;

    bool isWhitespace(char c);
    void skipWhitespace();//跳过
    any parseValue();//解析 JSON 值
    unsigned int parseNumber(); // 解析 JSON 数字
    string parseString();
    JsonObject parseObject();
    JsonArray parseArray();
    bool parseBoolean();
    void parseNull();
};

vector<string> toStringList(const JsonArray& jsonArray);//数组变字符串

void printJson(const any& value, int indentLevel = 0);//

#endif // JSONPARSER_H
