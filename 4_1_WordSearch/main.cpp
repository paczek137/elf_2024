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

struct NodeLetter
{
    NodeLetter *left = nullptr;
    NodeLetter *right = nullptr;
    NodeLetter *up = nullptr;
    NodeLetter *down = nullptr;
    NodeLetter *leftUp = nullptr;
    NodeLetter *leftDown = nullptr;
    NodeLetter *rightUp = nullptr;
    NodeLetter *rightDown = nullptr;

    char letter;

    NodeLetter (const char l) : letter(l){}
};

struct Letters
{
    std::vector<NodeLetter> letters;
    size_t width{};
    size_t height{};
    std::string check = "MAS";

    void ParseInput(const std::vector<string> &lines)
    {
        for (size_t i=0; i<lines.size(); i++)
        {
            for(size_t j=0; j<lines[i].size(); j++)
            {
                NodeLetter letter = NodeLetter(lines[i][j]);
                this->letters.push_back(letter);
            }
        }
        this->width = lines[0].size();
        this->height = lines.size();
        AddNeighbours();
    }

    void AddNeighbours()
    {
        std::cout << "size: " << this->letters.size() << "\n";
        for (size_t i=0; i<this->letters.size(); i++)
        {
            // → → →
            if (((i+1) < this->letters.size()) and ((i+1)%this->width != 0))  letters[i].right = &letters[i+1];
            // ← ← ←
            if ((i > 0) and (i % this->width != 0))  letters[i].left = &letters[i-1];
            // ↑ ↑ ↑
            if (i > (this->width-1))  letters[i].up = &letters[i-this->width];
            // ↓ ↓ ↓
            if ((i+this->width) < this->width * this->height)  letters[i].down = &letters[i+this->width];
            // std::cout << "edge, i: " << i << ", l: " << letters[i].letter << "\n";
        }
    }
    bool CheckRightChainOfXMAS(const NodeLetter &letter)
    {
        return ((letter.right != nullptr) and (letter.right->letter == 'M') and
                (letter.right->right != nullptr) and (letter.right->right->letter == 'A') and
                (letter.right->right->right != nullptr) and (letter.right->right->right->letter == 'S'));
    }
    bool CheckLeftChainOfXMAS(const NodeLetter &letter)
    {
        return ((letter.left != nullptr) and (letter.left->letter == 'M') and
                (letter.left->left != nullptr) and (letter.left->left->letter == 'A') and
                (letter.left->left->left != nullptr) and (letter.left->left->left->letter == 'S'));
    }
    bool CheckDownChainOfXMAS(const NodeLetter &letter)
    {
        return ((letter.down != nullptr) and (letter.down->letter == 'M') and
                (letter.down->down != nullptr) and (letter.down->down->letter == 'A') and
                (letter.down->down->down != nullptr) and (letter.down->down->down->letter == 'S'));
    }
    bool CheckUpChainOfXMAS(const NodeLetter &letter)
    {
        return ((letter.up != nullptr) and (letter.up->letter == 'M') and
                (letter.up->up != nullptr) and (letter.up->up->letter == 'A') and
                (letter.up->up->up != nullptr) and (letter.up->up->up->letter == 'S'));
    }
    bool CheckDownRightChainOfXMAS(NodeLetter &letter) // ↘
    {
        NodeLetter *next = &letter;
        for (const char &c : this->check)
        {
            if (next->down == nullptr) return false;
            if (next->down->right == nullptr) return false;
            if (next->down->right->letter != c) return false;
            next = next->down->right;
        }
        return true;
    }
    bool CheckDownLeftChainOfXMAS(NodeLetter &letter) // ↙
    {
        NodeLetter *next = &letter;
        for (const char &c : this->check)
        {
            if (next->down == nullptr) return false;
            if (next->down->left == nullptr) return false;
            if (next->down->left->letter != c) return false;
            next = next->down->left;
        }
        return true;
    }
    bool CheckUpRightChainOfXMAS(NodeLetter &letter) // ↗
    {
        NodeLetter *next = &letter;
        for (const char &c : this->check)
        {
            if (next->up == nullptr) return false;
            if (next->up->right == nullptr) return false;
            if (next->up->right->letter != c) return false;
            next = next->up->right;
        }
        return true;
    }
    bool CheckUpLeftChainOfXMAS(NodeLetter &letter) // ↖
    {
        NodeLetter *next = &letter;
        for (const char &c : this->check)
        {
            if (next->up == nullptr) return false;
            if (next->up->left == nullptr) return false;
            if (next->up->left->letter != c) return false;
            next = next->up->left;
        }
        return true;
    }
    void CountXMAS()
    {
        int count = 0;
        for (size_t i=0; i<this->letters.size(); i++)
        {
            if (this->letters[i].letter != 'X') continue;
            if (CheckRightChainOfXMAS(this->letters[i])) count++;
            if (CheckLeftChainOfXMAS(this->letters[i])) count++;
            if (CheckDownChainOfXMAS(this->letters[i])) count++;
            if (CheckUpChainOfXMAS(this->letters[i])) count++;
            if (CheckDownRightChainOfXMAS(this->letters[i])) count++;
            if (CheckDownLeftChainOfXMAS(this->letters[i])) count++;
            if (CheckUpRightChainOfXMAS(this->letters[i])) count++;
            if (CheckUpLeftChainOfXMAS(this->letters[i])) count++;
            
        }
        std::cout << "→ ← ↓ ↑ ↘ ↙ ↗ ↖ " << count << "\n";
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 18
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
        if (not std::isalpha(line[line.size()-1]))
        {
            line.pop_back();    
        }
        lines.push_back(line);
        // input = input + line;
    }

    Letters letters{};
    letters.ParseInput(lines);
    letters.CountXMAS();

    int total = 0;
        

    std::cout << "total: " << total << "\n";

    return EXIT_SUCCESS;
}
