#include <string>
#include <map>
#include <iostream>
#include <utility>
#include <vector>
#include <set>
#include <cassert>
#include <fstream>
/*
 * @author Xu Yusheng
 * @since 2020-12-28
 * @version 0.1
 * */
using namespace std;
using string_to_string_map = map<string, string>;
using string_to_int_map = map<string, int>;
using int_to_int_map = map<int, int>;
using variable_set = set<string>;
using int_set = set<int>;
using int_vec = vector<int>;
using string_vec = vector<string>;

void handler(const string &cmd);

string_vec callingLink;
string_to_int_map globalVariable;
bool dbg = true;

int execute();

//函数调用规则，碰见param就加入临时变量暂存区，碰见call则晴空临时变量暂存区
struct Function {
    string_vec variables;
    string functionName;

    explicit Function(string_vec &&consumed_string_vec) : variables(consumed_string_vec) {};

    Function() = default;;
};

using string_to_function_map = map<string, Function>;
vector<string> reversedFunc = {"input", "output"};
//变量声明
string_to_int_map variableRecord;
string_to_int_map functionRecord;
int_to_int_map lineToIdxRecord; //代码-索引映射表

variable_set variableSet;
string_vec codeVec; //代码行

string_to_function_map functionBaseRecord;
int_vec tempVariable;
int stackPointer = 0;

struct FunctionFrame {
    string_to_int_map varMap;
};

FunctionFrame buildFunctionFrame(const Function &baseFunction) {
    const auto &nameVec = baseFunction.variables;
    FunctionFrame functionFrame;

    if (nameVec.size() > tempVariable.size()) {
        cout << "参数不匹配" << baseFunction.functionName << endl;
        assert(0);
    }

    for (int i = 0; i < nameVec.size(); ++i)
        functionFrame.varMap[nameVec[i]] = tempVariable[i];

    tempVariable.clear();
    return functionFrame;
}

FunctionFrame curFrame;
namespace OP {
    template<typename T>
    T add(const T &a, const T &b) { return a + b; }

    template<typename T>
    T mul(const T &a, const T &b) { return a * b; }

    template<typename T>
    T div(const T &a, const T &b) { return a / b; }

    template<typename T>
    T minus(const T &a, const T &b) { return a - b; }

    template<typename T>
    T bitAnd(const T &a, const T &b) { return a & b; }

    template<typename T>
    T bitOr(const T &a, const T &b) { return a | b; }

    template<typename T>
    T bitXor(const T &a, const T &b) { return a ^ b; }

    template<typename T>
    T assign(T &a, const T &b) { a = b; }

    template<typename T>
    bool equals(const T &a, const T &b) { return a == b; }
} // namespace OP

template<typename T>
auto lookUpVariable(const map<string, T> &_map, const string &key) {
    return _map.find(key);
}

template<typename T1, typename T2>
auto printMap(const map<T1, T2> &_map) -> void {
    for (const auto &[key, value] : _map)
        cout << key << " " << value << endl;
}

template<typename T>
auto printSet(const set<T> &_set) -> void {
    for (const auto &v : _set)
        cout << v << " ";
    cout << endl;
}

template<typename T>
auto printLink(const vector<T> &vec) -> void {
    cout << "Function Frame: " << " ";
    bool work = false;
    for (const auto &v : vec) {
        if (!work)
            cout << v << "->";
        else {
            work = true;
            cout << v;
        }
    }
    cout << endl;
}

int programCounter = 0;

auto loadSymbol(const string &filePath) {
    cout << "Load Symbol => " << filePath << endl;
    ifstream symbolFile(filePath);
    if (!symbolFile.is_open()) {
        cout << "No This File" << endl;
        return;
    }
    string functionName;
    while (symbolFile >> functionName) {
        string variableName;
        symbolFile >> variableName;
        string_vec varSeq;
        while (symbolFile >> variableName && variableName != "end")
            varSeq.push_back(variableName);
        Function function(std::move(varSeq));
        function.functionName = functionName;
        functionBaseRecord[functionName] = function;
    }
    symbolFile.close();
}

auto loadCode(const string &filePath) {
    cout << "Load Code => " << filePath << endl;
    ifstream codeFile(filePath);
    if (!codeFile.is_open()) {
        cout << "No This File" << endl;
        return;
    }
    string tempStr;
    while (getline(codeFile, tempStr))
        handler(tempStr);
}

