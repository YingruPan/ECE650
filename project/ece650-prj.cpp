#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>   
#include <thread>
#include <mutex>
#include <condition_variable>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <pthread.h>
#include <chrono>
#include <future>
#include <atomic>

using namespace std;

long d1, d2, d3;
int vertexCount = 0;
struct Edge {
    int u, v;
    Edge(int _u, int _v) : u(_u), v(_v) {}
};
vector<Edge> edges;

vector<int> result_vc_0;
vector<int> result_vc_1;
vector<int> result_vc_2;

vector<int> VC_0(int vertexCount, const vector<Edge> edges) {
    vector<int> result;
    int k = 1;
    while (k <= vertexCount) {
        Minisat::Solver solver;
        vector<vector<Minisat::Var> > vars(vertexCount);
        for (int i = 0; i < vertexCount; ++i) {
            for (int j = 0; j < k; ++j) {
                vars[i].push_back(solver.newVar());
            }
        }
        // At least one vertex is the ith vertex in the vertex cover
        for (int j = 0; j < k; ++j) {
            Minisat::vec<Minisat::Lit> clause;
            for (int i = 0; i < vertexCount; ++i) {
                clause.push(Minisat::mkLit(vars[i][j]));
            }
            solver.addClause(clause);
        }
        // No one vertex can appear twice in a vertex cover
        for (int i = 0; i < vertexCount; ++i) {
            for (int p = 0; p < k; ++p) {
                for (int q = p + 1; q < k; ++q) {
                    solver.addClause(~Minisat::mkLit(vars[i][p]), ~Minisat::mkLit(vars[i][q]));
                }
            }
        }
        // No more than one vertex in the jth position of the vertex cover
        for (int j = 0; j < k; ++j) {
            for (int p = 0; p < vertexCount; ++p) {
                for (int q = p + 1; q < vertexCount; ++q) {
                    solver.addClause(~Minisat::mkLit(vars[p][j]), ~Minisat::mkLit(vars[q][j]));
                }
            }
        }
        // Every edge is incident to at least one vertex in the vertex cover
        for (size_t i = 0; i < edges.size(); ++i) {
            const Edge& edge = edges[i];
            Minisat::vec<Minisat::Lit> clause;
            for (int j = 0; j < k; ++j) {
                clause.push(Minisat::mkLit(vars[edge.u][j]));
                clause.push(Minisat::mkLit(vars[edge.v][j]));
            }
            solver.addClause(clause);
        }
        bool res = solver.solve();
        if (res) {
            for (int i = 0; i < vertexCount; ++i) {
                for (int j = 0; j < k; ++j) {
                    if (solver.modelValue(vars[i][j]) == Minisat::lbool((uint8_t)0)){
                        result.push_back(i);
                        break;
                    }
                }
            }
            sort(result.begin(), result.end());
            break;
        }
        ++k;
    }
    return result;
}


vector<int> VC_1(int vertexCount, const vector<Edge>& edges){
    vector<int> result;
    int degree[vertexCount];
    vector<Edge> edgesCopy = edges;
    while(true){
        for(int j = 0; j < vertexCount; j++){
            degree[j] = 0;
        }
        for (size_t i = 0; i < edgesCopy.size(); i++){
            const Edge& edge = edgesCopy[i];
            degree[edge.u] += 1;
            degree[edge.v] += 1;
        }
        int maxIndex = max_element(degree, degree + vertexCount) - degree;
        if(degree[maxIndex] == 0) break;
        for(size_t e = 0; e < edgesCopy.size(); e++){
            if(edgesCopy[e].u == maxIndex || edgesCopy[e].v == maxIndex){
                edgesCopy.erase(edgesCopy.begin() + e);
                e --;
            }
        }
        result.push_back(maxIndex);
    }
    sort(result.begin(), result.end());
    return result;
}


vector<int> VC_2(int vertexCount, const vector<Edge>& edges) {
    vector<int> result;
    vector<Edge> edges_copy = edges;
    srand(static_cast<unsigned int>(time(nullptr))); // use current time to randomly generate
    while(!edges_copy.empty()){
        int random_index = rand() % edges_copy.size();  // generate a integer within [0, edges.size() - 1]
        Edge selected_edge = edges_copy[random_index];  
        result.push_back(selected_edge.u);
        result.push_back(selected_edge.v);
        // delete edges with vertices already been covered
        for(size_t i = 0; i < edges_copy.size(); i ++){
            if(selected_edge.u == edges_copy[i].u || selected_edge.u == edges_copy[i].v ||
                selected_edge.v == edges_copy[i].u || selected_edge.v == edges_copy[i].v){
                edges_copy.erase(edges_copy.begin() + i);
                i --;
            }
        }
    }
    sort(result.begin(), result.end());
    return result;
}


