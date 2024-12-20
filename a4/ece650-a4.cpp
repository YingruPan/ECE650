#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

using namespace std;

struct Edge {
    int u, v;
    Edge(int _u, int _v) : u(_u), v(_v) {}
};

vector<int> findVertexCover(int vertexCount, const vector<Edge>& edges) {
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
                    if (solver.modelValue(vars[i][j]) == Minisat::l_True) {
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

int main(int argc, char **argv) {
    string line;
    int vertexCount = 0;
    while(true){
        vector<Edge> edges;
        getline(cin, line);
        if(line.empty()) break;
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
            while (edgesStream >> ignore >> u >> ignore >> v >> ignore >> ignore){
                edges.push_back(Edge(u - 1, v - 1));
            }
            vector<int> result = findVertexCover(vertexCount, edges);
            int i = 0;
            cout << result[i] + 1;
            for(i = 1; i < result.size(); i++){
                cout << " " << result[i] + 1;
            }
            cout << endl;
        }
    }
    return 0;
}
