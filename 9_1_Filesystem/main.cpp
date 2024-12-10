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

struct Block
{
    size_t id;
    bool isEmpty;

    Block(const size_t _id, const bool _isEmpty) :
        id(_id), isEmpty(_isEmpty) {}

    static std::string const ToString(const Block &block)
    {
        return block.isEmpty ? std::string(1, '.') : std::string(1, std::to_string(block.id)[0]);
    }
    friend std::ostream &operator<<(std::ostream &os, const Block &block)
    {
        return os << Block::ToString(block);
    }
};

struct Block2
{
    size_t id;
    size_t size;
    bool isEmpty;

    Block2(const size_t _id, const size_t _size, const bool _isEmpty) :
        id(_id), size(_size), isEmpty(_isEmpty) {}

    static std::string const ToString(const Block2 &block)
    {
        return block.isEmpty ? std::string(block.size, '.') : std::string(block.size, std::to_string(block.id)[0]);
        // return std::string(1, sat.frequency);
    }
    friend std::ostream &operator<<(std::ostream &os, const Block2 &block)
    {
        return os << Block2::ToString(block);
    }
};

struct Filesystem
{
    std::vector<Block> blocks;
    std::vector<Block2> blocks2;
    std::vector<Block> blocksSorted;
    size_t usedBlocks;

    void ParseInput(const std::vector<std::string> &input)
    {
        size_t i=0;
        size_t id=0;
        for (const auto &c : input[0])
        {
            size_t s=0;
            while (s < static_cast<size_t>(c - '0'))
            {
                this->blocks.emplace_back(id, static_cast<bool>(i%2));
                s++;
                if (not (i%2)) this->usedBlocks++;
            }
            i++;
            if(i%2) id++;
        }
        std::cout << "usedBlocks: " << this->usedBlocks << "\n";
    }

    void ParseInput2(const std::vector<std::string> &input)
    {
        size_t i=0;
        size_t id=0;
        for (const auto &c : input[0])
        {
            this->blocks2.emplace_back(id, static_cast<size_t>(c - '0'), static_cast<bool>(i%2));
            i++;
            if(i%2) id++;
        }
    }

    void SortBlocks()
    {
        size_t j=this->blocks.size()-1;
        for (size_t i=0; i<this->blocks.size() and i<this->usedBlocks; i++)
        {
            if (not this->blocks[i].isEmpty)
            {
                // std::cout << "adding1 " << this->blocks[i].id << "\n";
                this->blocksSorted.emplace_back(this->blocks[i].id, false);
            }
            else
            {
                while(this->blocks[j].isEmpty)
                {
                    if (i==j) break;
                    j--;
                }

                if (i==j) break;
                // std::cout << "adding2 " << this->blocks[j].id << "\n";
                this->blocksSorted.emplace_back(this->blocks[j].id, false);
                j--;
            }
        }
    }

    size_t GetChecksum()
    {
        size_t res=0;
        for (size_t i=0; i<this->blocksSorted.size(); i++)
        {
            res = res + this->blocksSorted[i].id * i;
        }
        return res;
    }

    static std::string const ToString(const Filesystem &fs)
    {
        return boost::algorithm::join(fs.blocks |
                boost::adaptors::transformed([](const Block &block)
                {
                    return Block::ToString(block);
                }), "");
    }

    static std::string const ToStringSorted(const Filesystem &fs)
    {
        return boost::algorithm::join(fs.blocksSorted |
                boost::adaptors::transformed([](const Block &block)
                {
                    return Block::ToString(block);
                }), "");
    }

    friend std::ostream &operator<<(std::ostream &os, const Filesystem &fs)
    {
        return os << Filesystem::ToString(fs);
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 1928
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

    Filesystem filesystem{};
    filesystem.ParseInput(lines);
    std::cout << filesystem << "\n";
    filesystem.SortBlocks();
    std::cout << Filesystem::ToStringSorted(filesystem) << "\n";
    std::cout << "checksum: " << filesystem.GetChecksum() << "\n";


    return EXIT_SUCCESS;
}