void handler(const string &cmd) {
#ifdef DEBUG
    cout << cmd << endl;
#endif
    if (cmd == "dbg") {
        dbg = !dbg;
    } else if (cmd == "total") {
        printMap(globalVariable);
    } else if (cmd == "code") {
        for (const auto &lineCode : codeVec)
            cout << lineCode << endl;
    } else if (cmd == "funcBase") {
        cout << "All Function Symbols ... " << endl;
        for (const auto &[funcName, funcBody] : functionBaseRecord) {
            cout << "DEFINITION: " << funcName;
            cout << " Variables: ";
            for (const auto &ele : funcBody.variables)
                cout << ele << " ";
            cout << endl;
        }
    } else if (cmd == "temp") {
        for (const auto &ele:tempVariable)
            cout << ele << " ";
        cout << endl;
    } else if (cmd == "clear") {
        programCounter = 0;
    } else if (cmd.substr(0, 8) == "loadcode") {
        loadCode(cmd.substr(9));
    } else if (cmd.substr(0, 8) == "loadfile") {
        loadSymbol(cmd.substr(9));
    } else if (cmd == "frame") {
        printMap(curFrame.varMap);
    } else if (cmd == "var") {
        printMap(variableRecord);
    } else if (cmd == "func") {
        printMap(functionRecord);
    } else if (cmd == "count") {
        cout << "Counter = " << programCounter << endl;
    } else if (cmd == "stack") {
        printLink(callingLink);
    } else if (cmd == "main") {
        tempVariable = int_vec(functionBaseRecord["main"].variables.size(), 0);
        curFrame = buildFunctionFrame(functionBaseRecord["main"]);
        stackPointer = functionRecord["main"];
        callingLink.push_back(cmd);
        execute();
        cout << "程序执行完毕" << endl;
    } else if (cmd[0] == '(') {
        int lineNum = 0, idx = 1;
        while (cmd[idx++] != ')')
            lineNum = lineNum * 10 + cmd[idx - 1] - '0';
        int actualNum = lineToIdxRecord.size();
        codeVec.push_back(cmd.substr(idx + 1));
        lineToIdxRecord[lineNum] = actualNum;
        if (codeVec.back().substr(0, 4) == "Prog")
            functionRecord[codeVec.back().substr(5)] = lineNum;
    }
}

template<typename T>
T input(const string &var) {
    cout << "来自程序的输入，输入定向至 " << var << ": ";
    T temp;
    cin >> temp;
    return temp;
}

template<typename T>
T input() {
    cout << "来自程序的输入，输入定向至 : ";
    T temp;
    cin >> temp;
    return temp;
}

template<typename T>
auto output(const T &val) {
    cout << "来自程序的输出 " << val << endl;
}

auto payload(const string &code) {
}

auto parse(const string &code) {
}

auto isNum(const char &ch) {
    return ch >= '0' && ch <= '9';
}

auto split(const string &s, const string &separator) {
    vector<string> result;
    int posBegin = 0;
    int posSeparator = s.find(separator);

    while (posSeparator != std::string::npos) {
        result.push_back(s.substr(posBegin, posSeparator - posBegin)); //
        posBegin = posSeparator + (int) separator.size();                    // 分隔符的下一个元素
        posSeparator = s.find(separator, posBegin);
    }
    if (posBegin != s.length()) // 指向最后一个元素，加进来
        result.push_back(s.substr(posBegin));

    return std::move(result);
}

auto startWith(const string &a, const string &b) {
    int len = b.length();
    if (a.length() < b.length())
        return false;
    return a.substr(0, len) == b;
}

int string_to_int(string str) {
    bool f = false;
    if (str[0] == '-') str = str.substr(1), f = true;
    int localNumber = 0;
    for (const auto &ch : str) {
        localNumber = localNumber * 10 + ch - '0';
    }
    if (f) localNumber = -localNumber;
    return localNumber;
}

int calc(const string_vec &pars) {
    int res = string_to_int(pars.front());
    for (int i = 1; i < pars.size(); i += 2) {
        string op = pars[i];
        int opd = string_to_int(pars[i + 1]);
        switch (op[0]) {
            case '+':
                res = OP::add(res, opd);
                break;
            case '-':
                res = OP::minus(res, opd);
                break;
            case '*':
                res = OP::mul(res, opd);
                break;
            case '/':
                res = OP::div(res, opd);
                break;
            case '&':
                res = OP::bitAnd(res, opd);
                break;
            case '|':
                res = OP::bitOr(res, opd);
                break;
            case '^':
                res = OP::bitXor(res, opd);
                break;
            case '=':
                res = OP::equals(res, opd);
                break;
            case '!':
                res = !OP::equals(res, opd);
                break;
            default:
                cerr << "unsupported operator! " << op << endl;
        }
    }
    return res;
}

int fetch(const string &name) {
    if (isNum(name.front()))
        return string_to_int(name);
    if (curFrame.varMap.find(name) != curFrame.varMap.end())
        return curFrame.varMap[name];
    else
        return variableRecord[name];
}

