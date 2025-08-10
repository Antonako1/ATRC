#define _CRT_SECURE_NO_DEPRECATE
#include "./include/ATRC/ATRC.h"
#include "./include/filer.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <cctype>
#include <stack>
#include <cmath>

uint64_t atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
bool atrc_stdlib_writecheck = false;

std::string str_to_lower(const char *str){
    std::string res = "";
    for(size_t i = 0; i < strlen(str); i++){
        res += std::tolower(str[i]);
    }
    return res;
}
void str_to_lower_s(std::string &str){
    for(size_t i = 0; i < str.size(); i++){
        str[i] = std::tolower(str[i]);
    }
}

std::vector<std::string> split(const std::string &str, char separator){
    std::vector<std::string> res;
    std::string buf = "";
    for(const char &c : str){
        if(c == separator){
            res.push_back(buf);
            buf = "";
            continue;
        }
        buf += c;
    }
    if(buf.size() > 0){
        res.push_back(buf);
    }
    return res;
}

std::string str_to_upper(const char *str){
    std::string res = "";
    for(size_t i = 0; i < strlen(str); i++){
        res += std::toupper(str[i]);
    }
    return res;
}

void str_to_upper_s(std::string &str){
    for(size_t i = 0; i < str.size(); i++){
        str[i] = std::toupper(str[i]);
    }
}

std::vector<std::string> atrc_to_vector(char separator, const std::string &value){
    atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
    std::vector<std::string> res;
    if(value.size() == 0){
        return res;
    }
    std::string temp_value = value;
    if(temp_value.at(temp_value.size()-1) != separator){
        temp_value += separator;
    }
    std::string buf = "";
    for(const char &c : temp_value){
        if(c == separator){
            res.push_back(buf);
            buf = "";
            continue;
        }
        buf += c;
    }
    atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
    return res;
}

PString_Arr atrc_to_list(const char separator, const char* value) {
    if (value == NULL) return NULL;

    atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;

    PString_Arr res = (PString_Arr)malloc(sizeof(String_Arr));
    if (res == NULL) return NULL;

    std::vector<std::string> temp = atrc_to_vector(separator, value);
    if (temp.empty()) {
        __ATRC_FREE_MEMORY_EX(res);
        return NULL;
    }

    res->list = (char**)malloc(temp.size() * sizeof(char*));
    if (res->list == NULL) {
        __ATRC_FREE_MEMORY_EX(res);
        return NULL;
    }

    res->count = temp.size();
    for (size_t i = 0; i < temp.size(); i++) {
        res->list[i] = (char*)malloc(temp[i].size() + 1);
        if (res->list[i] == NULL) {
            // Cleanup previously allocated memory
            for (size_t j = 0; j < i; j++) {
                __ATRC_FREE_MEMORY_EX(res->list[j]);
            }
            __ATRC_FREE_MEMORY_EX(res->list);
            __ATRC_FREE_MEMORY_EX(res);
            return NULL;
        }
        std::strcpy(res->list[i], temp[i].c_str());
    }

    atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
    return res;
}


void *atrc_free_list(PString_Arr list) {
    atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
    if (list == NULL){ 
        return NULL;
    }

    for (size_t i = 0; i < list->count; i++) {
        __ATRC_FREE_MEMORY_EX(list->list[i]);
    }
    __ATRC_FREE_MEMORY_EX(list->list);
    list->count = 0; // Reset count
    if(list != NULL) __ATRC_FREE_MEMORY_EX(list);
    atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
    return NULL;
}

bool atrc_to_bool(const char* value){
    std::string temp=str_to_lower(value);
    trim(temp);
    if(temp == "true" || temp == "1" || temp == "yes" || temp == "on"){
        atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
        return true;
    } else if(temp == "false" || temp == "0" || temp == "no" || temp == "off"){
        atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
        return false;
    } else {
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
        return false;
    }
}