void* VC_0_thread(void* arg) {
    d1 = 0;
    pthread_t thread_id = pthread_self();  // get pthread ID
    clockid_t clock_id;
    pthread_getcpuclockid(thread_id, &clock_id);  // get CPU clock ID
    result_vc_0.clear();
    struct timespec start_time, end_time;
    clock_gettime(clock_id, &start_time);
    result_vc_0 = VC_0(vertexCount, edges);
    clock_gettime(clock_id, &end_time);
    d1 = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000;
    return nullptr;
}


void* VC_1_thread(void* arg) {
    d2 = 0;
    pthread_t thread_id = pthread_self();
    clockid_t clock_id;
    pthread_getcpuclockid(thread_id, &clock_id);
    result_vc_1.clear();
    struct timespec start_time, end_time;
    clock_gettime(clock_id, &start_time);
    result_vc_1 = VC_1(vertexCount, edges);
    clock_gettime(clock_id, &end_time);
    d2 = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000;
    return nullptr;
}


void* VC_2_thread(void* arg) {
    d3 = 0;
    pthread_t thread_id = pthread_self(); 
    clockid_t clock_id;
    pthread_getcpuclockid(thread_id, &clock_id);
    result_vc_2.clear();
    struct timespec start_time, end_time;
    clock_gettime(clock_id, &start_time);
    result_vc_2 = VC_2(vertexCount, edges);
    clock_gettime(clock_id, &end_time);
    d3 = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_nsec - start_time.tv_nsec) / 1000;
    return nullptr;
}

int main(int argc, char **argv) {
    string line;
    while(getline(cin, line)){
        if (line.size() == 0) {     // if nothing was read, go to top of the while to check for eof
            continue;
        }
        if(line[0] == 'V'){
            istringstream(line.substr(2)) >> vertexCount;
        }
        else if(line[0] == 'E'){
            line = line.substr(2);
            line.erase(0, 1);
            line.erase(line.size() - 1);
            line = line + ',';
            stringstream edgesStream(line);
            char ignore;
            int u, v;
            edges.clear();
            while (edgesStream >> ignore >> u >> ignore >> v >> ignore >> ignore){
                edges.push_back(Edge(u - 1, v - 1));
            }
            timespec waitTime;
            clock_gettime(CLOCK_REALTIME,&waitTime);
            waitTime.tv_sec += 10;  //having wait time of 10 sec for this graph cnf-sat solver thread
            // Create threads
            pthread_t t1, t2, t3;
            pthread_create(&t1, nullptr, VC_0_thread, nullptr);
            pthread_create(&t2, nullptr, VC_1_thread, nullptr);
            pthread_create(&t3, nullptr, VC_2_thread, nullptr);
            // ref::https://linux.die.net/man/3/pthread_timedjoin_np
            // Join threads
            int ret = pthread_timedjoin_np(t1,nullptr,&waitTime);
            pthread_join(t2, nullptr);
            pthread_join(t3, nullptr);
            if(ret == ETIMEDOUT){
                //cout << "CNF-SAT-VC timeout" << endl;
                result_vc_0.clear();
            }
            /*else if(ret == 0){
                cout << "CNF-SAT-VC time: " << d1 << " us" << endl;
            }else{
                cout << "Error occurred while joining thread." << endl;
            }
            cout << "APPROX-VC-1 time: " << d2 << " us" << endl;
            cout << "APPROX-VC-2 time: " << d3 << " us" << endl;*/
            //Output the results 
            cout << "CNF-SAT-VC: ";
            if(!result_vc_0.empty()){
                int i1 = 0;
                cout << result_vc_0[i1] + 1;
                for(size_t i1 = 1; i1 < result_vc_0.size(); i1++){
                    cout << "," << result_vc_0[i1] + 1;
                }
                cout << endl;
            }else{
                cout << "timeout" << endl;
            }
            cout << "APPROX-VC-1: ";
            int i2 = 0;
            cout << result_vc_1[i2] + 1;
            for(size_t i2 = 1; i2 < result_vc_1.size(); i2++){
                cout << "," << result_vc_1[i2] + 1;
            }
            cout << endl;
            cout << "APPROX-VC-2: ";
            int i3 = 0;
            cout << result_vc_2[i3] + 1;
            for(size_t i3 = 1; i3 < result_vc_2.size(); i3++){
                cout << "," << result_vc_2[i3] + 1;
            }
            cout << endl;
            
            /*if(ret == 0){
                float r0 = static_cast<float>(result_vc_0.size()) / result_vc_0.size();
                cout << "CNF-SAT-VC approximation ratio: " << r0 << endl;
                float r1 = static_cast<float>(result_vc_1.size()) / result_vc_0.size();
                cout << "APPROX-VC-1 approximation ratio: " << r1 << endl;
                float r2 = static_cast<float>(result_vc_2.size()) / result_vc_0.size();
                cout << "APPROX-VC-2 approximation ratio: " << r2 << endl;
            }*/
        } 
    }
    return 0;
}
