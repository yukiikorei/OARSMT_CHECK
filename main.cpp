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

class border_cmp_increase{
public:
    bool operator()(const std::array<INT_32,3>& a,const std::array<INT_32,3>& b)const{
        if(a[0]<b[0]) return true;
        else if(a[0]==b[0]){
            if(a[1]<b[1]) return true;
            else if(a[1]==b[1]) return a[2]<b[2];
            else return false;
        }else
            return false;
    }
};
class border_cmp_decrease{
public:
    bool operator()(const std::array<INT_32,3>& a,const std::array<INT_32,3>& b)const{
        if(a[0]>b[0]) return true;
        else if(a[0]==b[0]){
            if(a[1]<b[1]) return true;
            else if(a[1]==b[1]) return a[2]<b[2];
            else return false;
        }else
            return false;
    }
};


typedef std::map<INT_32,std::vector<INT_32>> Tree_edge;
typedef std::map<point,std::vector<point>> Tree_edge_point_version;

class format {
public:
    // origin instance data
    std::vector<point>      terminals;
    std::vector<obstacle>   obstacles;
    std::vector<corner>     obstacle_corners;
    INT_32 left_bound;
    INT_32 right_bound;
    INT_32 down_bound;
    INT_32 up_bound;
    void load_instance_for_terminals_and_obstacle_corners(std::string file_name);

    // node index to location( from escape segment )
    std::vector<point>    all_nodes;
    void load_nodes_location(std::string file_name);

    // steiner edge
    Tree_edge tree_edge;
    void load_steiner_tree(std::string file_name);

    // steiner edge point version
    Tree_edge_point_version tree_epv;
    void load_steiner_tree_location(std::string file_name);
};