uint64_t atrc_to_uint64_t(const char* value){
    uint64_t res = 0;
    try {
        res = std::stoull(value);
        atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
    } catch (const std::invalid_argument&) {
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
        errormsg(ERR_STDLIB_CAST_ERROR, -1, value, "atrc_stdlib");
    } catch (const std::out_of_range&) {
        errormsg(ERR_STDLIB_CAST_ERROR, -1, value, "atrc_stdlib");
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
    }
    return res;
}

int64_t atrc_to_int64_t(const char* value){
    int64_t res = 0;
    try {
        res = std::stoll(value);
        atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
    } catch (const std::invalid_argument&) {
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
        errormsg(ERR_STDLIB_CAST_ERROR, -1, value, "atrc_stdlib");
    } catch (const std::out_of_range&) {
        errormsg(ERR_STDLIB_CAST_ERROR, -1, value, "atrc_stdlib");
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
    }
    return res;
}

double atrc_to_double(const char* value){
    double res = 0;
    try {
        res = std::stod(value);
        atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
    } catch (const std::invalid_argument&) {
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
        errormsg(ERR_STDLIB_CAST_ERROR, -1, value, "atrc_stdlib");
    } catch (const std::out_of_range&) {
        errormsg(ERR_STDLIB_CAST_ERROR, -1, value, "atrc_stdlib");
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
    }
    return res;
}



static const std::unordered_map<std::string, MATH_OPERATOR_TYPE> operatorMap = {
    {"+", MATH_OPERATOR_TYPE::ADD}, {"-", MATH_OPERATOR_TYPE::SUBTRACT},
    {"*", MATH_OPERATOR_TYPE::MULTIPLY}, {"/", MATH_OPERATOR_TYPE::DIVIDE},
    {"%", MATH_OPERATOR_TYPE::MODULO},   {"^", MATH_OPERATOR_TYPE::POWER},
    {"(", MATH_OPERATOR_TYPE::OPEN_PAREN}, {")", MATH_OPERATOR_TYPE::CLOSE_PAREN},
    {"SQRT", MATH_OPERATOR_TYPE::SQRT}, {"ABS", MATH_OPERATOR_TYPE::ABS},
    {"LOG", MATH_OPERATOR_TYPE::LOG},   {"LOG10", MATH_OPERATOR_TYPE::LOG10},
    {"SIN", MATH_OPERATOR_TYPE::SIN},   {"COS", MATH_OPERATOR_TYPE::COS},
    {"TAN", MATH_OPERATOR_TYPE::TAN},   {"ASIN", MATH_OPERATOR_TYPE::ASIN},
    {"ACOS", MATH_OPERATOR_TYPE::ACOS}, {"ATAN", MATH_OPERATOR_TYPE::ATAN},
};

double MathParser::evaluate(const std::string& expression) {
    auto tokens = tokenize(expression);
    auto rpn = toRPN(tokens);
    return evaluateRPN(rpn);
}

