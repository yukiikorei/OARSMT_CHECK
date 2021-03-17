#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <array>
#include <set>
#include <map>



#define INT_32 int
#define INT_32_MAX INT32_MAX

enum corner_type{TERMINAL,NODE,LEFT_UP,LEFT_DOWN,RIGHT_UP,RIGHT_DOWN};
typedef std::array<INT_32,2> point;
typedef std::array<INT_32,2> segment;
typedef std::array<INT_32,3> corner;
typedef std::array<INT_32,4> obstacle; // left-down right-up
typedef std::array<INT_32,2> border;    // tow_location
typedef std::map<INT_32,std::vector<border>> H_border;
typedef std::map<INT_32,std::vector<border>> V_border;

typedef std::map<INT_32,std::vector<INT_32>> Tree_edge;

class format {
public:
    // origin instance data
    std::vector<point>      terminals;
    std::vector<obstacle>   obstacles;
    std::vector<corner>     obstacle_corners;
    INT_32 left_border;
    INT_32 right_border;
    INT_32 down_border;
    INT_32 up_border;
    void load_instance_for_terminals_and_obstacle_corners(std::string file_name);

    // node index to location( from escape segment )
    std::vector<point>    all_nodes;
    void load_nodes_location(std::string file_name);

    // steiner edge
    Tree_edge tree_edge;
    void load_steiner_tree(std::string file_name);

};

void format::load_instance_for_terminals_and_obstacle_corners(std::string file_name) {

    std::string tem;
    std::ifstream input_file(file_name);
    left_border     = INT_32_MAX;
    up_border       = INT_32_MAX;
    right_border    = 0;
    up_border       = 0;

    if(!input_file){
        std::cout<<"error when try to open \""+file_name+"\""<<std::endl;
        exit(1);
    }

    while(getline(input_file,tem)){
        if(tem.size()>1){
            if(tem[0]=='D'&&tem[1]=='D'){
                INT_32 x,y,i;
                sscanf(tem.c_str(),"DD%d%d%d",&i,&x,&y);
                terminals.push_back({x,y});
                if(x<left_border)   left_border     = x;
                if(x>right_border)  right_border    = x;
                if(y<down_border)   down_border     = y;
                if(y>up_border)     up_border       = y;
            }else if(tem[0]=='R'&&tem[1]=='R'){
                INT_32 left,down,right,up;
                sscanf(tem.c_str(),"RR%d%d%d%d",&left,&down,&right,&up);
                obstacles.push_back({left,down,right,up});
                obstacle_corners.push_back({left,up,LEFT_UP});
                obstacle_corners.push_back({left,down,LEFT_DOWN});
                obstacle_corners.push_back({right,up,RIGHT_UP});
                obstacle_corners.push_back({right,down,RIGHT_DOWN});

                if(left<left_border)    left_border    = left;
                if(right>right_border)  right_border   = right;
                if(down<down_border)    down_border    = up;
                if(up>up_border)        up_border      = up;
            }
        }
    }

    input_file.close();

    //std::cout<<terminals.size()<<" "<<obstacle_corners.size()<<std::endl;
}

void format::load_nodes_location(std::string file_name){
    std::string tem;
    std::ifstream input_file(file_name);
    if(!input_file){
        std::cout<<"error when try to open \""+file_name+"\""<<std::endl;
        exit(1);
    }
    int number;
    std::string useless;
    int x;
    int y;
    while(getline(input_file,tem)){
        if(tem.size()>1){
            if(tem[0]=='T'){
                sscanf(tem.c_str(),"T %d %d %d",&number,&x,&y);
                all_nodes.push_back({x,y});
            }else if(tem[0]=='N'){
                sscanf(tem.c_str(),"N %d %d %d",&number,&x,&y);
                all_nodes.push_back({x,y});
            }
        }
    }
}

void format::load_steiner_tree(std::string file_name) {
    std::string tem;
    std::ifstream input_file(file_name);
    if(!input_file){
        std::cout<<"error when try to open \""+file_name+"\""<<std::endl;
        exit(1);
    }
    int begin_point;
    int end_point;
    while(getline(input_file,tem)){
        if(tem.size()>1){
            if(tem[0]=='E'){
                sscanf(tem.c_str(),"E %d%d",&begin_point,&end_point);
                auto i = tree_edge.find(begin_point);
                if(i!=tree_edge.end()){
                    i->second.push_back(end_point);
                } else{
                    tree_edge.insert(std::pair<INT_32,std::vector<INT_32>>(begin_point,{end_point}));
                }
                i = tree_edge.find(end_point);
                if(i!=tree_edge.end()){
                    i->second.push_back(begin_point);
                } else{
                    tree_edge.insert(std::pair<INT_32,std::vector<INT_32>>(end_point,{begin_point}));
                }
            }
        }
    }
}


