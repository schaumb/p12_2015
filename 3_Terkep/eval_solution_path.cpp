#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <cstdint>
#include <cmath>

using std::vector;
using std::cerr;
using std::string;
using std::cout;

struct Link {
    int start = INT_MIN;
    int end = INT_MIN;
    double length = NAN;

    Link(int start, int end, double length)
    {
        this->start = std::min(start, end);
        this->end = std::max(start, end);
        this->length = length;
    }

    bool operator==(const Link& other) const
    {
        return start == other.start && end == other.end;
    }

    bool operator!=(const Link& other) const { return !(*this == other); }

    struct less_start_end {
        bool operator()(const Link& lhs, const Link& rhs) const
        {
            return lhs.start < rhs.start || (lhs.start == rhs.start && lhs.end < rhs.end);
        }
    };

    struct less_start_end_length {
        bool operator()(const Link& lhs, const Link& rhs) const
        {
            return lhs.start < rhs.start || (lhs.start == rhs.start && lhs.end < rhs.end) || (lhs.start == rhs.start && lhs.end == rhs.end && lhs.length < rhs.length);
        }
    };
};

inline void check(bool cond, const char* msg)
{
    if (!cond) {
        fprintf(stderr, "[ERROR] %s\n", msg);
        exit(1);
    }
}
inline void check(bool cond, const string& msg) { check(cond, msg.c_str()); }
#define CHECK(Cond, Msg) ((Cond) ? ((void)0) : (check(Cond, Msg)))

template <class... Args>
string stringf(const char* format, Args... args)
{
    char buf[8192];
    sprintf(buf, format, args...);
    return buf;
}

struct Node {
    double lon = NAN;
    double lat = NAN;
};

const double kEarthRadiusMeters = 6372797.560856;
const double kPi = 3.141592653589793238462643383279502884197;
const double kMetersPerLatDeg = (kEarthRadiusMeters * kPi / 180);

double metersPerLonDegAtLat(double lat)
{
    CHECK(-90 <= lat && lat <= 90, stringf("Invalid lat: %f", lat));
    return cos(lat * (kPi / 180)) * (kEarthRadiusMeters * kPi / 180);
}
float metersPerLonDegAtLat(float lat)
{
    CHECK(-90 <= lat && lat <= 90, stringf("Invalid lat: %f", lat));
    return cos(lat * (float)(kPi / 180)) * (float)(kEarthRadiusMeters * kPi / 180);
}
inline double square(double x) { return x * x; }

//approx dist between geocoordinates
double geoDist(const Node& p1, const Node& p2)
{
    //no sphere geometry, just convert degs to meters
    float lonDiff = (float)fabs(p1.lon - p2.lon);
    if (lonDiff > 180)
        lonDiff = 360 - lonDiff;
    float latDiff = (float)fabs(p1.lat - p2.lat);
    return sqrt(square(lonDiff * metersPerLonDegAtLat((float)(p1.lat + p2.lat) / 2)) + square(latDiff * (float)kMetersPerLatDeg));
}

void loadGraph(const string& graphPath, std::vector<Link>& links,
    int& nodeCount, int& linkCount)
{
    cout << "Reading " << graphPath << std::endl;
    std::ifstream file(graphPath);
    CHECK(file.good(), stringf("Can't open %s", graphPath.c_str()));
    nodeCount = INT_MIN;
    linkCount = INT_MIN;

    file >> nodeCount;
    file >> linkCount;

    CHECK(nodeCount > 0, stringf("Invalid node count: %d", nodeCount));
    CHECK(linkCount > 0, stringf("Invalid link count: %d", linkCount));

    vector<Node> nodes(nodeCount);
    for (int i = 0; i != nodeCount; ++i) {
        auto& node = nodes[i];
        file >> node.lat >> node.lon;
        CHECK(-90 <= node.lat && node.lat <= 90, stringf("Invalid latitude: %f", node.lat));
        CHECK(-180 <= node.lon && node.lon <= 180, stringf("Invalid longitude: %f", node.lon));
    }

    links.clear();
    links.reserve(linkCount);

    for (int i = 0; i != linkCount; ++i) {
        int start = INT_MIN, end = INT_MIN;
        double length = NAN;
        file >> start >> end >> length;
        CHECK(start != end, "Link start point = link end point");
        CHECK(0 <= start && start < nodeCount, stringf("Invalid link start node idx: %d", start));
        CHECK(0 <= end && end < nodeCount, stringf("Invalid link end node idx: %d", end));
        //CHECK if straight distance is <= length
        double straight_dist = geoDist(nodes[start], nodes[end]);
        CHECK(0 < length && straight_dist * .9999 < length, stringf("Invalid link length: %f (straight: %f)", length, straight_dist));
        links.push_back(Link(start, end, length));
    }

    // sort the links
    std::sort(links.begin(), links.end(), Link::less_start_end_length());

    // unique the links
    // it's possible that there are more than one link between 2 node
    // in this case we have to use the shortest one
    // so if there are more link for a node-pair in the sorted links vector, then
    // we can keep the shortest one, and we can drop the others

    links.erase(std::unique(links.begin(), links.end()));

    CHECK(file.good(), "Invalid file at the end of reading");
    cout << "Done reading " << graphPath << std::endl;
}

