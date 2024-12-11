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

    void Blink(size_t num)
    {
        while (num--)
        {
            this->Blink();
            std::cout << "size: " << this->stones.size() << "\n";
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

    Stones stones{};
    stones.ParseInput(lines);
    std::cout << Stones::ToString(stones) << "\n";
    stones.Blink(25);
    // std::cout << Stones::ToString(stones) << "\n";

    return EXIT_SUCCESS;
}
