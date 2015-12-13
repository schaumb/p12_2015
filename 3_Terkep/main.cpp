#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct Routes : std::unordered_map<int, std::unordered_map<int, double>>
{
    std::unordered_map<int, std::unordered_map<int, std::pair<std::deque<int>, double>>> dijsktra;

    inline void set(int start, int end, double val) {
        (*this)[start].emplace(start, val);
        (*this)[end].emplace(end, val);
    }

    inline bool has(int start, int end, double& res) const {
        auto it = this->find(start);
        if(it == this->end()) return false;
        auto it2 = it->second.find(end);
        if(it2 == it->second.end()) return false;
        res = it2->second;
        return true;
    }

    inline bool has_dijsktra(int start, int end, std::deque<int>& vec, double& res) const {
        auto it = dijsktra.find(start);
        if(it == dijsktra.end()) return false;
        auto it2 = it->second.find(end);
        if(it2 == it->second.end()) return false;
        vec = it2->second.first;
        res = it2->second.second;
        return true;
    }

    int getRoute(int start, int end, std::ostream& out) {
        double d;
        if(has(start, end, d)) {
            return {end};
        }
        std::deque<int> result;
        if(has_dijsktra(start, end, result, d)) {
            return result;
        }

        std::unordered_map<int, int> parent;
        std::unordered_map<int, int> back_parent;
        parent.emplace(start, start);
        back_parent.emplace(start, start);

        std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> getNeigh;
        getNeigh.emplace(0, start);
        bool found = false;
        while(getNeigh.size() && !found) {
            auto elem = getNeigh.top();

            getNeigh.pop();

            for( auto& neigbours : (*this)[elem.second] ) {

                if(parent.count(neigbours.first)) {
                    continue;
                }
                parent[neigbours.first] = elem.second;
                back_parent[elem.second] = neigbours.first;
                if(neigbours.first == end) {
                    break;
                    found = true;
                }

                getNeigh.emplace(elem.first + neigbours.second, neigbours.first);
            }
        }
        result.push_front(end);
        while(start != ( end = back_parent[end]) ){
            result.push_front(end);
        }
        for(elem : result) {
            out << elem << "\n";
        }
        return end;
    }
};

struct dev_null {
    inline dev_null& operator*(){ return *this; }
    inline dev_null& operator++(){ return *this; }

    template<class T>
    inline dev_null& operator=(T&&){ return *this; }
};

int main()
{
    std::ifstream map("map.txt");

    std::ifstream destinations("destinations.txt");

    int linkCount, nodeCount, destCount;
    map >> nodeCount >> linkCount;

    std::copy_n(std::istream_iterator<double>(map), nodeCount * 2, dev_null{});

    Routes routes;
    routes.reserve(linkCount);
    int x, y;
    double val;
    for (int i = 0; i != linkCount; ++i) {
        map >> x >> y >> val;
        routes.set(x, y, val);
    }

    destinations >> destCount;

    std::unordered_set<int> indexes;
    indexes.reserve(destCount);
    std::copy_n(std::istream_iterator<int>(destinations), destCount, std::inserter(indexes, indexes.begin()));

    std::ofstream path("path.txt");
    path << std::string(10, ' ') << std::endl;

    int last = *indexes.begin();

    indexes.erase(indexes.begin());

    while(indexes.size()) {
        auto index = *indexes.begin();
        indexes.erase(indexes.begin());

        routes.getRoute(last, index, path);

        last = index;
    }


    path << ss.str();
    return 0;
}