void format::load_instance_for_terminals_and_obstacle_corners(std::string file_name) {

    std::string tem;
    std::ifstream input_file(file_name);
    left_bound     = INT_32_MAX;
    up_bound       = INT_32_MAX;
    right_bound    = 0;
    up_bound       = 0;

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
                if(x < left_bound) left_bound     = x;
                if(x > right_bound) right_bound    = x;
                if(y < down_bound) down_bound     = y;
                if(y > up_bound) up_bound       = y;
            }else if(tem[0]=='R'&&tem[1]=='R'){
                INT_32 left,down,right,up;
                sscanf(tem.c_str(),"RR%d%d%d%d",&left,&down,&right,&up);
                obstacles.push_back({left,down,right,up});
                obstacle_corners.push_back({left,up,LEFT_UP});
                obstacle_corners.push_back({left,down,LEFT_DOWN});
                obstacle_corners.push_back({right,up,RIGHT_UP});
                obstacle_corners.push_back({right,down,RIGHT_DOWN});

                if(left < left_bound) left_bound    = left;
                if(right > right_bound) right_bound   = right;
                if(down < down_bound) down_bound    = up;
                if(up > up_bound) up_bound      = up;
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

void format::load_steiner_tree_location(std::string file_name) {
    std::string tem;
    std::ifstream input_file(file_name);
    if(!input_file){
        std::cout<<"error when try to open \""+file_name+"\""<<std::endl;
        exit(1);
    }
    point begin_point;
    point end_point;

    while(getline(input_file,tem)){
        if(tem.size()>1){
            if(tem[0]=='E'){
                sscanf(tem.c_str(),"E %d%d%d%d ",&begin_point[0],&begin_point[1],&end_point[0],&end_point[1]);
                auto i = tree_epv.find(begin_point);
                if(i!=tree_epv.end()){
                    i->second.push_back(end_point);
                } else{
                    tree_epv.insert(std::pair<point ,std::vector<point>>(begin_point,{end_point}));
                }
                i = tree_epv.find(end_point);
                if(i!=tree_epv.end()){
                    i->second.push_back(begin_point);
                } else{
                    tree_epv.insert(std::pair<point ,std::vector<point>>(end_point,{begin_point}));
                }
            }
        }
    }
}



/*
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
*/

int main(int argc,char* argv[]) {
    std::cout << "steiner check" << std::endl;
    std::cout << "instance file: " << argv[1]<<std::endl;
    std::cout << "result   file: " << argv[2]<<std::endl;
    std::cout<<"-----------------------------------------"<<std::endl;
    std::cout<<"loading file   ................";
    format f;
    f.load_instance_for_terminals_and_obstacle_corners(argv[1]);
    f.load_steiner_tree_location(argv[2]);
    std::cout<<"\033[32mDONE\033[0m"<<std::endl;
    std::cout<<"check connect  ................";
    std::set<point>    accessible;
    std::vector<point> list;
    INT_32 current = 0;
    list.push_back(f.terminals[0]);
    accessible.insert(f.terminals[0]);
    while (current<list.size()){
        auto i = f.tree_epv.find(list[current])->second;
        for (point n : i) {
            if (accessible.insert(n).second){
                list.push_back(n);
            }
        }
        current++;
    }
    for (point p : f.terminals) {
        if(accessible.find(p) == accessible.end()){
            std::cout<<"\033[31mERROR\033[0m"<<std::endl;
            return 0;
        }
    }
    if(accessible.size()!=f.tree_epv.size()) {
        std::cout << "\033[31mERROR\033[0m" << std::endl;
        return 0;
    }
    std::cout<<"\033[32mDONE\033[0m"<<std::endl;


    INT_32 total_length = 0;
    std::cout<<"obstacle avoid ................";

    std::set<std::array<INT_32,3>,border_cmp_decrease> up_border;
    std::set<std::array<INT_32,3>,border_cmp_increase> down_border;
    std::set<std::array<INT_32,3>,border_cmp_increase> left_border;
    std::set<std::array<INT_32,3>,border_cmp_decrease> right_border;

    std::set<point> have_been_to;
    for(auto o : f.obstacles){
        up_border.insert({o[3],o[0],o[2]});
        down_border.insert({o[1],o[0],o[2]});
        left_border.insert({o[0],o[1],o[3]});
        right_border.insert({o[2],o[1],o[3]});
    }
    for(auto e : f.tree_epv){
        point begin_point = e.first;
        have_been_to.insert(begin_point);
        for (point end_point : e.second) {
            if(have_been_to.find(end_point) != have_been_to.end()) continue;
            if (begin_point[0]==end_point[0]){
                INT_32 in_x   = begin_point[0];
                INT_32 from_y = begin_point[1];
                INT_32 to_y   = end_point[1];
                if(from_y>to_y){
                    from_y = end_point[1];
                    to_y   = begin_point[1];
                }
                auto down_border_i = down_border.lower_bound({from_y,0,0});
                while (down_border_i!=down_border.end()){
                    if((*down_border_i)[0]>=to_y)
                        break;
                    if((*down_border_i)[1]<in_x&&in_x<(*down_border_i)[2]){
                        std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                        return 0;
                    }
                    down_border_i++;
                }
                auto up_border_i = up_border.lower_bound({to_y,0,0});
                while (up_border_i!=up_border.end()){
                    if((*up_border_i)[0]<=from_y)
                        break;
                    if((*up_border_i)[1]<in_x&&in_x<(*up_border_i)[2]){
                        std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                        return 0;
                    }
                    up_border_i++;
                }
                total_length+=to_y-from_y;
            } else if(begin_point[1] == end_point[1]){
                INT_32 in_y   = begin_point[1];
                INT_32 from_x = begin_point[0];
                INT_32 to_x   = end_point[0];
                if(from_x>to_x){
                    from_x = end_point[1];
                    to_x   = begin_point[1];
                }
                auto left_border_i = left_border.lower_bound({from_x,0,0});
                while (left_border_i!=left_border.end()){
                    if((*left_border_i)[0]>=to_x)
                        break;
                    if((*left_border_i)[1]<in_y&&in_y<(*left_border_i)[2]){
                        std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                        return 0;
                    }
                    left_border_i++;
                }
                auto right_border_i = right_border.lower_bound({to_x,0,0});
                while (right_border_i!=right_border.end()){
                    if((*right_border_i)[0]<=from_x)
                        break;
                    if((*right_border_i)[1]<in_y&&in_y<(*right_border_i)[2]){
                        std::cout<<"\033[31mERROR\033[0m"<<std::endl;
                        return 0;
                    }
                    right_border_i++;
                }
                total_length+=to_x-from_x;
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


