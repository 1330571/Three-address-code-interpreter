//
// Created by xys on 2020/12/29.
//

//基本块
#include<fstream>
#include <vector>
#include<iostream>
#include<set>
#include<map>
#include<utility>
#include<algorithm>

using namespace std;

const int START = 100;

enum TYPE {
    DEF,
    ASSIGN_VARIABLE,
    ASSIGN_BINARY_OP,
    RET,
    PARAM,
    ASSIGN_CALL,
};

vector<string> reversed = {"input", "output"};

vector<string> split(const string &s, const string &separator) {
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

    return result;
}

struct VAL_OR_VAR {
    //0 -> constant
    //1 -> unknown variable
    //2 -> known variable
    int type{};
    string var;
    int val{};
};

namespace VAROP {
    VAL_OR_VAR add(const VAL_OR_VAR &a, const VAL_OR_VAR &b) {
        VAL_OR_VAR ret;

    }
};

struct NODE {
    NODE *left; //左子树
    NODE *right; //右子树

    bool isValue; //是否是数值

    string op; //该结点的运算过程
    string appliedCode; //制造非叶子结点的代码
    vector<VAL_OR_VAR> varName; //变量名
};

struct DAG {
    NODE *head{};
    struct VAR {
        string name;
        vector<VAL_OR_VAR> value;
    };
    map<string, VAR> varFrame;
    map<string, NODE *> varPos;
};

struct CODE {
    vector<string> codes;
    vector<int> codeOfBlock;

    struct block {
        int l, r;
    };
    struct functionPrototype {
        string functionName;
        int defLine{};
        set<string> variableName;
    };
    vector<block> block;
    map<string, functionPrototype> funcRecord;

    auto printTheBlock(int num) -> void {
        cout << "Block " << num << endl;
        auto[l, r] = block[num];
        for (int iter = l; iter <= r; ++iter)
            cout << codes[iter] << endl;
    }

    auto blockOptimize(int num) -> void {
        auto[l, r] = block[num]; //local optimize form l to r
        DAG dag;
        auto fetch = [&](const string &name) -> VAL_OR_VAR {
            auto it = dag.varFrame.find(name);
            if (it == dag.varFrame.end()) {
                DAG::VAR newVar;
                newVar.name = name;
                VAL_OR_VAR valOrVar;
                valOrVar.type = 1;
                valOrVar.var = "_" + name;
                newVar.value.push_back(valOrVar);
                NODE node;
                node.isValue = true;
                node.left = node.right = nullptr;
                node.varName.push_back(valOrVar);
                dag.varPos.insert({name, &node});
                return valOrVar;
            } else {
                return it->second.value.back();
            }
        };
        for (int iter = l; iter <= r; ++iter) {
            string line = codes[iter];
            auto tokens = split(line, " ");
            if (tokens[1] == ":=") {
                //是赋值语句
                if (tokens.size() == 3) {
                    
                }
                if (tokens.size() == 5) {

                }
            }
        }
    }

    auto printBlock() -> void {
        auto pr = [&](auto l, auto r) -> void {
            for (int iter = l; iter <= r; ++iter)
                cout << iter + START << ": " << codes[iter] << endl;
        };
        for (const auto &[_l, _r]: block)
            cout << _l << "->" << _r << " ";
        cout << endl;
        for (const auto &[_l, _r] : block) {
            pr(_l, _r);
            cout << endl;
        }
    }

    auto printToFile(const string &filepath) -> void {
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << "没有这个文件" << filepath << endl;
            return;
        }
        auto pr = [&](auto l, auto r) -> void {
            for (int iter = l; iter <= r; ++iter)
                file << iter + START << ": " << codes[iter] << endl;
        };
        int idx = 0;
        for (const auto &[_l, _r] : block) {
            file << "BLOCK " << idx++ << endl;
            pr(_l, _r);
            file << endl;
        }
        file.close();
    }

    auto buildBlock() -> void {
        vector<int> codeSep;

        auto parseSep = [&](const string &line) -> pair<bool, int> {
            auto startWith = [&](const string &str) {
                if (line.size() < str.size()) return false;
                return line.substr(0, str.size()) == str;
            };
            auto tokens = split(line, " ");
            if (startWith("if")) {
                if (tokens[tokens.size() - 2] == "Goto") {
                    int num = atoi(tokens.back().c_str());
                    return {true, num - START};
                }
            }
            if (startWith("Goto"))
                return {true, atoi(tokens.back().c_str()) - START};

            if (startWith("call") || (tokens.size() >= 4 && tokens[2] == "call")) {
                if (std::find(reversed.begin(), reversed.end(), tokens.back()) != reversed.end())
                    return {false, -1}; //配置是否需要拆分
                return {true, funcRecord[tokens.back()].defLine};
            }

            if (startWith("return")) return {true, -1};

            return {false, -1};
        };

        int idx = 0;
        for (const string &code:codes) {
            cout << "parsing " << code << endl;
            pair<bool, int> p = parseSep(code);
            if (p.first) {
                codeSep.push_back(idx + 1);
                if (p.second != -1)
                    codeSep.push_back(p.second);
            }
            idx++;
        }
        codeSep.push_back(0);
        codeSep.push_back(codes.size());
        sort(codeSep.begin(), codeSep.end());
        codeSep.erase(unique(codeSep.begin(), codeSep.end()), codeSep.end());
        block.clear();
        int prev = codeSep.front(), blockNum = 0;
        codeOfBlock.assign(codes.size(), 0);
        for (int i = 1; i < codeSep.size(); ++i) {
            block.push_back({prev, codeSep[i] - 1});
            for (int _l = prev; _l <= codeSep[i] - 1; ++_l)
                codeOfBlock[_l] = blockNum;
            blockNum++;
            prev = codeSep[i];
        }
    }

    vector<functionPrototype> functions;
};

CODE code;

bool startWith(const string &a, const string &b) {
    if (a.size() < b.size())return false;
    return a.substr(0, b.size()) == b;
}

void readCode(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) cerr << "没有这个文件，请检查 " << filepath << endl;
    else {
        string line;
        code.codes.clear();

        while (getline(file, line)) {
            if (line.size() <= 6) continue; //去除空行
            code.codes.push_back(line);
        }
        int lineNum = 0;
        for (auto &codeLine: code.codes) {
            int idx = 0;
            while (codeLine[idx++] != ')');
            codeLine = codeLine.substr(idx + 1);
            if (startWith(codeLine, "Prog")) {
                string name = codeLine.substr(5);
                CODE::functionPrototype f;
                f.functionName = name;
                f.defLine = lineNum;
                code.funcRecord[name] = f;
            }
            ++lineNum;
        }

        cout << "读取文件 " << filepath << "完毕" << endl;
        file.close();
    }
}


template<typename T>
auto printVec(const vector<T> &v) -> void {
    for (const auto &ele:v) cout << ele << endl;
}

int main() {
    string line;
    while (cout << "optimizer> ", getline(cin, line)) {
        if (startWith(line, "load")) {
            readCode(line.substr(5));
        } else if (startWith(line, "split block")) {
            code.buildBlock();
        } else if (startWith(line, "print block")) {
            code.printBlock();
        } else if (startWith(line, "pf")) {
            code.printToFile(line.substr(3));
        } else if (startWith(line, "block")) {
            code.printTheBlock(atoi(line.substr(6).c_str()));
        } else if (startWith(line, "genDag")) {
            code.blockOptimize(atoi(line.substr(7).c_str()));
        }
    }
    return 0;
}