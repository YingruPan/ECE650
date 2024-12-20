#include <iostream>
#include <unistd.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>

using namespace std;

int crossProduct(const pair<int, int>& A, const pair<int, int>& B) {
    return A.first * B.second - A.second * B.first;
}

class Segment{
public:
    Segment(const pair<int, int>& startPoint, const pair<int, int>& endPoint) : start(startPoint), end(endPoint) {}
    bool isOverlap(const Segment& other) const {
        // check if collinear
        if (!isCollinear(other)) {
            return false;
        }
        int minA = min(start.first, end.first);
        int maxA = max(start.first, end.first);
        int minB = min(other.start.first, other.end.first);
        int maxB = max(other.start.first, other.end.first);
        if (maxA <= minB || maxB <= minA) {
            return false;
        }
        return true;
    }
    bool isIntersect(const Segment& other) const {
        pair<int, int> v1 = make_pair(other.start.first - start.first, other.start.second - start.second);
        pair<int, int> v2 = make_pair(other.end.first - start.first, other.end.second - start.second);
        pair<int, int> s1 = make_pair(end.first - start.first, end.second - start.second);
        pair<int, int> v3 = make_pair(start.first - other.start.first, start.second - other.start.second);
        pair<int, int> v4 = make_pair(end.first - other.start.first, end.second - other.start.second);
        pair<int, int> s2 = make_pair(other.end.first - other.start.first, other.end.second - other.start.second);
        bool tag1 = (crossProduct(v1, s1) * crossProduct(v2, s1) <= 0);
        bool tag2 = (crossProduct(v3, s2) * crossProduct(v4, s2) <= 0);
        bool tag = (tag1 && tag2);
        return tag;
    }
private:
    pair<int, int> start;
    pair<int, int> end;
    // check if two Segments are collinear
    bool isCollinear(const Segment& other) const {
        // vertical
        if (start.first == end.first && other.start.first == other.end.first) {
            return start.first == other.start.first;
        }
        // horizontal
        if (start.second == end.second && other.start.second == other.end.second) {
            return start.second == other.start.second;
        }
        // commom cases
        return (end.second - start.second) * (other.end.first - other.start.first) ==
               (other.end.second - other.start.second) * (end.first - start.first);
    }
};

class Street{
public:
    Street(const string& name) : streetname(name) {}
    void addCoordinate(int x, int y) {
        coordinates.push_back(make_pair(x, y));
    }
    string getStreetname() const {
        return streetname;
    }
    vector<Segment> getSegments() const {
        vector<Segment> segments;
        for (int i = 0; i < coordinates.size() - 1; i++) {
            segments.push_back(Segment(coordinates[i], coordinates[i + 1]));
        }
        return segments;
    }
    bool isStreetIntersect(const Street& other){
        vector<Segment> mySegments = getSegments();
        vector<Segment> otherSegments = other.getSegments();
        for(int i = 0; i < mySegments.size(); i++){
            for(int j = 0; j < otherSegments.size(); j++){
                if(mySegments[i].isIntersect(otherSegments[j])){
                    return true;
                }
            }
        }
        return false;
    }
private:
    string streetname;
    vector<pair<int, int> > coordinates;
};

int randomNumber(int x, int y) {
    int range = y - x + 1;
    const size_t bytes = 4;
    vector<char> buffer(bytes);
    ifstream urandom("/dev/urandom", ios::in | ios::binary);
    if(!urandom){
        cerr << "Failed to open /dev/urandom" << endl;
        return -1;
    }
    urandom.read(buffer.data(), bytes);
    if(!urandom){
        cerr << "Failed to read from /dev/urandom" << endl;
        return -1;
    }
    unsigned int random_number = 0;
    for(size_t i = 0; i < bytes; ++i){
        random_number = (random_number << 8) | (static_cast<unsigned char>(buffer[i]));
    }
    return x + (random_number % range);
}

bool isQualified(vector<Street> streetList){
    vector<Segment> allSegments;
    vector<vector<Segment> > SegmentsByStreet;
    for(int i = 0; i < streetList.size(); i++){
        vector<Segment> currentStreetSegments = streetList[i].getSegments();
        SegmentsByStreet.push_back(currentStreetSegments);
        allSegments.insert(allSegments.end(), currentStreetSegments.begin(), currentStreetSegments.end());
    }
    for (size_t i = 0; i < allSegments.size(); ++i) {
        for (size_t j = i + 1; j < allSegments.size(); ++j) {
            if (allSegments[i].isOverlap(allSegments[j])) {
                return false;
            }
        }
    }
    for(int i = 0; i < streetList.size(); i++){
        for(int j = i + 1; j < streetList.size(); j++){
            if(streetList[i].isStreetIntersect(streetList[j])){
                return true;
            }
        }
    }
    return false;
}



int main(int argc, char *argv[]){

    int opt;

    // default value of arguments
    int s_value = 10;
    int n_value = 5;
    int l_value = 5;
    int c_value = 20;

    // getopt
    while((opt = getopt(argc, argv, "s:n:l:c:")) != -1){
        char *ch;
        errno = 0;    // reset to 0 to detect error
        strtol(optarg, &ch, 10);
        if(*ch != '\0' || errno != 0){
            cerr << "Error: Input argument should be an integer" << endl;
            return 1;
        }
        switch (opt){
        case 's':
            s_value = atoi(optarg);
            break;
        case 'n':
            n_value = atoi(optarg);
            break;
        case 'l':
            l_value = atoi(optarg);
            break;
        case 'c':
            c_value = atoi(optarg);
            break;
        default:
            break;
        }
    }

    if(s_value < 2 || n_value < 1 || l_value < 5 || c_value < 1){
        cerr << "Error: Invalid arguments" << endl;
        return 1;
    }

    vector<Street> streetList;

    // do every l seconds
    while(true){
        int streetSize = streetList.size();
        if(streetSize != 0){
             for(int i = 0; i < streetSize; i++){
                cout << "rm \"" << streetList[i].getStreetname() << "\"" << endl;
             }
        }
        streetList.clear();
        int s = randomNumber(2, s_value);
        stringstream streetNo, output_to_a1;
        int attempt = 0;

        while(attempt < 25){
            for(int j = 0; j < s; j++){
                int n = randomNumber(1, n_value);
                streetNo << j;
                string streetname_j = "Street" + streetNo.str();
                Street street_j(streetname_j);
                output_to_a1 << "add \"" << streetname_j << "\" ";
                for(int k = 0; k < n + 1; k++){
                    int c1 = randomNumber(-c_value, c_value);
                    int c2 = randomNumber(-c_value, c_value);
                    street_j.addCoordinate(c1, c2);
                    output_to_a1 << "(" << c1 << "," << c2 << ")";
                }
                output_to_a1 << endl;
                streetList.push_back(street_j);
                streetNo.str("");
                streetNo.clear();
            }
            output_to_a1 << "gg" << endl;
            if(isQualified(streetList)){
                cout << output_to_a1.str();
                cout.flush();
                break;
            }
            else{
                streetList.clear();
                attempt ++;
            }
            output_to_a1.str("");
            output_to_a1.clear();
        }
        if(attempt >= 25){
            cerr << "Error: Failed to generate a valid graph after 25 attempts" << endl;
        }

        int l = randomNumber(5, l_value);
        sleep(l);
    }

    return 0;
}