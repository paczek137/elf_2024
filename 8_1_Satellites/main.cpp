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

struct Satellite
{
    char frequency;
    size_t x;
    size_t y;

    Satellite(const char _frequency, const size_t _x, const size_t _y) :
        frequency(_frequency), x(_x), y(_y) {}

    static std::string const ToString(const Satellite &sat)
    {
        return std::string(1, sat.frequency);
    }
    friend std::ostream &operator<<(std::ostream &os, const Satellite &sat)
    {
        return os << Satellite::ToString(sat);
    }
};

struct Satellites
{
    std::vector<Satellite> satellites;
    std::map<char, std::vector<Satellite*>> mapOfFrequencies;
    std::vector<std::pair<Satellite*, Satellite*>> pairsOfSatellites;
    size_t height;
    size_t width;
    size_t antinodesCount;

    Satellite& GetSatellite(const size_t x, const size_t y)
    {
        return *(std::find_if(this->satellites.begin(), this->satellites.end(), [x, y] (Satellite &sat)
        {
            return (x == sat.x) and (y == sat.y);
        }));
    }

    void PrintMapOfFrequencies()
    {
        for (const auto &el : this->mapOfFrequencies)
        {
            std::cout << el.first << ": ";
            for (const auto &s : el.second)
            {
                std::cout << "(" << s->x << ", " << s->y << "), ";
            }
            std::cout << "\n";
        }
    }

    static std::string const ToString(const Satellites &sat)
    {
        return boost::algorithm::join(sat.satellites |
                boost::adaptors::transformed([sat](const Satellite &satellite)
                {
                    return Satellite::ToString(satellite) + ((satellite.x < sat.width-1) ? "" : "\n");
                }), "");
    }

    friend std::ostream &operator<<(std::ostream &os, const Satellites &sat)
    {
        return os << Satellites::ToString(sat);
    }

    void ParseInput(const std::vector<std::string> &input)
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
                this->satellites.emplace_back(c, x++, y);
            }
            x=0;
            y++;
        }
    }
    void FillMapOfFrequencies()
    {
        for (size_t i=0; i<this->satellites.size(); i++)
        {
            if (this->satellites[i].frequency == '.')
            {
                continue;
            }
            this->mapOfFrequencies[this->satellites[i].frequency].push_back(&this->satellites[i]);
        }
    }

    void FillOffPairsOfSatellites()
    {
        for (const auto &el : this->mapOfFrequencies)
        {
            for (size_t i=0; i<el.second.size(); i++)
            {
                std::cout << "i: " << i << "\n";
                for (size_t j=i+1; j<el.second.size(); j++)
                {
                    this->pairsOfSatellites.push_back({el.second[i], el.second[j]});
                }
            }
        }
    }
    bool isAlreadyInList(const std::vector<Satellite*> &vec, const size_t x, const size_t y)
    {
        return std::find_if(vec.cbegin(), vec.cend(), [x, y] (const Satellite *pair)
            {
                return (pair->x == x) and (pair->y == y);
            }) != vec.cend();
    }
    void AddAntinodes()
    {
        std::cout << "Adding antinodes...\n";
        char freq = this->pairsOfSatellites[0].first->frequency;
        std::vector<Satellite*> listOfUniqueAntinodes;
        for (const auto &el : this->pairsOfSatellites)
        {
            int x1{};
            int x2{};
            int y1{};
            int y2{};
            if (freq != el.first->frequency)
            {
                listOfUniqueAntinodes.clear();
                freq = el.first->frequency;
            }
            size_t x_diff = static_cast<size_t>(std::abs(static_cast<int>(el.first->x - el.second->x)));
            size_t y_diff = static_cast<size_t>(std::abs(static_cast<int>(el.first->y - el.second->y)));
            std::cout << "P1: (" << el.first->x << ", " << el.first->y << "), P2: (" 
                      << el.second->x << ", " << el.second->y << ")\n";
            std::cout << "x_diff: " << x_diff << ", y_diff: " << y_diff << "\n";
            if (el.first->x > el.second->x)
            {
                x1 = el.first->x + x_diff ;
                x2 = el.second->x - x_diff;
            }
            else
            {
                x1 = el.first->x - x_diff;
                x2 = el.second->x + x_diff;
            }

            y1 = el.first->y - y_diff;
            y2 = el.second->y + y_diff;

            if ( (x1 < this->width) and (x1 >= 0) and
                 (y1 < this->height) and (y1 >= 0) )
                 // (not this->isAlreadyInList(listOfUniqueAntinodes, x1, y1)))
            {
                std::cout << "F: " << el.first->frequency << " Adding in P: (" << x1 << ", " << y1 << ")\n";
                if ( (this->GetSatellite(static_cast<size_t>(x1), static_cast<size_t>(y1)).frequency == '.') or
                     (this->GetSatellite(static_cast<size_t>(x1), static_cast<size_t>(y1)).frequency == '#'))
                {
                    this->GetSatellite(static_cast<size_t>(x1), static_cast<size_t>(y1)).frequency = '#';
                    this->antinodesCount++;
                    listOfUniqueAntinodes.push_back(&(this->GetSatellite(static_cast<size_t>(x1), static_cast<size_t>(y1))));
                }
                
            }
            if ( (x2 < this->width) and (x2 >= 0) and
                 (y2 < this->height) and (y2 >= 0))
                 // (not this->isAlreadyInList(listOfUniqueAntinodes, x2, y2)))
            {
                std::cout << "F: " << el.first->frequency << " Adding in P: (" << x2 << ", " << y2 << ")\n";
                if ( (this->GetSatellite(static_cast<size_t>(x2), static_cast<size_t>(y2)).frequency == '.') or
                     (this->GetSatellite(static_cast<size_t>(x2), static_cast<size_t>(y2)).frequency == '#'))
                {
                    this->GetSatellite(static_cast<size_t>(x2), static_cast<size_t>(y2)).frequency = '#';
                    this->antinodesCount++;
                    listOfUniqueAntinodes.push_back(&(this->GetSatellite(static_cast<size_t>(x2), static_cast<size_t>(y2))));
                }
            }   
        }
    }
    int GetAllAntinodes()
    {
        return std::accumulate(this->satellites.cbegin(), this->satellites.cend(), 0, [](int sum, const Satellite &sat)
                                {
                                    return sum + ((sat.frequency == '#') ? 1 : 0);
                                });
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 14
    string filename("input"); // 276
    // All antinodes: 263 // too low
    // All antinodes: 286 // too high
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

    Satellites sat{};
    sat.ParseInput(lines);
    std::cout << sat << "\n";
    sat.FillMapOfFrequencies();
    sat.PrintMapOfFrequencies();
    sat.FillOffPairsOfSatellites();
    sat.AddAntinodes();
    std::cout << sat << "\n";
    std::cout << "All antinodes: " << sat.GetAllAntinodes() << "\n";
    std::cout << "All antinodes: " << sat.antinodesCount << "\n";


    return EXIT_SUCCESS;
}
