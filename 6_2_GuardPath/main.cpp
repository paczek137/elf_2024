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

    char symbol;
    size_t position;

    NodeMap (const char sym) : symbol(sym){}
    NodeMap (const char sym, const size_t pos) : symbol(sym), position(pos){}
};

struct Map
{
    std::vector<NodeMap> nodes;
    size_t width{};
    size_t height{};

    void Print()
    {
        for (const auto &node : this->nodes)
        {
            if (node.right != nullptr)
            {
                std::cout << node.symbol;
                continue;
            }
            std::cout << node.symbol << "\n";
        }
        std::cout << "Guard at: " << this->GetGuard().position << "\n";
        std::cout << "All Guard positions: " << this->GetAllPositionsOfGuard() << "\n";
    }
    void ParseInput(const std::vector<string> &lines)
    {
        for (size_t i=0; i<lines.size(); i++)
        {
            for(size_t j=0; j<lines[i].size(); j++)
            {
                // NodeMap symbol = NodeMap(lines[i][j]);
                NodeMap symbol = NodeMap(lines[i][j], i*lines.size()+j);
                this->nodes.push_back(symbol);
            }
        }
        this->width = lines[0].size();
        this->height = lines.size();
        AddNeighbours();
    }

    void AddNeighbours()
    {
        std::cout << "size: " << this->nodes.size() << "\n";
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
    NodeMap &GetGuard()
    {
        return *(std::find_if(this->nodes.begin(), this->nodes.end(), [](NodeMap &node)
            {
                return (node.symbol == '^') or 
                       (node.symbol == '<') or
                       (node.symbol == '>') or
                       (node.symbol == 'J');
            }));
    }
    bool isGuardOnMap()
    {
        return std::find_if(this->nodes.cbegin(), this->nodes.cend(), [](const NodeMap &node)
            {
                return (node.symbol == '^') or 
                       (node.symbol == '<') or
                       (node.symbol == '>') or
                       (node.symbol == 'J');
            }) != this->nodes.cend();
    }
    size_t GetAllPositionsOfGuard()
    {
        return std::accumulate(this->nodes.cbegin(), this->nodes.cend(), 0, [](size_t sum, const NodeMap &node)
                                {
                                    return sum + ((node.symbol == 'X') ? 1 : 0);
                                });
    }
    bool AddGuardRoute()
    {
        while (this->isGuardOnMap())
        {
            // std::cout << "found\n";
            // → ← ↓ ↑
            NodeMap &guard = this->GetGuard();
            // std::cout << "guard at: " << guard.position << "\n";
            if (guard.symbol == '^')
            {
                if (guard.up == nullptr)
                {
                    // guard.symbol = 'X';
                    guard.symbol = 'u';
                    continue;
                }
                if (guard.up->symbol == 'u')
                {
                    std::cout << "loop\n";
                    return true;
                }
                if (guard.up->symbol != '#')
                {
                    guard.up->symbol = '^';
                    // guard.symbol = 'X';
                    guard.symbol = 'u';
                }
                else
                {
                    guard.symbol = '>';
                }
            }
            else if (guard.symbol == 'J')
            {
                if (guard.down == nullptr)
                {
                    // guard.symbol = 'X';
                    guard.symbol = 'd';
                    continue;
                }
                if (guard.down->symbol == 'd')
                {
                    std::cout << "loop\n";
                    return true;
                }
                if (guard.down->symbol != '#')
                {
                    guard.down->symbol = 'J';
                    // guard.symbol = 'X';
                    guard.symbol = 'd';
                }
                else
                {
                    guard.symbol = '<';
                }
            }
            else if (guard.symbol == '>')
            {
                if (guard.right == nullptr)
                {
                    // guard.symbol = 'X';
                    guard.symbol = 'r';
                    continue;
                }
                if (guard.right->symbol == 'r')
                {
                    std::cout << "loop\n";
                    return true;
                }
                if (guard.right->symbol != '#')
                {
                    guard.right->symbol = '>';
                    // guard.symbol = 'X';
                    guard.symbol = 'r';
                }
                else
                {
                    guard.symbol = 'J';
                }
            }
            else if (guard.symbol == '<')
            {
                if (guard.left == nullptr)
                {
                    // guard.symbol = 'X';
                    guard.symbol = 'l';
                    continue;
                }
                if (guard.left->symbol == 'l')
                {
                    std::cout << "loop\n";
                    return true;
                }
                if (guard.left->symbol != '#')
                {
                    guard.left->symbol = '<';
                    // guard.symbol = 'X';
                    guard.symbol = 'l';
                }
                else
                {
                    guard.symbol = '^';
                }
            }
        }
        return false;
    }
    void CheckAllOptions()
    {
        int total{};
        std::vector<NodeMap> oldMap = this->nodes;
        for (size_t i=0; i<this->nodes.size(); i++)
        {
            if (this->nodes[i].symbol == '#') continue;
            this->nodes[i].symbol = '#';
            bool loop = this->AddGuardRoute();
            if (loop) total++;
            std::cout << "i: " << i << ", options: " << total << " - " <<
                          100 * i / (this->width * this->height) << "%\n";
            // this->Print();
            // std::cout << "\n";
            
            this->nodes = oldMap;
        }
        std::cout << "All options: " << total << "\n";
    }

};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 6
    // string filename("example_input2");
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
        // if (not std::isalpha(line[line.size()-1]))
        // {
        //     line.pop_back();    
        // }
        lines.push_back(line);
        // input = input + line;
    }

    Map map{};
    map.ParseInput(lines);
    map.Print();

    map.CheckAllOptions();
    // std::cout << "Guard turn\n\n";
    // map.AddGuardRoute();
    // map.Print();

    return EXIT_SUCCESS;
}
