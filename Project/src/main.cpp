#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <climits>
#include <random>
#include <chrono>

using namespace std;

vector<vector<char> > board;
vector<vector<char> > board_prev;

int num_mult_per_C;
int R, C, S, D;

// double final_hpwl;

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
    int used_clb;
    int used_mult;
    int need_clb;
    int need_mult;
    double center_x;
    double center_y;
    Floorplan(int id = 0, int x = 0, int y = 0, int w = 0, int h = 0, int used_clb = 0, int used_mult = 0, 
              int need_clb = 0, int need_mult = 0, double center_x = 0.0, double center_y = 0.0) {
        this->module_id = id;
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->used_clb = used_clb;
        this->used_mult = used_mult;
        this->need_clb = need_clb;
        this->need_mult = need_mult;
        this->center_x = x + (w / 2.0); //?? odd or even?
        this->center_y = y + (h / 2.0); //?? odd or even?
    }
    Floorplan(const Floorplan &f1) {
        this->module_id = f1.module_id;
        this->x = f1.x;
        this->y = f1.y;
        this->w = f1.w;
        this->h = f1.h;
        this->used_clb = f1.used_clb;
        this->used_mult = f1.used_mult;
        this->need_clb = f1.need_clb;
        this->need_mult = f1.need_mult;
        this->center_x = f1.x + (f1.w / 2.0); //?? odd or even?
        this->center_y = f1.y + (f1.h / 2.0); //?? odd or even?
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

        if (x + width >= C) {
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
            if (x + width >= C) {
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
            if (x + width >= C) {
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
            if (height_flag && x >= C) {
                attempt_flag = 1; // re enable attempt_flag
            }

            if (x >= C) {
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
        // need_mult * 3 for finding real M
        out_f_plan = Floorplan(modules[k].module_num, x, y, width, height, cnt_clb, cnt_mul, need_clb, need_mult * 3); 
        floor_plan_out.push_back(out_f_plan);

        // print_board(board);
        // assert(cnt_mul >= need_mult * 3);
        // assert(cnt_clb >= need_clb);
        // update x, y
        if (height > max_height)
            max_height = height;
        x += width;
        if (x >= C) {
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
    vector<Floorplan> floor_plan_for_cal;
    for (int i = 0; i < floor_plan.size(); ++i) {
        floor_plan_for_cal.push_back(floor_plan[i]);
    }

    stable_sort(floor_plan_for_cal.begin(), floor_plan_for_cal.end(), 
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
            if (floor_plan_for_cal[module_n - 1].center_x > max_x) {
                max_x = floor_plan_for_cal[module_n - 1].center_x;
            }
            if (floor_plan_for_cal[module_n - 1].center_x < min_x) {
                min_x = floor_plan_for_cal[module_n - 1].center_x;
            }
            if (floor_plan_for_cal[module_n - 1].center_y > max_y) {
                max_y = floor_plan_for_cal[module_n - 1].center_y;
            }
            if (floor_plan_for_cal[module_n - 1].center_y < min_y) {
                min_y = floor_plan_for_cal[module_n - 1].center_y;
            }
            // floor_plan[module_n - 1].center_y
        }
        HPWL += ((max_x - min_x) + (max_y - min_y));
    }
    return HPWL;
}

double optimize(vector<Net> &nets, vector<Floorplan> &floor_plan, double hpwl) {
    // sort according used mult and used clb
    // stable_sort(floor_plan_out.begin(), floor_plan_out.end(), 
    //     [](const Floorplan &a, const Floorplan &b) {
    //         if (a.used_mult == b.used_mult) {
    //             return a.used_clb > b.used_clb;
    //         }
    //         return a.used_mult > b.used_mult;
    // });
    auto start_time_opt = chrono::system_clock::now();

    int swap_count = 0;
    vector<Floorplan> floor_plan_tmp;
    for (int i = 0; i < floor_plan.size(); ++i) {
        floor_plan_tmp.push_back(floor_plan[i]);
    }

    // use swap_count to control
    while (swap_count < 10000) {
        for (int i = 0; i < floor_plan.size(); ++i) {
            // swap once, 
            // => better, then move to next obj. (i.e. ++i)
            // => not better, swap again!
            // floor_plan[i] is the pivot
            // set maximum swap counting value
            // if (swap_count > 1000) {
            //     return final_hpwl;
            // }
            for (int j = 0; j < floor_plan.size(); ++j) {
                // find condition...
                if (j == i) {
                    continue;
                }
                if (floor_plan[j].used_clb >= floor_plan[i].need_clb && floor_plan[j].used_mult >= floor_plan[i].need_mult
                    && floor_plan[i].used_clb >= floor_plan[j].need_clb && floor_plan[i].used_mult >= floor_plan[j].need_mult) {
                    // swap (id, need_clb, need_mult)
                    // int tmp_id = floor_plan_tmp[j].module_id;
                    // int tmp_need_clb = floor_plan_tmp[j].need_clb;
                    // int tmp_need_mult = floor_plan_tmp[j].need_mult;
                    swap(floor_plan_tmp[j].module_id, floor_plan_tmp[i].module_id);
                    swap(floor_plan_tmp[j].need_clb, floor_plan_tmp[i].need_clb);
                    swap(floor_plan_tmp[j].need_mult, floor_plan_tmp[i].need_mult);
                    // Then, calculate HPWL
                    double tmp_hpwl = cal_HPWL(nets, floor_plan_tmp);
                    // find better sol.
                    if (tmp_hpwl < hpwl) {
                        swap(floor_plan[j].module_id, floor_plan[i].module_id);
                        swap(floor_plan[j].need_clb, floor_plan[i].need_clb);
                        swap(floor_plan[j].need_mult, floor_plan[i].need_mult);
                        hpwl = tmp_hpwl;
                    }
                    else {
                        swap(floor_plan_tmp[j].module_id, floor_plan_tmp[i].module_id);
                        swap(floor_plan_tmp[j].need_clb, floor_plan_tmp[i].need_clb);
                        swap(floor_plan_tmp[j].need_mult, floor_plan_tmp[i].need_mult);
                    }
                    swap_count++;
                    
                    // break;
                }
                if (swap_count >= 10000) {
                    return hpwl;
                }
                
            }
        }
        auto end_time_opt = chrono::system_clock::now();
        chrono::duration<double> elapsed_opt = end_time_opt - start_time_opt;
        // cout << "time: " << elapsed.count() << "\n";
        if (elapsed_opt.count() > 60) {
            break;
        }
    }
   
    // for (int i = 0; i < floor_plan_tmp.size(); ++i) {
    //     cout << floor_plan_tmp[i].module_id << " ";
    //     cout << floor_plan_tmp[i].used_clb << " " << floor_plan_tmp[i].need_clb << " ";
    //     cout << floor_plan_tmp[i].used_mult << " " << floor_plan_tmp[i].need_mult << " ";
    //     cout << "\n";
    // }

    return hpwl;
}

int main(int argc, char* argv[]) {

    auto start_time = chrono::system_clock::now();

    string tmp;
    string arch_path, module_path, net_path;
  
    if (argc > 1) { // given testcases
        arch_path = argv[1];
        module_path = argv[2];
        net_path = argv[3];
    }
    // else {
    //     cout << "Please given testcases!\n";
    //     arch_path = benchmark_path + "case1.arch";
    //     module_path = benchmark_path + "case1.module";
    //     net_path = benchmark_path + "case1.net";
    // }
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

    // output initial floorplan
    int ans = solve(modules);
    if (ans != 0) {
        cout << "FAILED\n";
        return -1;
    }
    else {
        cout << "Successfully done!\n";
    }

    vector<Floorplan> floor_plan_final;
    vector<Floorplan> floor_plan_init;
    vector<Floorplan> floor_plan_opt;
    for (int i = 0; i < floor_plan_out.size(); ++i) {
        floor_plan_final.push_back(floor_plan_out[i]);
        floor_plan_init.push_back(floor_plan_out[i]);
        floor_plan_opt.push_back(floor_plan_out[i]);
    }

    double final_hpwl = cal_HPWL(nets, floor_plan_opt); // find initial hpwl
    double init_final_hpwl = final_hpwl; // save init hpwl
    cout << "Initial HPWL: " << init_final_hpwl << "\n";
    cout << "Start optimizing...\n";

    // directly optimize
    final_hpwl = optimize(nets, floor_plan_opt, init_final_hpwl);
    if (final_hpwl < init_final_hpwl) {
        floor_plan_final.clear();
        for (int i = 0; i < floor_plan_opt.size(); ++i) {
            floor_plan_final.push_back(floor_plan_opt[i]);
        }
    }

    // put out back to init
    floor_plan_opt.clear();
    for (int i = 0; i < floor_plan_init.size(); ++i) {
        floor_plan_opt.push_back(floor_plan_init[i]);
    }

    stable_sort(floor_plan_opt.begin(), floor_plan_opt.end(), 
        [](const Floorplan &a, const Floorplan &b) {
            if (a.used_mult == b.used_mult) {
                return a.used_clb > b.used_clb;
            }
            return a.used_mult > b.used_mult;
    });
    
    double final_hpwl_2 = optimize(nets, floor_plan_opt, init_final_hpwl);
    if (final_hpwl_2 < final_hpwl) {
        floor_plan_final.clear();
        for (int i = 0; i < floor_plan_opt.size(); ++i) {
            floor_plan_final.push_back(floor_plan_opt[i]);
        }
    }
    final_hpwl = min(final_hpwl, final_hpwl_2);
    // optimizing my random shuffle
    // the shuffling times is decided by initial hpwl
    int max_shuffle = 1000;
    if (final_hpwl < 100000) {
        max_shuffle = 500;
    }
    // else if (final_hpwl < 500000) {
    //     max_shuffle = 500;
    // }
    // else {
    //     max_shuffle = 200;
    // }
    cout << "Maximum shuffle iter: " << max_shuffle << "\n";

    for (int k = 0; k < max_shuffle; ++k) {
        floor_plan_opt.clear();
        for (int i = 0; i < floor_plan_final.size(); ++i) {
            floor_plan_opt.push_back(floor_plan_final[i]);
        }

        shuffle(floor_plan_opt.begin(), floor_plan_opt.end(), default_random_engine(k));
        final_hpwl_2 = optimize(nets, floor_plan_opt, init_final_hpwl);
        // if (k % 100 == 0)
        //     cout << "Iter: " << k << ", hpwl: " << final_hpwl_2 << "\n";
        if (final_hpwl_2 < final_hpwl) {
            floor_plan_final.clear();
            for (int i = 0; i < floor_plan_opt.size(); ++i) {
                floor_plan_final.push_back(floor_plan_opt[i]);
            }
        }
        final_hpwl = min(final_hpwl, final_hpwl_2);
        
        auto end_time = chrono::system_clock::now();
        chrono::duration<double> elapsed = end_time - start_time;
        // cout << "time: " << elapsed.count() << "\n";
        if (elapsed.count() > 60 * 8) {
            break;
        }
    }

    double check_hpwl = cal_HPWL(nets, floor_plan_final); // check final hpwl again
    assert(check_hpwl == final_hpwl);

    stable_sort(floor_plan_final.begin(), floor_plan_final.end(), 
        [](const Floorplan &a, const Floorplan &b) {
            return a.module_id < b.module_id;
    });

    if ((int)((final_hpwl * 10) - ((int)final_hpwl * 10)) != 0) {
        cout << "Final total HPWL: " << fixed << setprecision(1) << final_hpwl << "\n";
    }
    else {
        cout << "Fianl total HPWL: " << fixed << setprecision(0) << final_hpwl << "\n";
    }

    ofstream f_o;
    if (argc > 1) {
        f_o.open(argv[4]);
    }
    else {
        cout << "Unknown command!" << "\n";
        return -1;
    }

    
    for (int i = 0; i < floor_plan_final.size(); ++i) {
        f_o << floor_plan_final[i].module_id << " ";
        f_o << floor_plan_final[i].x << " " << floor_plan_final[i].y << " " << floor_plan_final[i].w << " " << floor_plan_final[i].h;
        f_o << "\n";
    }
    if ((int)((final_hpwl * 10) - ((int)final_hpwl * 10)) != 0) {
        f_o << fixed << setprecision(1) << final_hpwl << "\n";
    }
    else {
        f_o << fixed << setprecision(0) << final_hpwl << "\n";
    }
    
    f_o.close();
    cout << "Done floorplan!!\n";
    return 0;
}