// Tokenizer
std::vector<MathToken> MathParser::tokenize(const std::string& input) {
    std::vector<MathToken> tokens;
    size_t i = 0;
    while (i < input.size()) {
        if (std::isspace(input[i])) {
            i++;
        } else if (std::isdigit(input[i]) || input[i] == '.') {
            size_t len = 0;
            double val = std::stod(&input[i], &len);
            tokens.push_back({MATH_TOKEN_TYPE::NUMBER, val, MATH_NUMBER_TYPE::FLOAT});
            i += len;
        } else if (input[i] == '0' && (i+1 < input.size()) && (input[i+1] == 'x' || input[i+1] == 'X')) {
            i += 2;
            size_t len = 0;
            int val = std::stoi(&input[i], &len, 16);
            tokens.push_back({MATH_TOKEN_TYPE::NUMBER, static_cast<double>(val), MATH_NUMBER_TYPE::HEXADECIMAL});
            i += len;
        } else if (input[i] == '0' && (i+1 < input.size()) && (input[i+1] == 'b' || input[i+1] == 'B')) {
            i += 2;
            size_t len = 0;
            int val = std::stoi(&input[i], &len, 2);
            tokens.push_back({MATH_TOKEN_TYPE::NUMBER, static_cast<double>(val), MATH_NUMBER_TYPE::BINARY});
            i += len;
        } else {
            bool matched = false;
            for (const auto& [str, op] : operatorMap) {
                if (input.compare(i, str.length(), str) == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::OPERATOR, op});
                    i += str.length();
                    matched = true;
                    break;
                }
            }
            if (!matched) {
                if (input.compare(i, 2, "PI") == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::NUMBER, ATRC_PI, MATH_NUMBER_TYPE::PI});
                    i += 2;
                } else if (input.compare(i, 1, "E") == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::NUMBER, ATRC_E, MATH_NUMBER_TYPE::E});
                    i += 1;
                } else if (input.compare(i, 2, "GR") == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::NUMBER, ATRC_GOLDEN_RATIO, MATH_NUMBER_TYPE::GOLDEN_RATIO});
                    i += 2;
                } else if (input.compare(i, 4, "SQRT2") == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::NUMBER, ATRC_SQRT2, MATH_NUMBER_TYPE::SQRT2});
                    i += 4;
                } else if (input.compare(i, 4, "LOG2E") == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::NUMBER, ATRC_LOG2E, MATH_NUMBER_TYPE::LOG2E});
                    i += 4;
                } else if (input.compare(i, 6, "LOG10E") == 0) {
                    tokens.push_back({MATH_TOKEN_TYPE::NUMBER, ATRC_LOG10E, MATH_NUMBER_TYPE::LOG10E});
                    i += 6;
                } else {
                    // Handle unknown token
                    errormsg(ERR_INVALID_EXPRESSION, -1, "Unknown token: " + input.substr(i), "atrc_stdlib");
                }
            }
        }
    }
    return tokens;
}

// Simple operator precedence
int precedence(MATH_OPERATOR_TYPE op) {
    switch (op) {
        case MATH_OPERATOR_TYPE::ADD:
        case MATH_OPERATOR_TYPE::SUBTRACT: return 1;
        case MATH_OPERATOR_TYPE::MULTIPLY:
        case MATH_OPERATOR_TYPE::DIVIDE:
        case MATH_OPERATOR_TYPE::MODULO: return 2;
        case MATH_OPERATOR_TYPE::POWER: return 3;
        default: return 0;
    }
}

