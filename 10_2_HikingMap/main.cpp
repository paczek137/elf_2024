#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <filesystem>
#include <memory>
#include <map>
#include <set>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/range/adaptor/tokenized.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <chrono>
#include <thread>
#include <compare>
#include <initializer_list>
#include <ctime>
#include <concepts>
#include <regex>
#include <deque>
#include <climits>
#include <thread>

using namespace std;

struct NodeMap
{
    NodeMap *left = nullptr;
    NodeMap *right = nullptr;
    NodeMap *up = nullptr;
    NodeMap *down = nullptr;
    NodeMap *previous = nullptr;

    char height;
    size_t position;

    NodeMap (const char _height) : height(_height){}
    NodeMap (const char _height, const size_t _position) : height(_height), position(_position){}

    static std::string const ToString(const NodeMap &node)
    {
        return std::string(1, node.height);
    }
    friend std::ostream &operator<<(std::ostream &os, const NodeMap &node)
    {
        return os << NodeMap::ToString(node);
    }
    friend bool operator==(const NodeMap &l, const NodeMap &r)
    {
        return (l.position == r.position);
    }
};

struct HikingMap
{
    std::vector<NodeMap> nodes;
    size_t width{};
    size_t height{};

    static std::string const ToString(const std::vector<NodeMap> &nodes)
    {
        return boost::algorithm::join(nodes |
                boost::adaptors::transformed([](const NodeMap &node)
                {
                    return NodeMap::ToString(node) + ((node.right == nullptr) ? "\n" : "");
                }), "");
    }

    void ParseInput(const std::vector<string> &lines)
    {
        this->width = lines[0].size();
        this->height = lines.size();

        for (size_t i=0; i<this->height; i++)
        {
            for(size_t j=0; j<this->width; j++)
            {
                // this->nodes.emplace_back(lines[i][j]);
                this->nodes.emplace_back(lines[i][j], i*this->width + j);
            }
        }
        AddNeighbours();
    }

    void AddNeighbours()
    {
        // std::cout << "size: " << this->nodes.size() << "\n";
        for (size_t i=0; i<this->nodes.size(); i++)
        {
            // → → →
            if (((i+1) < this->nodes.size()) and ((i+1)%this->width != 0))  nodes[i].right = &nodes[i+1];
            // ← ← ←
            if ((i > 0) and (i % this->width != 0))  nodes[i].left = &nodes[i-1];
            // ↑ ↑ ↑
            if (i > (this->width-1))  nodes[i].up = &nodes[i-this->width];
            // ↓ ↓ ↓
            if ((i+this->width) < this->width * this->height)  nodes[i].down = &nodes[i+this->width];
            // std::cout << "edge, i: " << i << ", l: " << nodes[i].symbol << "\n";
        }
    }

    size_t CalculateTrailhead(const NodeMap &node, std::vector<NodeMap> &peaks)
    {
        if (node.height == '9')
        {
            // if (std::find(peaks.cbegin(), peaks.cend(), node) != peaks.cend())
            // {
            //     return 0;    
            // }
            // std::cout << "9 at: " << node.position << "\n";
            peaks.push_back(node);
            return 1;
        }
        return ( ( (node.left != nullptr) and (node.left->height == node.height+1) ) ? this->CalculateTrailhead(*node.left, peaks) : 0 ) +
               ( ( (node.right != nullptr) and (node.right->height == node.height+1) ) ? this->CalculateTrailhead(*node.right, peaks) : 0 ) +
               ( ( (node.up != nullptr) and (node.up->height == node.height+1) ) ? this->CalculateTrailhead(*node.up, peaks) : 0 ) +
               ( ( (node.down != nullptr) and (node.down->height == node.height+1) ) ? this->CalculateTrailhead(*node.down, peaks) : 0 );
    }
    void CalculateAllTrailheads()
    {
        size_t total=0;
        std::vector<NodeMap> peaks{};
        for (size_t i=0; i<this->nodes.size(); i++)
        {
            if(this->nodes[i].height != '0') continue;
            total = total + this->CalculateTrailhead(this->nodes[i], peaks);
            peaks.clear();
            std::cout << "i: " << i << ", total: " << total << "\n";
        }
        std::cout << "All trailheads: " << total << "\n";
    }

};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 81
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }
    
    std::vector<std::string> lines{};

    while (getline(input_file, line, '\n')) 
    {
        lines.push_back(line);
    }

    HikingMap map{};
    map.ParseInput(lines);
    std::cout << HikingMap::ToString(map.nodes) << "\n";
    map.CalculateAllTrailheads();

    return EXIT_SUCCESS;
}
