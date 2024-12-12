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

template <class T>
int getNumDigits(T number)
{
    int digits = 0;
    if (number < 0) digits = 1; // remove this line if '-' counts as a digit
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

struct Stone
{
    size_t number;

    Stone (const size_t _number) : number(_number){}

    static std::string const ToString(const Stone &stone)
    {
        return std::to_string(stone.number);
    }
    friend std::ostream &operator<<(std::ostream &os, const Stone &stone)
    {
        return os << Stone::ToString(stone);
    }
    friend bool operator==(const Stone &l, const Stone &r)
    {
        return (l.number == r.number);
    }
};

struct Stones
{
    std::vector<Stone> stones;
    std::vector<std::vector<size_t>> lookupTable;
    static constexpr size_t LOOKUP_TABLE_SIZE = 35; // up to 33
    static constexpr size_t LOOKUP_TABLE_LEVELS_SIZE = 100;

    static std::string const ToString(const Stones &stones)
    {
        return "Size: " + std::to_string(stones.stones.size()) + ": " +
                boost::algorithm::join(stones.stones |
                boost::adaptors::transformed([](const Stone &stone)
                {
                    return Stone::ToString(stone);
                }), " ");
    }

    static std::string const ToString(const std::vector<Stone> &stones)
    {
        return "Size: " + std::to_string(stones.size()) + ": " +
                boost::algorithm::join(stones |
                boost::adaptors::transformed([](const Stone &stone)
                {
                    return Stone::ToString(stone);
                }), " ");
    }

    void ParseInput(const std::vector<string> &lines)
    {
        this->CreateLookUpTable();
        this->stones.clear();
        std::stringstream ss(lines[0]);
        size_t num{};
        while (!ss.eof())
        {
            ss >> num;
            this->stones.emplace_back(num);
        }
    }

    size_t GetFirstHalfOfNumber(const size_t number)
    {
        // std::cout << "GetFirstHalfOfNumber\n";
        // std::cout << "GetFirstHalfOfNumber: " <<  std::to_string(number).substr(0, getNumDigits(number)/2) << "\n";
        std::string s = std::to_string(number).substr(0, getNumDigits(number)/2);
        // return std::stol(std::to_string(number).substr(0, getNumDigits(number)/2));
        return std::stol(s);
    }

    size_t GetSecondHalfOfNumber(size_t number)
    {
        // std::cout << "GetSecondHalfOfNumber\n";
        std::string s = std::to_string(number).substr(getNumDigits(number)/2);
        return std::stol(s);
        // return std::stol(std::to_string(number).substr(getNumDigits(number)));
    }

    void Blink()
    {
        std::vector<Stone> newStones;
        for (size_t i=0; i<this->stones.size(); i++)
        {
            if (this->stones[i].number == 0)
            {
                newStones.emplace_back(1);
            }
            else if (getNumDigits(this->stones[i].number) % 2 == 0)
            {
                // std::cout << "cutting: " <<  this->stones[i].number << "\n";
                newStones.emplace_back(this->GetFirstHalfOfNumber(this->stones[i].number));
                newStones.emplace_back(this->GetSecondHalfOfNumber(this->stones[i].number));
            }
            else
            {
                newStones.emplace_back(this->stones[i].number * 2024);
            }
        }
        // std::cout << Stones::ToString(newStones) << "\n";
        this->stones = newStones;
    }

    void BlinkFromLookupTable(size_t num)
    {
        size_t total{};
        std::vector<Stone> newStones;
        // std::cout << Stones::ToString(this->stones) << "\n";

        while (num--)
        {
            newStones.clear();
            for (size_t i=0; i<this->stones.size(); i++)
            {
                if (this->stones[i].number < LOOKUP_TABLE_LEVELS_SIZE)
                {
                    // std::cout << "here1 i: " << i << ", number: " << this->stones[i].number << ", num: " << num
                    //           << ", lookup: " << this->lookupTable[this->stones[i].number][num] << "\n";
                    total = total + this->lookupTable[this->stones[i].number][num];
                }
                else if (getNumDigits(this->stones[i].number) % 2 == 0)
                {
                    // std::cout << "cutting: " <<  this->stones[i].number << "\n";
                    newStones.emplace_back(this->GetFirstHalfOfNumber(this->stones[i].number));
                    newStones.emplace_back(this->GetSecondHalfOfNumber(this->stones[i].number));
                }
                else
                {
                    newStones.emplace_back(this->stones[i].number * 2024);
                }
            }
            this->stones = newStones;
            // std::cout << Stones::ToString(newStones) << "\n";
            std::cout << "blink: total: " << total + newStones.size() << "\n";
        }
        total = total + newStones.size();
        std::cout << "Total: " << total << "\n";
    }

    void Blink(size_t num)
    {
        size_t i=0;
        while (num--)
        {
            this->Blink();
            std::cout << "blink: " << ++i << ", size: " << this->stones.size() << "\n";
            if (num < LOOKUP_TABLE_SIZE)
            {
                this->BlinkFromLookupTable(num);
                break;
            }
        }

    }

    void CreateLookUpTable()
    {
        for (size_t i=0; i<LOOKUP_TABLE_LEVELS_SIZE; i++)
        {
            std::cout << "i: " << i << "\n";
            this->stones = {Stone(i)};
            std::vector<size_t> singleLookUpTable{};
            for (size_t j=0; j<LOOKUP_TABLE_SIZE; j++)
            {
                this->Blink();
                singleLookUpTable.emplace_back(this->stones.size());
            }
            this->lookupTable.push_back(singleLookUpTable);
            singleLookUpTable.clear();
        }
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 55312
    // string filename("example_input2");
    string filename("input"); // 65601038650482 -> TOO LOW
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

    Stones stones{};
    stones.ParseInput(lines);
    std::cout << Stones::ToString(stones) << "\n";

    stones.Blink(75);
    // std::cout << Stones::ToString(stones) << "\n";

    return EXIT_SUCCESS;
}