// Shunting Yard
std::vector<MathToken> MathParser::toRPN(const std::vector<MathToken>& tokens) {
    std::vector<MathToken> output;
    std::stack<MathToken> operators;

    for (const auto& token : tokens) {
        if (token.type == MATH_TOKEN_TYPE::NUMBER) {
            output.push_back(token);
        } else {
            MATH_OPERATOR_TYPE op = std::get<MATH_OPERATOR_TYPE>(token.value);
            if (op == MATH_OPERATOR_TYPE::OPEN_PAREN) {
                operators.push(token);
            } else if (op == MATH_OPERATOR_TYPE::CLOSE_PAREN) {
                while (!operators.empty() && std::get<MATH_OPERATOR_TYPE>(operators.top().value) != MATH_OPERATOR_TYPE::OPEN_PAREN) {
                    output.push_back(operators.top());
                    operators.pop();
                }
                if (!operators.empty()) operators.pop(); // Remove '('
            } else {
                while (!operators.empty()) {
                    auto top = operators.top();
                    if (top.type == MATH_TOKEN_TYPE::OPERATOR &&
                        precedence(std::get<MATH_OPERATOR_TYPE>(top.value)) >= precedence(op)) {
                        output.push_back(top);
                        operators.pop();
                    } else {
                        break;
                    }
                }
                operators.push(token);
            }
        }
    }

    while (!operators.empty()) {
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

// RPN Evaluator
double MathParser::evaluateRPN(const std::vector<MathToken>& rpn) {
    std::stack<double> stack;

    for (const auto& token : rpn) {
        if (token.type == MATH_TOKEN_TYPE::NUMBER) {
            stack.push(std::get<double>(token.value));
        } else {
            size_t op = (size_t)std::get<MATH_OPERATOR_TYPE>(token.value);

            size_t required_operands = (op == (size_t)MATH_OPERATOR_TYPE::SQRT || op == (size_t)MATH_OPERATOR_TYPE::LOG) ? 1u : 2u;
            if (stack.size() < required_operands) {
                errormsg(ERR_INVALID_EXPRESSION, -1, "Insufficient operands for operator: " + std::to_string(static_cast<int>(op)), "atrc_stdlib");
                return 0.0; // Return a default value on error
            }
            double b = stack.top(); stack.pop();
            double a = (stack.empty() ? 0 : stack.top());
            if (op != (size_t)MATH_OPERATOR_TYPE::SQRT && op != (size_t)MATH_OPERATOR_TYPE::LOG) stack.pop();

            switch (op) {
                case (size_t)MATH_OPERATOR_TYPE::ADD: stack.push(a + b); break;
                case (size_t)MATH_OPERATOR_TYPE::SUBTRACT: stack.push(a - b); break;
                case (size_t)MATH_OPERATOR_TYPE::MULTIPLY: stack.push(a * b); break;
                case (size_t)MATH_OPERATOR_TYPE::DIVIDE: 
                    if (b == 0) {
                        errormsg(ERR_INVALID_EXPRESSION, -1, "Division by zero " + std::to_string(a) + " / " + std::to_string(b), "atrc_stdlib");
                        return 0.0; // Return a default value on error
                    }
                    if (b != 0) {
                        double res = a / b;
                        stack.push(res);
                    } 
                    break;
                case (size_t)MATH_OPERATOR_TYPE::MODULO: 
                    if (b == 0) {
                        errormsg(ERR_INVALID_EXPRESSION, -1, "Modulo by zero " + std::to_string(a) + " % " + std::to_string(b), "atrc_stdlib");
                        return 0.0; // Return a default value on error
                    }
                    stack.push(fmod(a, b)); 
                    break;
                case (size_t)MATH_OPERATOR_TYPE::POWER: stack.push(pow(a, b)); break;

                case (size_t)MATH_OPERATOR_TYPE::SQRT: stack.push(sqrt(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::ABS: stack.push(fabs(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::LOG: stack.push(log(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::LOG10: stack.push(log10(b)); break;

                case (size_t)MATH_OPERATOR_TYPE::SIN: stack.push(sin(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::COS: stack.push(cos(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::TAN: stack.push(tan(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::ASIN: stack.push(asin(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::ACOS: stack.push(acos(b)); break;
                case (size_t)MATH_OPERATOR_TYPE::ATAN: stack.push(atan(b)); break;

                default:
                    errormsg(ERR_INVALID_EXPRESSION, -1, "Unknown operator: " + std::to_string(static_cast<int>(op)), "atrc_stdlib");
                    return 0.0f; // Return a default value on error
            }
        }
    }

    if (stack.size() != 1) {
        errormsg(ERR_INVALID_EXPRESSION, -1, "Invalid expression: stack size is not 1 after evaluation", "atrc_stdlib");
        stack.push(0.0); // Clear the stack and push a default value
    }
    return stack.top();
}



double atrc_math_exp(const char* value) {
    MathParser parser;
    try {
        double result = parser.evaluate(value);
        atrc_stdlib_errval = _ATRC_SUCCESSFULL_ACTION;
        return result;
    } catch (const std::exception&) {
        errormsg(ERR_INVALID_EXPRESSION, -1, value, "atrc_stdlib");
        atrc_stdlib_errval = _ATRC_UNSUCCESSFULL_ACTION;
        return 0.0; // Return a default value on error
    }
}


