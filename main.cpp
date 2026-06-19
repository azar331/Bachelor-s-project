#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#define ll long long
#define pipii pair<int, pair<int, int>>
#define pii pair<int, int>
using namespace std;
const int mod = 998244353;

void solve(vector<vector<int>> &weight, vector<vector<int>>& route, vector<vector<pair<int, int>>> &records, int per,
           int source, int dest, int curtime){


    int n = weight.size() - 1;
    set<pipii> sett;
    vector<int> best_time(n + 1, 1e9);
    vector<pii> parent(n + 1, {-1, -1});

    best_time[source] = curtime;
    parent[source] = {source, 0};
    sett.insert({curtime, {source, -1}});

    while(!sett.empty()){
        auto it = sett.begin();
        pipii cur = *it;
        int tim = cur.first;
        int stopid = cur.second.first;
        int busid = cur.second.second;
        sett.erase(it);
        if (best_time[stopid] < tim){continue;}

        vector<pii> locrec = records[stopid];
        for (pii rec: locrec){
            int otherbusid = rec.first;
            int arrival = rec.second;
            int timetoreach;
            if (arrival >= tim){timetoreach = arrival - tim;}
            else{
                int ceildiv = (tim - arrival + per - 1)/per;
                timetoreach = arrival +  ceildiv*per - tim;
            }
            int nxtstop = -1;
            for (int i = 0; i < route[otherbusid].size(); i++){
                if (route[otherbusid][i] == stopid){
                    if (i == route[otherbusid].size() - 1){
                        nxtstop = route[otherbusid][0];
                    }
                    else{
                        nxtstop = route[otherbusid][i + 1];
                    }
                }
            }
            if (nxtstop == -1){cout << "ERROR0" << '\n'; return;}

            int nextweight = weight[stopid][nxtstop];

            int next_tim = tim + timetoreach + nextweight;

            if (best_time[nxtstop] > next_tim){
                best_time[nxtstop] = next_tim;
                parent[nxtstop] = {stopid, otherbusid};
                sett.insert({next_tim, {nxtstop, otherbusid}});
            }
        }
    }

    if (parent[dest].first == -1){
        cout << "Can not be reached" << '\n';
        return;
    }
    int prev = dest;
    vector<pii> path;
    while (true){
        path.push_back(parent[prev]);
        prev = parent[prev].first;
        if (prev == parent[prev].first){break;}
    }
    reverse(path.begin(), path.end());

    for (pii prr: path){
        cout << "From " << prr.first << ", Using bus number: " << prr.second + 300 << endl;
    }
    cout << "Time of arrival: " << best_time[dest] << endl;
    cout << "Total time spent: " << best_time[dest] - curtime << endl; return;

}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}


bool parse_bus_stop_info (map<string, vector<pair<string, int>>>& bus_stop_info) {
    
    ifstream file("bus_stop_info.csv");
    if (!file.is_open()) {
        cerr << "Error: Could not open file 'bus_stop_info.csv'" << '\n';
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (trim(line).empty()) continue;

        string street, minor_loc, id_str;
        
        size_t first_comma = line.find(',');
        if (first_comma != string::npos) {
            street = trim(line.substr(0, first_comma));
            string remainder = trim(line.substr(first_comma + 1));

            size_t last_comma = remainder.find_last_of(',');
            if (last_comma != string::npos) {
                minor_loc = trim(remainder.substr(0, last_comma));
                id_str = trim(remainder.substr(last_comma + 1));
            } else {
  
                size_t last_space = remainder.find_last_of(" \t");
                if (last_space != string::npos) {
                    minor_loc = trim(remainder.substr(0, last_space));
                    id_str = trim(remainder.substr(last_space + 1));
                }
            }


            if (!id_str.empty()) {
                try {
                    int stop_id = stoi(id_str);
                    bus_stop_info[street].push_back({minor_loc, stop_id});
                } catch (const exception& e) {
                    cerr << "Warning: Could not parse ID '" << id_str << "' on line: " << line << '\n';
                }
            }
        }
    }
    file.close();

    return true;
}