string int_to_string(int v) {
    if (v == 0) return "0";
    bool f = false;
    if (v < 0) f = true, v = -v;
    string str;
    while (v) {
        str.push_back(v % 10 + '0');
        v /= 10;
    }
    reverse(str.begin(), str.end());
    if (f) str = "-" + str;
    return str;
}

int eval(const string_vec &tokens) {
    if (tokens.size() == 3) {
        return fetch(tokens.back());
    } else if (tokens.size() == 5) {
        return calc(string_vec({int_to_string(fetch(tokens[2])), tokens[3], int_to_string(fetch(tokens[4]))}));
    } else {
        //默认call
        auto targetFunc = tokens[3];
        for (const auto &str:reversedFunc) {
            if (str == targetFunc) {
                if (str == "input") {
                    return input<int>(tokens.front());
                } else if (str == "output") {
                    output(tempVariable.front());
                    tempVariable.clear();
                    return 0;
                } else {
                    cerr << targetFunc << "为关键字但是并没有提供实现" << endl;
                }
            }
        }
        //调用函数 buildFrame T4 := call gcd
        int lastPointer = stackPointer;
        stackPointer = functionRecord[targetFunc];
        FunctionFrame functionFrame = curFrame;
        curFrame = buildFunctionFrame(functionBaseRecord[targetFunc]);
        callingLink.push_back(targetFunc);
        int temp = execute();
        stackPointer = lastPointer;
        callingLink.pop_back();
        curFrame = functionFrame;
        return temp;
    }
}

int execute() {
    while (true) {
        int goal = lineToIdxRecord[stackPointer];
        const string &code = codeVec[goal];
        cout << "Now Line :  " << "(" << stackPointer << ")" << " executing " << "` " << code << " `" << endl;
        string temp;

        while (dbg && cout << "dbg< " && cin >> temp && temp != "c") {
            handler(temp);
        }
        programCounter++;

        if (startWith(code, "Prog")) {
            ++stackPointer;
            continue;
        } else if (startWith(code, "call")) {
            string targetFunc = code.substr(5);
            bool flag = false;
            for (const auto &str:reversedFunc) {
                if (str == targetFunc) {
                    flag = true;
                    if (str == "input") {
                        input<int>();
                    } else if (str == "output") {
                        output(tempVariable.front());
                        tempVariable.clear();
                    }
                }
                ++stackPointer;
            }
            if (flag) continue;
            int lastPointer = stackPointer + 1;
            stackPointer = functionRecord[targetFunc];
            FunctionFrame functionFrame = curFrame;
            curFrame = buildFunctionFrame(functionBaseRecord[targetFunc]);
            callingLink.push_back(targetFunc);
            execute();
            stackPointer = lastPointer;
            callingLink.pop_back();
            curFrame = functionFrame;
        } else if (startWith(code, "return")) {
            if (code.size() == 6) return 0;
            string targetVariable = code.substr(7);
//            return curFrame.varMap[targetVariable];
            return fetch(targetVariable);
        } else if (startWith(code, "param")) {
            string targetExpr = code.substr(6);
            int localNumber = 0;
            if (isNum(targetExpr[0])) {
                localNumber = string_to_int(targetExpr);
            } else {
                localNumber = fetch(targetExpr);
            }
            tempVariable.push_back(localNumber);
            ++stackPointer;
        } else if (startWith(code, "Goto")) {
            stackPointer = string_to_int(code.substr(5));
        } else if (startWith(code, "if")) {
            auto &&tokens = split(code, " ");
            int len = (int) tokens.size() - 1;
            if (tokens[len - 1] != "Goto") {
                cerr << "unsupported code" << code << endl;
                assert(0);
            }
            int result = calc(
                    string_vec({int_to_string(fetch(tokens[1])), tokens[2], int_to_string(fetch(tokens[3]))}));
            if (result)
                stackPointer = string_to_int(tokens.back());
            else
                ++stackPointer;
        } else {
            auto &&tokens = split(code, " ");
            if (tokens[1] == ":=") {
                int temp = eval(tokens);
                globalVariable[tokens[0]] = temp;
                if (curFrame.varMap.find(tokens[0]) != curFrame.varMap.end())
                    curFrame.varMap[tokens[0]] = temp;
                else
                    variableRecord[tokens[0]] = temp;
            } else {
                cerr << "unsupported vm code" << endl;
            }
            ++stackPointer;
        }
    }
    return 0;
}

auto eval(const string &expr) {

    //普通表达式
}

auto main() -> int {
    string line;
    while (cout << "xys< ", getline(cin, line)) {
        handler(line);
    }
    return 0;
}