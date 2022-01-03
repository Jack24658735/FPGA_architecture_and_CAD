#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

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

vector<vector<char> > board;

int main() {

    string benchmark_path = "./Project/benchmarks";
    string tmp;
    ifstream in_arch(benchmark_path + "/case1.arch");
    ifstream in_module(benchmark_path + "/case1.module");
    ifstream in_net(benchmark_path + "/case1.net");
    int cnt = 0;
    // read arch
    vector<int> arch;
    while (getline(in_arch, tmp)) {
        istringstream ss(tmp);
        string t;
        while (ss >> t) {
            arch.push_back(stoi(t));
        }
    }
    int R = arch[0];
    int C = arch[1];
    int S = arch[2];
    int D = arch[3];
    // print(arch);
    board.resize(R);
    for (int i = 0; i < R; ++i) {
        board[i].resize(C);
    }
    // cout << board.size() << "\n";
    // cout << board[0].size() << "\n";

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
    while (getline(in_module, tmp)) {
        // cout << tmp << "\n";
    }
    // read net
    while (getline(in_net, tmp)) {
        // cout << tmp << "\n";
    }
    return 0;
}