bool parse_stop_graph(vector<vector<int>>& adj) {
    ifstream file("stop_graph.csv");
    
    if (!file.is_open()) {
        cerr << "Error: Could not open file 'stop_graph.csv'\n";
        return false;
    }

    string line;
    while (getline(file, line)) {

        if (line.empty()) continue;

        stringstream ss(line);
        string u_str, v_str, w_str;

        if (getline(ss, u_str, ',') && 
            getline(ss, v_str, ',') && 
            getline(ss, w_str, ',')) {
            
            try {
             
                int u = stoi(u_str);
                int v = stoi(v_str);
                int weight = stoi(w_str);

                int max_node = max(u, v);
                if (max_node >= adj.size()) {
                    int old_size = adj.size();
                    adj.resize(max_node + 1);
                    for (int i = 0; i <= max_node; i++) {
                        adj[i].resize(max_node + 1, 1e9);
                    }
                }

                adj[u][v] = weight;
                
            } catch (const exception& e) {
                cerr << "Warning: Malformed line skipped -> " << line << '\n';
            }
        }
    }

    file.close();
    return true;
}

bool parse_bus_routes(vector<vector<int>>& route_table) {

    const string filename = "bus_routes.csv";

    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open file '" << filename << "'\n";
        return false;
    }

    string line;
    while (getline(file, line)) {
  
        if (line.empty()) continue;

        stringstream ss(line);
        string token;

        if (getline(ss, token, ',')) {
            try {
                int bus_id = stoi(token);

                if (bus_id >= route_table.size()) {
                    route_table.resize(bus_id + 1);
                }

                while (getline(ss, token, ',')) {

                    int stop_id = stoi(token);
                    route_table[bus_id].push_back(stop_id);
                }
                
            } catch (const exception& e) {
                cerr << "Warning: Malformed line skipped -> " << line << '\n';
            }
        }
    }

    file.close();
    return true;
}

bool fill_in_record(vector<vector<pii>>& records, vector<vector<int>> &route, 
vector<vector<int>> &weights){
    int n = route.size();
    for (int i = 0; i < n; i++){
        int tim = 0;
        for (int j = 0; j < route[i].size(); j++){
            int stopid = route[i][j];
            records[stopid].push_back({i, tim});
            if (j + 1 < route[i].size()){
                if (weights[stopid][route[i][j + 1]] == 0){
                    cout << "Error2" << '\n';
                    return false; 
                }
                tim += weights[stopid][route[i][j + 1]];
            }
        }
    }
    return true;
}

int askforstreet(string str,map<string, vector<pair<string, int>>> &bus_stop_info){
    while(true){
    string street;
    cout  << "Please insert " << str <<" streetname: " << endl;
    cin >> street;
    if ( bus_stop_info.count(street) == 0){
        cout << "There's no such street, try again." << endl;
        continue;
    }
    else {
        int num;
        while(true){
            int cnt = 1;
            cout << "Please insert ID for precise location: " << endl;
            for(auto x: bus_stop_info[street]){
                cout << cnt << " " << x.first << endl;
                cnt++;
            }
            
            cin >> num;
            if (!(num >= 1 && num <= bus_stop_info[street].size())){
                continue;
            }
            else{
                break;
            }
        }
        return bus_stop_info[street][num-1].second;
     }
    }
}



int main(void){

vector<vector<int>> weights;
parse_stop_graph(weights);
int n = weights.size() - 1;

map<string, vector<pair<string, int>>> bus_stop_info;
parse_bus_stop_info(bus_stop_info);


vector<vector<int>> route;
parse_bus_routes(route);

vector<vector<pair<int, int>>> records(n + 1);
fill_in_record(records, route, weights);

int per =0;
int source = -1;
int dest = -1;
int curtime;



string line;
while (true) {
    cout << "Enter a period for busses: " << endl;
    getline(cin, line); 

    
    if (line.empty()) {
        cout << "Invalid input! \n";
        continue;
    }

    stringstream ss(line);
    string leftover;

    if (ss >> per && per >= 0 && !(ss >> leftover)) {
        break; 
    }

    cout << "Invalid input! \n";
}


source = askforstreet("Initial", bus_stop_info);
dest = askforstreet("Destination", bus_stop_info);

if (cin.peek() == '\n') {
    cin.ignore();
}
string line2;
while (true) {
    cout << "Enter current time in minutes: " << endl;
    getline(cin, line2); 

    
    if (line2.empty()) {
        cout << "Invalid input! \n";
        continue;
    }

    stringstream ss(line2);
    string leftover;

    if (ss >> curtime && curtime >= 0 && !(ss >> leftover)) {
        break; 
    }

    cout << "Invalid input! \n";
}

solve(weights, route, records, per, source, dest, curtime);

return 0;


}


