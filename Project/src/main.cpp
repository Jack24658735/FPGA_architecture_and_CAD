#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

vector<vector<char> > board;
int num_mult_per_C;
int R, C, S, D;

class Module {
public:
    int module_num;
    int clb;
    int mult;

    Module(int num = 0, int clb = 0, int mult = 0) {
        this->module_num = num;
        this->clb = clb;
        this->mult = mult;
    }
    void set_num(int num) {
        this->module_num = num;
    }
    void set_clb(int clb) {
        this->clb = clb;
    }
    void set_mult(int mult) {
        this->mult = mult;
    }
};

class Net {
public:
    int net_num;
    vector<int> collect_nets;
    Net(int num = 0) {
        this->net_num = num;
    }
    void set_num(int num) {
        this->net_num = num;
    }
};

void print_1d(vector<int> &ans) {
    for (int i = 0; i < ans.size(); ++i) {
        cout << ans[i] << " ";
    }
    cout << "\n";
}

void print_board(vector<vector<char> > &ans) {
    ofstream f;
    f.open("out.txt");
    for (int i = 0; i < ans.size(); ++i) {
        for (int j = 0; j < ans[0].size(); ++j) {
            f << ans[i][j] << " ";
        }
        f << "\n";
    }
    f << "\n";
    f.close();
}

void solve(vector<Module> &modules, vector<Net> &nets) {

    // sorting with multiplier requirements
    sort(modules.begin(), modules.end(), 
        [](const Module &a, const Module &b) {
            if (a.mult == b.mult) {
                return a.clb > b.clb;
            }
            return a.mult > b.mult;
    });

    // modify x, y as usual, and it transforms to x_re, y_re!
    int x = 0, y = 0;
    int y_re = R - 1 - y;
    
    // put on the board? => todo: for multiple modules..

    // for (int i = 0; i < modules.size(); ++i) {
    int need_mult = modules[2].mult;
    int need_clb = modules[2].clb;
    y = y + need_mult * 3;
    y_re = R - 1 - y;
    cout << y << "\n";
    cout << y_re << "\n";

    // y decides the h
    // so, find x with clb num
    x = need_clb / (y + 1);
    // if x > D - 1 => need handle..

    x += S; // remember ++!!
    
    // starting point
    // x -= S;
    cout << x << "\n";
    int cnt_clb = 0, cnt_mul = 0;
    for (int i = R - 1; i > y_re; --i) {
        for (int j = 0; j < x; ++j) {
            if (board[i][j] == 'M') {
                cnt_mul++;
            }
            else if (board[i][j] == 'C') {
                cnt_clb++;
            }
            board[i][j] = 'O';
        }
    }
    print_board(board);
    cout << cnt_clb << " " << cnt_mul << "\n";
    // num of D - 1 cols of CLB??
        
    // }


    // for (int i = 0; i < modules.size(); ++i) {
    //     cout << modules[i].module_num << " " << modules[i].clb << " " << modules[i].mult << "\n";
    // }
}

int main(int argc, char* argv[]) {
    string benchmark_path = "../benchmarks/";
    string tmp;
    string arch_path, module_path, net_path;
  
    if (argc > 1) { // given testcases
        arch_path = benchmark_path + argv[1];
        module_path = benchmark_path + argv[2];
        net_path = benchmark_path + argv[3];
    }
    else {
        // cout << "Please given testcases!\n";
        arch_path = benchmark_path + "case1.arch";
        module_path = benchmark_path + "case1.module";
        net_path = benchmark_path + "case1.net";
    }
    ifstream in_arch(arch_path);
    ifstream in_module(module_path);
    ifstream in_net(net_path);
    // read arch
    vector<int> arch;
    while (getline(in_arch, tmp)) {
        istringstream ss(tmp);
        string t;
        while (ss >> t) {
            arch.push_back(stoi(t));
        }
    }
    R = arch[0];
    C = arch[1];
    S = arch[2];
    D = arch[3];

    // find the number of mult per column
    num_mult_per_C = R / 3;
    cout << "NUM mul per col: " << num_mult_per_C << "\n";

    board.resize(R);
    for (int i = 0; i < R; ++i) {
        board[i].resize(C);
    }

    // cout << board.size() << "\n";
    // cout << board[0].size() << "\n";
    
    /// init FPGA board
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (j - S >= 0 && (j - S) % D == 0) {
                board[i][j] = 'M';
            }
            else {
                board[i][j] = 'C';
            }
        }
    }
    
    // read module
    vector<Module> modules;
    while (getline(in_module, tmp)) {
        istringstream ss(tmp);
        string t;
        int idx = 0;
        // int clb = 0, mult = 0, module_num = 0;
        Module m_in;
        while (ss >> t) {
            if (idx == 0) {
                idx++;
                m_in.set_num(stoi(t));
            }
            else if (idx == 1) {
                m_in.set_clb(stoi(t));
                idx++;
            }
            else if (idx == 2) {
                m_in.set_mult(stoi(t));
                idx = 0;
                modules.push_back(m_in);
            }
        }
    }
    // for (int i = 0; i < modules.size(); ++i) {
    //     cout << modules[i].module_num << " " << modules[i].clb << " " << modules[i].mult << "\n";
    // }

    // read net
    vector<Net> nets;
    while (getline(in_net, tmp)) {
        istringstream ss(tmp);
        string t;
        Net net;
        int flag = 0, idx = 0;
        while (ss >> t) {
            if (idx == 0)
                net.set_num(stoi(t));
            if (t == "{")
                flag = 1;
            else if (t == "}")
                flag = 0;
            
            if (flag && t != "{") {
                net.collect_nets.push_back(stoi(t));
            }
            idx++;
        }
        nets.push_back(net);
    }

    // for (int i = 0; i < nets.size(); ++i) {
    //     cout << nets[i].net_num << " ";
    //     for (int j = 0; j < nets[i].collect_nets.size(); ++j) {
    //         cout << nets[i].collect_nets[j] << " ";
    //     }
    //     cout << "\n";
    // }

    solve(modules, nets);

    // output floorplan?

    
    
    return 0;
}