void loadNodeIdxListFile(const string& filePath, const string& fileType,
    const int nodeCount, vector<int>& nodes)
{
    cout << "Reading " << filePath << std::endl;
    std::ifstream file(filePath);

    int node_count = INT_MIN;
    file >> node_count;
    CHECK(0 < node_count, stringf("Invalid node_count: %d", node_count));

    nodes.assign(node_count, INT_MIN);
    for (int i = 0; i != node_count; ++i) {
        file >> nodes[i];
        CHECK(0 <= nodes[i] && nodes[i] < nodeCount, stringf("Invalid node id: %d", nodes[i]));
    }

    cout << "Done reading " << filePath << std::endl;
}

int mainCore(int argc, char* argv[])
{
    string map_file;
    string destinations_file;
    string solution_path_file;

    if (argc == 1) {
        map_file = "map.txt";
        destinations_file = "destinations.txt";
        solution_path_file = "path.txt";
    } else if (argc < 4) {
        cerr << "Usage: eval_solution_path <map_file> "
                "<destinations_file> <solution_path_file>"
             << std::endl;
        return 1;
    } else {
        map_file = argv[1];
        destinations_file = argv[2];
        solution_path_file = argv[3];
    }

    // vars that describes the graph
    std::vector<Link> links;
    int node_count;
    int link_count;

    loadGraph(map_file, links, node_count, link_count);

    vector<int> destination_nodes;
    loadNodeIdxListFile(destinations_file, "input", node_count, destination_nodes);

    // load the output path
    vector<int> solution_path;
    loadNodeIdxListFile(solution_path_file, "output", node_count, solution_path);

    if (solution_path.empty()) {
        cerr << "ERROR: the output path is empty" << std::endl;
        return 1;
    }

    CHECK(solution_path.front() != solution_path.back(), "the first node of the path shouldn't be repeated at the end");

    // CHECK that every destination node can be found in the path
    {
        for (size_t i = 0; i != destination_nodes.size(); ++i) {
            const int node_id = destination_nodes[i];
            auto it = std::find(solution_path.begin(), solution_path.end(), node_id);
            CHECK(it != solution_path.end(), stringf("the input node %d (node id = %d) can't be found in the solution path.", i, node_id));
        }
    }

    //close the path
    solution_path.push_back(solution_path.front());

    // CHECK the continuity of the path and calculate the its length
    double total_length = 0.0;

    auto lb = links.begin();
    auto le = links.end();

    for (size_t i = 1; i != solution_path.size(); ++i) {
        const int prev_node_id = solution_path[i - 1];
        const int curr_node_id = solution_path[i];

        const Link link_to_find(prev_node_id, curr_node_id, 0.0);
        auto it = std::lower_bound(lb, le, link_to_find, Link::less_start_end());

        CHECK(it != le && *it == link_to_find, stringf("There is no link between the #%d and the # node of the path", i - 1, i));

        total_length += it->length;
    }

    std::cout.precision(10);
    std::cout << "OK: total length of the path: " << total_length << " m"
              << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    try {
        return mainCore(argc, argv);
    } catch (const std::exception& ex) {
        cerr << "FATAL: std::exception caught, what(): " << ex.what()
             << std::endl;
        return 2;
    } catch (...) {
        cerr << "FATAL: unknown exception caught" << std::endl;
        return 2;
    }
}

// end
