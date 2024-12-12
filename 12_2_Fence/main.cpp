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
#include <functional>
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

struct FenceNode
{
    char symbol;
    size_t x;
    size_t y;
    FenceNode *left = nullptr;
    FenceNode *right = nullptr;
    FenceNode *up = nullptr;
    FenceNode *down = nullptr;

    FenceNode (const char _symbol) : symbol(_symbol){}
    FenceNode (const char _symbol, const size_t _x, const size_t _y) : 
                symbol(_symbol), x(_x), y(_y)  {}

    static std::string const ToString(const FenceNode &node)
    {
        return std::string(1, node.symbol);
    }
    friend std::ostream &operator<<(std::ostream &os, const FenceNode &node)
    {
        return os << FenceNode::ToString(node);
    }
    friend bool operator==(const FenceNode &l, const FenceNode &r)
    {
        // std::cout << "comparing " << l.symbol << "==" << r.symbol << "\n";
        return (l.x == r.x) and (l.y == r.y) and (l.symbol == r.symbol);
        // return (l.symbol == r.symbol);
    }
};

struct Fence
{
    std::vector<FenceNode> fence;
};

struct Fences
{
    std::vector<FenceNode> allFenceNodes;
    std::vector<Fence> fences;
    size_t height;
    size_t width;

    void ParseInput(const std::vector<string> &input)
    {
        this->height = input.size();
        this->width = input[0].size();
        std::cout << "height: " << this->height << ", width: " << this->width << "\n";
        size_t x = 0;
        size_t y = 0;
        for (auto const& line : input)
        {
            for (const auto &c : line)
            {
                this->allFenceNodes.emplace_back(c, x++, y);
            }
            x=0;
            y++;
        }
        this->AddNeighbours();
        this->GroupFences2();
    }

    void AddNeighbours()
    {
        for (size_t i=0; i<this->allFenceNodes.size(); i++)
        {
            // → → →
            if (((i+1) < this->allFenceNodes.size()) and ((i+1)%this->width != 0))  this->allFenceNodes[i].right = &this->allFenceNodes[i+1];
            // ← ← ←
            if ((i > 0) and (i % this->width != 0))  this->allFenceNodes[i].left = &this->allFenceNodes[i-1];
            // ↑ ↑ ↑
            if (i > (this->width-1))  this->allFenceNodes[i].up = &this->allFenceNodes[i-this->width];
            // ↓ ↓ ↓
            if ((i+this->width) < this->width * this->height)  this->allFenceNodes[i].down = &this->allFenceNodes[i+this->width];
        }
    }

    void AddNodeToFence(const FenceNode &node, Fence &fence)
    {
        std::cout << "trying to add: " << node.symbol << ", x: " << node.x << ", y: " << node.y << "\n";
        if (fence.fence.cend() != std::find(fence.fence.cbegin(), fence.fence.cend(), node))
        {
            return;
        }

        fence.fence.push_back(node);
        if ( (node.left != nullptr) and ((*node.left).symbol == node.symbol)) this->AddNodeToFence(*(node.left), fence);
        if ( (node.right != nullptr) and ((*node.right).symbol == node.symbol)) this->AddNodeToFence(*(node.right), fence);
        if ( (node.up != nullptr) and ((*node.up).symbol == node.symbol)) this->AddNodeToFence(*(node.up), fence);
        if ( (node.down != nullptr) and ((*node.down).symbol == node.symbol)) this->AddNodeToFence(*(node.down), fence);
    }

    void GroupFences2()
    {
        size_t j=0;
        for (size_t i=0; i<this->allFenceNodes.size(); i++)
        {
            if (std::any_of(this->fences.begin(), this->fences.end(), [currentFenceNode = this->allFenceNodes[i]](Fence &fence)
                            {
                                return fence.fence.cend() != std::find(fence.fence.cbegin(), fence.fence.cend(), currentFenceNode);
                            }))
            {
                continue;
            }
            this->fences.push_back(Fence{});
            std::cout << "New fence: " << this->allFenceNodes[i].symbol << "\n";
            this->AddNodeToFence(this->allFenceNodes[i], this->fences[j++]);
        }
        for (const auto &fence : this->fences)
        {
            std::cout << "Fence of region: " << fence.fence[0].symbol << ", size: " << fence.fence.size() << "\n";
        }
    }