int main(int argc,char* argv[]) {
    std::cout << "steiner check" << std::endl;
    std::cout << "instance file: " << argv[1]<<std::endl;
    std::cout << "escape g file: " << argv[2]<<std::endl;
    std::cout << "result   file: " << argv[3]<<std::endl;
    std::cout<<"-----------------------------------------"<<std::endl;
    std::cout<<"loading file   ................";
    format f;
    f.load_instance_for_terminals_and_obstacle_corners(argv[1]);
    f.load_nodes_location(argv[2]);
    f.load_steiner_tree(argv[3]);
    std::cout<<"\033[32mDONE\033[0m"<<std::endl;
    std::cout<<"check connect  ................";
    std::set<INT_32>    accessible;
    std::vector<INT_32> list;
    INT_32 current = 0;
    list.push_back(0);
    accessible.insert(0);
    while (current<list.size()){
        auto i = f.tree_edge.find(list[current])->second;
        for (int n : i) {
            if (accessible.insert(n).second){
                list.push_back(n);
            }
        }
        current++;
    }
    INT_32 terminal_count = f.terminals.size();
    for (int i = 0; i < terminal_count; ++i) {
        if(accessible.find(i) == accessible.end()){
            std::cout<<"\033[31mERROR\033[0m"<<std::endl;
            return 0;
        }
    }
    if(accessible.size()!=f.tree_edge.size()) {
        std::cout << "\033[31mERROR\033[0m" << std::endl;
        return 0;
    }
    std::cout<<"\033[32mDONE\033[0m"<<std::endl;

    INT_32 total_length = 0;
    std::cout<<"obstacle avoid ................";
    H_border h_border;
    V_border v_border;
    for(auto o : f.obstacles){
        // h border
        auto i = h_border.find(o[1]);
        if(i == h_border.end()){
            h_border.insert(std::pair<INT_32,std::vector<border>>(o[1],{{o[0],o[2]}}));
        } else{
            i->second.push_back({o[0],o[2]});
        }
        i = h_border.find(o[3]);
        if(i == h_border.end()){
            h_border.insert(std::pair<INT_32,std::vector<border>>(o[3],{{o[0],o[2]}}));
        } else{
            i->second.push_back({o[0],o[2]});
        }

        // v border
        i = v_border.find(o[0]);
        if(i == v_border.end()){
            v_border.insert(std::pair<INT_32,std::vector<border>>(o[0],{{o[1],o[3]}}));
        } else{
            i->second.push_back({o[1],o[3]});
        }
        i = v_border.find(o[2]);
        if(i == v_border.end()){
            v_border.insert(std::pair<INT_32,std::vector<border>>(o[2],{{o[1],o[3]}}));
        } else{
            i->second.push_back({o[1],o[3]});
        }
    }
    for(auto e : f.tree_edge){
        INT_32 begin_point_number;
        begin_point_number = e.first;
        point begin_point = f.all_nodes[begin_point_number];
        point end_point;
        for (INT_32 end_point_number : e.second) {
            if(begin_point_number>end_point_number) continue;
            end_point = f.all_nodes[end_point_number];
            if (begin_point[0]==end_point[0]){
                if(begin_point[1]<end_point[1]){
                    total_length += end_point[1] - begin_point[1];
                    auto iter = h_border.upper_bound(begin_point[1]);
                    while (iter->first<end_point[1]){
                        for (auto b : iter->second) {
                            if(begin_point[0]>b[0]&&begin_point[0]<b[1]){
                                std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                                return 0;
                            }
                        }
                        iter++;
                    }
                }else{
                    total_length += begin_point[1] - end_point[1];
                    auto iter = h_border.upper_bound(end_point[1]);
                    while (iter->first<begin_point[1]){
                        for (auto b : iter->second) {
                            if(begin_point[0]>b[0]&&begin_point[0]<b[1]){
                                std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                                return 0;
                            }
                        }
                        iter++;
                    }
                }
            } else if(begin_point[1] == end_point[1]){

                if(begin_point[0]<end_point[0]){
                    total_length += end_point[0] - begin_point[0];
                    auto iter = v_border.upper_bound(begin_point[0]);
                    while (iter->first<end_point[0]){
                        for (auto b : iter->second) {
                            if(begin_point[1]>b[0]&&begin_point[1]<b[1]){
                                std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                                return 0;
                            }
                        }
                        iter++;
                    }
                }else{
                    total_length += begin_point[0] - end_point[0];
                    auto iter = v_border.upper_bound(end_point[0]);
                    while (iter->first<begin_point[0]){
                        for (auto b : iter->second) {
                            if(begin_point[1]>b[0]&&begin_point[1]<b[1]){
                                std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                                return 0;
                            }
                        }
                        iter++;
                    }
                }
            }else{
                // not a rect line
                std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                return 0;
            }
        }
    }

    std::cout<<"\033[32mDONE\033[0m"<<std::endl;
    std::cout<<"total length: \033[34m"<<total_length<<"\033[0m"<<std::endl;

    return 0;
}




