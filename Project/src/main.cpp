#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iomanip>

using namespace std;

vector<vector<char> > board;
vector<vector<char> > board_prev;

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

class Floorplan {
public:
    int module_id;
    int x;
    int y;
    int w;
    int h;
    double center_x;
    double center_y;
    Floorplan(int id = 0, int x = 0, int y = 0, int w = 0, int h = 0, double center_x = 0.0, double center_y = 0.0) {
        this->module_id = id;
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->center_x = x + (w / 2.0); //?? odd or even?
        this->center_y = y + (h / 2.0); //?? odd or even?
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

vector<Floorplan> floor_plan_out;
Floorplan out_f_plan;

int solve(vector<Module> &modules) {

    // sorting with multiplier requirements
    stable_sort(modules.begin(), modules.end(), 
        [](const Module &a, const Module &b) {
            if (a.mult == b.mult) {
                return a.clb > b.clb;
            }
            return a.mult > b.mult;
    });

    // modify x, y as usual, and it transforms y to y_re!
    int x = 0, y = 0;
    int y_re = R - 1 - y;
    // for (int i = 0; i < modules.size(); ++i) {
    //     cout << modules[i].module_num << " " << modules[i].clb << " " << modules[i].mult << "\n";
    // }
    
    // put on the board? => todo: for multiple modules..
    int max_height = 0;
    int largest_r = 0;

    int attempt_flag = 0;
    int height_flag = 0;
    int height = 0;



    for (int k = 0; k < modules.size(); ++k) {
        // test should it be satisfied?
        int is_satisfied = 1;

        int need_mult = modules[k].mult;
        int need_clb = modules[k].clb;
        // cout << "# of module: " << k << "\n";
        // cout << "module num: " << modules[k].module_num << "\n";
        // cout << "needed M:" << need_mult * 3 << " needed C:" << need_clb << "\n";
        
        if (height_flag == 0) {
            height = need_mult * 3;

            // handle if need_mult == 0 (i.e. only need CLB)
            if (need_mult == 0 && modules[k - 1].mult != 0) {
                // height needs to handle

                // find the largest # of rows
                for (int j = 0; j < board.size(); ++j) {
                    if (board[j][0] == 'O') {
                        largest_r = j;
                        break;
                    }
                }
                height = largest_r;
                
                // move to left top corner
                x = 0;
                y = R - height;
                y_re = R - 1 - y; 
            }
            else if (need_mult == 0) {
                height = largest_r;
            }
        }
        
        
        // cout << "curr x: " << x << " curr y: " << y << "\n";
        // cout << "real y: " << y_re << "\n";
        // cout << "h: " << height << "\n";

        while (attempt_flag) {
            int curr_x, curr_y;
            int loop_flag = 0;
            for (int i = 0; i < C; ++i) {
                for (int j = 0; j < R; ++j) {
                    
                    if (board[j][i] != 'O') {
                        int tmp_j = j;

                        while (board[j][i] != 'O') {
                            j++;
                        }
                        x = i;
                        y = R - j; 
                        y_re = R - 1 - y; 
                        attempt_flag = 0;
                        loop_flag = 1;
                        height = j - tmp_j;
                        break;
                    }
                }
                if (loop_flag)
                    break;
            }
        }
        

        // cal the width
        // TODO:
        int width = need_clb / height;
        int cnt_clb = 0, cnt_mul = 0;


        ////
        // adjust the loop! (init point should be different in other modules!)
        ////

        // cout << "x: " << x << "\n";
        // cout << "y: " << y << "\n";

        if (x + width > C) {
            is_satisfied = 0;
        }
        else {
            for (int i = y_re; i > y_re - height; --i) {
                for (int j = x; j < x + width; ++j) {
                    if (board[i][j] == 'M') {
                        cnt_mul++;
                    }
                    else if (board[i][j] == 'C') {
                        cnt_clb++;
                    }
                    else if (board[i][j] == 'O') {
                        cout << "Overlap!" << "\n";
                        cout << i << " " << j << "\n";
                        return -1;
                    }
                    board[i][j] = 'O';
                }
            }
        }

        
        // if need_mult == 0, just skip this loop
        while (cnt_mul < need_mult * 3 && need_mult != 0) {
            width++;
            if (x + width > C) {
                is_satisfied = 0;
                break;
            }
            for (int i = y_re; i > y_re - height; --i) {
                for (int j = x; j < x + width; ++j) {
                    if (board[i][j] == 'M') {
                        cnt_mul++;
                    }
                    else if (board[i][j] == 'C') {
                        cnt_clb++;
                    }
                    // else if (board[i][j] == 'O') {
                    //     cout << "Overlap!" << "\n";
                    //     break;
                    // }
                    board[i][j] = 'O';
                }
            }
        }
        
        

        while (cnt_clb < need_clb) {
            width++;
            if (x + width > C) {
                is_satisfied = 0;
                break;
            }
            for (int i = y_re; i > y_re - height; --i) {
                for (int j = x; j < x + width; ++j) {
                    if (board[i][j] == 'M') {
                        cnt_mul++;
                    }
                    else if (board[i][j] == 'C') {
                        cnt_clb++;
                    }
                    board[i][j] = 'O';
                }
            }
        }

        if (is_satisfied == 0) {
            for (int i = 0; i < R; ++i) {
                for (int j = 0; j < C; ++j) {
                    board[i][j] = board_prev[i][j];
                }
            }
            k--; // back to prev. module

            x += width;
            if (height > max_height) {
                max_height = height;
            }
            // if height_flag == 1, then test x boundary
            if (height_flag && x > C) {
                attempt_flag = 1; // re enable attempt_flag
            }

            if (x > C) {
                x = 0; 
                // update y, restore max_height
                y += max_height;
                y_re = R - 1 - y;

                // handle case 6 (if from top-left to top-right is not enough..)
                if (y_re < 0) {
                    attempt_flag = 1;
                    height_flag = 1;
                }
                max_height = 0;

            }
            continue; // don't do next part!
        }
        else {
            for (int i = 0; i < R; ++i) {
                for (int j = 0; j < C; ++j) {
                    board_prev[i][j] = board[i][j];
                }
            }
        }

        out_f_plan = Floorplan(modules[k].module_num, x, y, width, height);
        floor_plan_out.push_back(out_f_plan);

        // print_board(board);
        // assert(cnt_mul >= need_mult * 3);
        // assert(cnt_clb >= need_clb);
        // update x, y
        if (height > max_height)
            max_height = height;
        x += width;
        if (x > C) {
            x = 0; 
            // update y, restore max_height
            y += max_height;
            y_re = R - 1 - y;
            max_height = 0;
        }
    }
    return 0;
    
}

double cal_HPWL(vector<Net> &nets, vector<Floorplan> &floor_plan) {
    stable_sort(floor_plan.begin(), floor_plan.end(), 
        [](const Floorplan &a, const Floorplan &b) {
            return a.module_id < b.module_id;
    });
    double HPWL = 0;
    for (int i = 0; i < nets.size(); ++i) {
        // for each net
        double max_x = INT_MIN;
        double max_y = INT_MIN;
        double min_x = INT_MAX;
        double min_y = INT_MAX;
        for (int j = 0; j < nets[i].collect_nets.size(); ++j) {
            int module_n = nets[i].collect_nets[j];
            if (floor_plan[module_n - 1].center_x > max_x) {
                max_x = floor_plan[module_n - 1].center_x;
            }
            if (floor_plan[module_n - 1].center_x < min_x) {
                min_x = floor_plan[module_n - 1].center_x;
            }
            if (floor_plan[module_n - 1].center_y > max_y) {
                max_y = floor_plan[module_n - 1].center_y;
            }
            if (floor_plan[module_n - 1].center_y < min_y) {
                min_y = floor_plan[module_n - 1].center_y;
            }
            // floor_plan[module_n - 1].center_y
        }
        HPWL += ((max_x - min_x) + (max_y - min_y));
    }
    return HPWL;
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
        cout << "Please given testcases!\n";
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

    board.resize(R);
    board_prev.resize(R);
    for (int i = 0; i < R; ++i) {
        board[i].resize(C);
        board_prev[i].resize(C);
    }

    
    /// init FPGA board
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (j - S >= 0 && (j - S) % D == 0) {
                board[i][j] = 'M';
                board_prev[i][j] = 'C';
            }
            else {
                board[i][j] = 'C';
                board_prev[i][j] = 'C';
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

    // output floorplan
    int ans = solve(modules);
    if (ans != 0) {
        cout << "FAILED\n";
    }
    else {
        cout << "Successfully done!\n";
    }

    double hpwl = cal_HPWL(nets, floor_plan_out);
    if ((int)((hpwl * 10) - ((int)hpwl * 10)) != 0) {
        cout << "Total HPWL: " << fixed << setprecision(1) << hpwl << "\n";
    }
    else {
        cout << "Total HPWL: " << fixed << setprecision(0) << hpwl << "\n";
    }

    ofstream f_o;
    if (argc > 1) {
        f_o.open(argv[4]);
    }
    else {
        f_o.open("case1.floorplan");
    }
    for (int i = 0; i < floor_plan_out.size(); ++i) {
        f_o << floor_plan_out[i].module_id << " ";
        f_o << floor_plan_out[i].x << " " << floor_plan_out[i].y << " " << floor_plan_out[i].w << " " << floor_plan_out[i].h;
        f_o << "\n";
    }
    if ((int)((hpwl * 10) - ((int)hpwl * 10)) != 0) {
        f_o << fixed << setprecision(1) << hpwl << "\n";
    }
    else {
        f_o << fixed << setprecision(0) << hpwl << "\n";
    }
    
    f_o.close();
    cout << "Done floorplan!!\n";
    return 0;
}