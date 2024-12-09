#include "jsonparser.h"
#include <stdexcept>
#include <cctype>

bool JsonParser::isWhitespace(char c) {
    return std::isspace(static_cast<unsigned char>(c));
}

void JsonParser::skipWhitespace() {
    while (index < input.size() && isWhitespace(input[index])) {
        ++index;
    }
}

std::any JsonParser::parse(const std::string& json) {
    input = json;
    index = 0;
    skipWhitespace();
    return parseValue();
}

std::any JsonParser::parseValue() {
    skipWhitespace();
    if (index >= input.size()) {
        throw std::runtime_error("Unexpected end of input");
    }
    char current = input[index];
    if (current == '{') {
        return parseObject();
    } else if (current == '[') {
        return parseArray();
    } else if (std::isdigit(current)) { 
        return parseNumber();
    } else if (current == '"') {
        return parseString();
    } else if (current == 't' || current == 'f') {
        return parseBoolean();
    } else if (current == 'n') {
        parseNull();
        return nullptr;
    } else {
        throw std::runtime_error("Invalid JSON input");
    }
}

unsigned int JsonParser::parseNumber() {
    size_t start = index;
    while (index < input.size() && std::isdigit(input[index])) {
        ++index;
    }
    return std::stoul(input.substr(start, index - start));
}

std::string JsonParser::parseString() {
    ++index; // skip the initial quote
    std::string result;

    while (index < input.size()) {
        char current = input[index];

        if (current == '"') {
            ++index; // Closing quote found
            break;
        }
        if (current == '\\') {
            // Handle escapes
            ++index;
            if (index >= input.size()) {
                throw std::runtime_error("Unexpected end of input after escape character");
            }
            char escapedChar = input[index];
            switch (escapedChar) {
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            case '/': result += '/'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            case 'u': {
                if (index + 4 >= input.size()) {
                    throw std::runtime_error("Invalid unicode escape sequence");
                }
                std::string hex = input.substr(index + 1, 4);
                wchar_t unicodeChar = static_cast<wchar_t>(std::stoi(hex, nullptr, 16));
                result += static_cast<char>(unicodeChar);
                index += 4;
                break;
            }
            default:
                throw std::runtime_error("Invalid escape sequence");
            }
        } else {
            result += current;
        }
        ++index;
    }

    if (input[index - 1] != '"') {
        throw std::runtime_error("Expected closing quote for string");
    }

    return result;
}

JsonObject JsonParser::parseObject() {
    ++index; // skip the opening brace
    JsonObject object;
    skipWhitespace();
    while (index < input.size() && input[index] != '}') {
        std::string key = parseString();
        skipWhitespace();
        if (input[index] != ':') {
            throw std::runtime_error("Expected ':' in object");
        }
        ++index;
        skipWhitespace();
        object[key] = parseValue();
        skipWhitespace();
        if (input[index] == ',') {
            ++index;
            skipWhitespace();
        } else if (input[index] != '}') {
            throw std::runtime_error("Expected ',' or '}' in object");
        }
    }
    if (index >= input.size() || input[index] != '}') {
        throw std::runtime_error("Expected closing '}' for object");
    }
    ++index; // skip the closing brace
    return object;
}

JsonArray JsonParser::parseArray() {
    ++index; // skip the opening bracket
    JsonArray array;
    skipWhitespace();
    while (index < input.size() && input[index] != ']') {
        array.push_back(parseValue());
        skipWhitespace();
        if (input[index] == ',') {
            ++index;
            skipWhitespace();
        } else if (input[index] != ']') {
            throw std::runtime_error("Expected ',' or ']' in array");
        }
    }
    if (index >= input.size() || input[index] != ']') {
        throw std::runtime_error("Expected closing ']' for array");
    }
    ++index; // skip the closing bracket
    return array;
}

bool JsonParser::parseBoolean() {
    if (input.compare(index, 4, "true") == 0) {
        index += 4;
        return true;
    } else if (input.compare(index, 5, "false") == 0) {
        index += 5;
        return false;
    } else {
        throw std::runtime_error("Invalid JSON boolean");
    }
}

void JsonParser::parseNull() {
    if (input.compare(index, 4, "null") == 0) {
        index += 4;
    } else {
        throw std::runtime_error("Invalid JSON null value");
    }
}

vector<string> toStringList(const JsonArray& jsonArray)
{
    vector<string> stringList;
    for (const auto& itemData : jsonArray) {
        string item = any_cast<string>(itemData);
        stringList.push_back(item);
    }
    return stringList;
}

void printJson(const any& value, int indentLevel)
{
    auto indent = string(indentLevel * 2, ' ');

    if (value.type() == typeid(unsigned int)) {
        cout << indent << any_cast<unsigned int>(value) << endl;
    } else if (value.type() == typeid(string)) {
        cout << indent << '"' << any_cast<string>(value) << '"' << endl;
    } else if (value.type() == typeid(bool)) {
        cout << indent << (any_cast<bool>(value) ? "true" : "false") << endl;
    } else if (value.type() == typeid(nullptr_t)) {
        cout << indent << "null" << endl;
    } else if (value.type() == typeid(JsonObject)) {
        const auto& obj = any_cast<JsonObject>(value);
        cout << indent << "{" << endl;
        for (const auto& [key, val] : obj) {
            cout << indent << "  \"" << key << "\": ";
            printJson(val, indentLevel + 1);
        }
        cout << indent << "}" << endl;
    } else if (value.type() == typeid(JsonArray)) {
        const auto& arr = any_cast<JsonArray>(value);
        cout << indent << "[" << endl;
        for (const auto& val : arr) {
            printJson(val, indentLevel + 1);
        }
        cout << indent << "]" << endl;
    } else {
        cout << indent << "Unknown type" << endl;
    }
}