    void CalculatePrice()
    {
        size_t total=0;
        for (size_t i=0; i<this->fences.size(); i++)
        {
            size_t sides=0;
            size_t perimeter=0;
            for (size_t j=0; j<this->fences[i].fence.size(); j++)
            {
                const FenceNode &node = this->fences[i].fence[j];
                if (((node.left != nullptr) and ((*(node.left)).symbol != node.symbol)) or 
                    ((node.x == 0) and (node.left == nullptr))) {perimeter++;}
                if (((node.right != nullptr) and ((*(node.right)).symbol != node.symbol)) or 
                    ((node.x == this->width-1) and (node.right == nullptr))) {perimeter++;}
                if (((node.up != nullptr) and ((*(node.up)).symbol != node.symbol)) or
                    ((node.y == 0) and (node.up == nullptr))) {perimeter++;}
                if (((node.down != nullptr) and ((*(node.down)).symbol != node.symbol)) or
                    ((node.y == this->height-1) and (node.down == nullptr))) {perimeter++;}

                if (((((node.left != nullptr) and ((*(node.left)).symbol != node.symbol)) or 
                      ((node.x == 0) and (node.left == nullptr))) and
                    (((node.up != nullptr) and ((*(node.up)).symbol != node.symbol)) or 
                     ((node.y == 0) and (node.up == nullptr)))) or
                     ((node.left != nullptr) and (node.up != nullptr) and ((*(node.left)).up != nullptr) and
                       ((*(node.left)).symbol == node.symbol) and ((*(node.up)).symbol != node.symbol) and 
                       ((*((*(node.left)).up)).symbol == node.symbol) )) {sides++;}
                if (((((node.left != nullptr) and ((*(node.left)).symbol != node.symbol)) or 
                      ((node.x == 0) and (node.left == nullptr))) and
                    (((node.down != nullptr) and ((*(node.down)).symbol != node.symbol)) or 
                     ((node.y == this->height-1) and (node.down == nullptr)))) or
                     ((node.left != nullptr) and (node.down != nullptr) and ((*(node.left)).down != nullptr) and
                       ((*(node.left)).symbol == node.symbol) and ((*(node.down)).symbol != node.symbol) and 
                       ((*((*(node.left)).down)).symbol == node.symbol) )) {sides++;}
                if (((((node.right != nullptr) and ((*(node.right)).symbol != node.symbol)) or 
                      ((node.x == this->width-1) and (node.right == nullptr))) and
                    (((node.up != nullptr) and ((*(node.up)).symbol != node.symbol)) or 
                     ((node.y == 0) and (node.up == nullptr)))) or
                     ((node.right != nullptr) and (node.up != nullptr) and ((*(node.right)).up != nullptr) and
                       ((*(node.right)).symbol == node.symbol) and ((*(node.up)).symbol != node.symbol) and
                       ((*((*(node.right)).up)).symbol == node.symbol) )) {sides++;}
                if (((((node.right != nullptr) and ((*(node.right)).symbol != node.symbol)) or 
                      ((node.x == this->width-1) and (node.right == nullptr))) and
                    (((node.down != nullptr) and ((*(node.down)).symbol != node.symbol)) or 
                     ((node.y == this->height-1) and (node.down == nullptr)))) or
                     ((node.right != nullptr) and (node.down != nullptr) and ((*(node.right)).down != nullptr) and
                       ((*(node.right)).symbol == node.symbol) and ((*(node.down)).symbol == node.symbol) and
                       ((*((*(node.right)).down)).symbol == node.symbol) )) {sides++;}
                // if ((((node.left != nullptr) and ((*(node.left)).symbol != node.symbol)) or 
                //     ((node.x == 0) and (node.left == nullptr))) and
                //     (((node.down != nullptr) and ((*(node.down)).symbol != node.symbol)) or 
                //     ((node.y == this->height-1) and (node.down == nullptr)))) {sides++;}
                // if ((((node.right != nullptr) and ((*(node.right)).symbol != node.symbol)) or 
                //     ((node.x == this->width-1) and (node.right == nullptr))) and
                //     (((node.up != nullptr) and ((*(node.up)).symbol != node.symbol)) or 
                //     ((node.y == 0) and (node.up == nullptr)))) {sides++;}
                // if ((((node.right != nullptr) and ((*(node.right)).symbol != node.symbol)) or 
                //     ((node.x == this->width-1) and (node.right == nullptr))) and
                //     (((node.down != nullptr) and ((*(node.down)).symbol != node.symbol)) or 
                //     ((node.y == this->height-1) and (node.down == nullptr)))) {sides++;}
            }
            std::cout << "Region: " << this->fences[i].fence[0].symbol << ", area: " << this->fences[i].fence.size() << 
                         ", perimeter: " << perimeter << ", sides: " << sides << "\n";
            total = total +  this->fences[i].fence.size() * sides;
        }

        std::cout << "Total: " << total << "\n";
    }

    // void GroupFences()
    // {
    //     for (size_t i=0; i<this->allFenceNodes.size(); i++)
    //     {
    //         auto it = std::find_if(this->fences.begin(), this->fences.end(), [currentFenceNode = this->allFenceNodes[i]](Fence &fence)
    //         {
    //             return fence.fence.cend() != std::find_if(fence.fence.cbegin(), fence.fence.cend(), [currentFenceNode](const FenceNode &fenceNode)
    //             {
    //                 // std::cout << "comparing: " << currentFenceNode.symbol << "\n";
    //                 return (currentFenceNode.symbol == fenceNode.symbol) and
    //                        ( ( (fenceNode.left != nullptr) and (*(fenceNode.left) == currentFenceNode)) or
    //                          ( (fenceNode.right != nullptr) and (*(fenceNode.right) == currentFenceNode)) or
    //                          ( (fenceNode.up != nullptr) and (*(fenceNode.up) == currentFenceNode)) or
    //                          ( (fenceNode.down != nullptr) and (*(fenceNode.down) == currentFenceNode))); 
    //             });
    //         });
    //         if (this->fences.end() != it)
    //         {
    //             (*it).fence.push_back(this->allFenceNodes[i]);
    //             continue;
    //         }
    //         // std::cout << "pushing new fence: " << this->allFenceNodes[i].symbol << "\n";
    //         this->fences.push_back(Fence{{this->allFenceNodes[i]}});
    //     }
    //     for (const auto &fence : this->fences)
    //     {
    //         std::cout << "Fence of region: " << fence.fence[0].symbol << ", size: " << fence.fence.size() << "\n";
    //     }
    // }

    static std::string const ToString(const std::vector<FenceNode> &nodes)
    {
        return boost::algorithm::join(nodes |
                boost::adaptors::transformed([](const FenceNode &node)
                {
                    return FenceNode::ToString(node) + ((node.right == nullptr) ? "\n" : "");
                }), "");
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    string filename("example_input"); // 1206
    // string filename("input"); 
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

    Fences fences{};
    fences.ParseInput(lines);
    std::cout << Fences::ToString(fences.allFenceNodes) << "\n";
    fences.CalculatePrice();

    return EXIT_SUCCESS;
}
