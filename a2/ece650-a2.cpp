// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <cstdio>

using namespace std;

stringstream sserr;

// Define a class of Graph, including addVertex, addEdge and shortestPath operations
class Graph {
public:
    void clearGraph() {
        adjacencyList.clear();
    }
    void clearEdge() {
        int i;
        for (i = 1; i <= adjacencyList.size(); i++) {
            adjacencyList[i].clear();
        }
    }
    // Add a vertex to Graph, and create adjacencyList for it
    // The vertex is named after an integer, and its adjacencyList is initialized with empty
    void addVertex(int vertex) {
        adjacencyList[vertex];
    }
    // Add an edge (u,v) to Graph, completed by adding u and v to each other's adjacencyList
    bool addEdge(int u, int v) {
        // If u or v does not exist in the Graph, output an error
        if (adjacencyList.find(u) == adjacencyList.end() || adjacencyList.find(v) == adjacencyList.end()) {
            if(sserr.str() == ""){
                sserr << "Error: One or both vertices do not exist for edge <" << u << "," << v << ">";
            }
            return false;
        }
        // If duplicated edges input, raise an error
        else if (adjacencyList[u].find(v) != adjacencyList[u].end() || adjacencyList[v].find(u) != adjacencyList[v].end()) {
            if(sserr.str() == ""){
                sserr << "Error: Duplicated edges <" << u << "," << v << "> and <" << v << "," << u << ">";
            }
            return false;
        }
        // Else add u and v to each other's adjacencyList
        else {
            adjacencyList[u].insert(v);
            adjacencyList[v].insert(u);
            return true;
        }
    }
    // Get the shortest path between start and end using BFS
    vector<int> shortestPath(int start, int end) {
        // If start or end does not exist in the vertices of Graph
        if (adjacencyList.find(start) == adjacencyList.end() || adjacencyList.find(end) == adjacencyList.end()) {
            if(sserr.str() == ""){
                sserr << "Error: One or both vertices do not exist";
            }
            return {};
        }
        // Else BFS
        queue<int> q;
        map<int, int> parent;
        set<int> visited;
        q.push(start);
        // Set start as visited
        visited.insert(start);
        while (!q.empty()) {
            // Set first vertex in q as current, then delete it from q
            int current = q.front();
            q.pop();
            // Current equals to end means we have already found the shortest path between start and end
            // Get the shortest path by using reconstructPath function
            if (current == end) {
                return reconstructPath(parent, start, end);
            }
            // Else go through all neighbors of current and set them as visited
            // Set their parent as current
            // Add them to q
            const set<int>& neighbors = adjacencyList[current];
            for (set<int>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
                int neighbor = *it;
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    parent[neighbor] = current;
                    q.push(neighbor);
                }
            }
        }
        // If haven't found the path, return empty vector
        if(sserr.str() == ""){
            sserr << "Error: No path exists between " << start << " and " << end;
        }
        return {};
    }
private:
    map<int, set<int> > adjacencyList;
    // trace back the parents of end till start
    vector<int> reconstructPath(map<int, int>& parent, int start, int end) {
        vector<int> path;
        for (int at = end; at != start; at = parent[at]) {
            path.push_back(at);
        }
        path.push_back(start);
        reverse(path.begin(), path.end());
        return path;
    }
};

int main() {
    Graph graph;
    string line;
    while (getline(cin, line)) {
        // Skip empty line
        if (line.empty()) continue;
        if (line[0] == 'V') {
            graph.clearGraph();
            int vertexCount;
            istringstream(line.substr(2)) >> vertexCount;
                        for (int i = 1; i <= vertexCount; ++i) {
                graph.addVertex(i);
            }
            if (vertexCount < 2) {
                if(sserr.str() == ""){
                    sserr << "Error: vertex count must be at least 2";
                }
                graph.clearGraph();
            }
        } 
        else if (line[0] == 'E') {
            graph.clearEdge();
            // Add each <u,v> to edges of graph
            line = line.substr(2);
            line.erase(0, 1);
            line.erase(line.size() - 1);
            line = line + ',';
            stringstream edgesStream(line);
            char ignore;
            int u, v;
            while (edgesStream >> ignore >> u >> ignore >> v >> ignore >> ignore) {
                if (u == v) {
                    if(sserr.str() == ""){
                        sserr << "Error: Cannot add a self-loop to undirected graph";
                    }
                    graph.clearEdge();
                    break;
                }
                else {
                    if (!graph.addEdge(u, v)) {
                        graph.clearEdge();
                        break;
                    }
                }
            }
        } 
        else if (line[0] == 's') {
            int start, end;
            istringstream(line.substr(2)) >> start >> end;
            if (start == end) {
                if(sserr.str() == ""){
                    sserr << "Error: No self-loop in undirected graph";
                }
            }
            else {
                vector<int> path = graph.shortestPath(start, end);
                if (!path.empty()) {
                    for (size_t i = 0; i < path.size(); ++i) {
                        cout << path[i];
                        if (i < path.size() - 1) cout << "-";
                    }
                    cout << endl;
                }
            } 
            if(sserr.str() != ""){
                string err = sserr.str();
                cerr << err << endl;
                sserr.str("");
            }
        }
    }
    return 0;
}
