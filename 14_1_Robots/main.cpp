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
#include <limits>

using namespace std;

struct Robot
{
    ssize_t x;
    ssize_t y;
    ssize_t vx;
    ssize_t vy;

    Robot()= default;
    // Robot (const size_t _x, const size_t _y, const ssize_t _vx, const ssize_t _vy) : 
    //             x(_x), y(_y), vx(_vx), vy(_vy)  {}

    static std::string const ToString(const Robot &robot)
    {
        return std::string("p=") + std::to_string(robot.x) + std::string(",") + std::to_string(robot.y) +
                std::string(" v=") + std::to_string(robot.vx) + std::string(",") + std::to_string(robot.vy);
    }
    friend std::ostream &operator<<(std::ostream &os, const Robot &robot)
    {
        return os << Robot::ToString(robot);
    }
};

struct Tile
{
    char symbol='.';
    size_t x;
    size_t y;
    size_t numberOfRobots=0;

    Tile(const size_t _x, const size_t _y) : x(_x), y(_y) {}

    void AddRobot()
    {
        this->numberOfRobots++;
    }

    void RemoveRobot()
    {
        this->numberOfRobots--;
    }

    static std::string const ToString(const Tile &tile)
    {
        return (tile.numberOfRobots == 0) ? std::string(1, tile.symbol) : 
               (tile.numberOfRobots < 10) ? std::to_string(tile.numberOfRobots) : std::string(1, 'X');
    }
    friend std::ostream &operator<<(std::ostream &os, const Tile &tile)
    {
        return os << Tile::ToString(tile);
    }
};

struct Area
{
    std::vector<Tile> tiles;
    std::vector<Robot> robots;
    ssize_t width;
    ssize_t height;

    Area () = default;
    Area (const size_t _width, const size_t _height) : width(_width), height(_height)
    {
        for (size_t y=0; y<this->height; y++)
        {
            for (size_t x=0; x<this->width; x++)
            {
                tiles.emplace_back(x, y);
            }
        }
    }

    void ParseInput(const std::vector<string> &input)
    {
        for (const auto& line : input)
        {
            Robot robot{};
            std::stringstream ss{};

            ss << line.substr(line.find("=") + 1);
            ss >> robot.x;
            ss.str(line.substr(line.find(",") + 1));
            ss >> robot.y;
            ss.str(line.substr(line.rfind("=") + 1));
            ss >> robot.vx;
            ss.str(line.substr(line.rfind(",") + 1));
            ss >> robot.vy;
            this->robots.push_back(robot);
        }
    }

    Tile &GetTile(const size_t x, const size_t y)
    {
        return *std::find_if(this->tiles.begin(), this->tiles.end(), [x,y](Tile &tile)
                {
                    return (x == tile.x) and (y == tile.y);
                });
    }

    void MoveRobots()
    {
        for (Robot &robot : this->robots)
        {
            this->GetTile(robot.x, robot.y).RemoveRobot();
            if ((robot.x + robot.vx) < 0)
            {
                robot.x = (((robot.x + robot.vx) % this->width) + this->width) % this->width;    
            }
            else
            {
                robot.x = (robot.x + robot.vx) % this->width;
            }
            if ((robot.y + robot.vy) < 0)
            {
                // std::cout << "MINUS\n";
                // std::cout << robot.y << "%" << robot.vy << "\n";
                robot.y = (((robot.y + robot.vy) % this->height) + this->height) % this->height;    
            }
            else
            {
                robot.y = (robot.y + robot.vy) % this->height;
            }
            
            this->GetTile(robot.x, robot.y).AddRobot();
        }
    }

    void MoveRobots(size_t seconds)
    {
        while (seconds--)
        {
            this->MoveRobots();
            // std::cout << Area::ToString(this->tiles) << "\n\n";
        }
    }

    void InitializeRobots()
    {
        for (Robot &robot : this->robots)
        {
            this->GetTile(robot.x, robot.y).AddRobot();
        }
    }

    void CalculateSafetyFactor()
    {
        size_t factor1=0;
        size_t factor2=0;
        size_t factor3=0;
        size_t factor4=0;
        for (const auto &tile : this->tiles)
        {
            if ((tile.x == this->width / 2) or (tile.y == this->height / 2))
            {
                continue;
            }

            if ((tile.x < this->width / 2) and (tile.y < this->height / 2))
            {
                factor1 = factor1 + tile.numberOfRobots;
            }
            if ((tile.x > this->width / 2) and (tile.y < this->height / 2))
            {
                factor2 = factor2 + tile.numberOfRobots;
            }
            if ((tile.x < this->width / 2) and (tile.y > this->height / 2))
            {
                factor3 = factor3 + tile.numberOfRobots;
            }
            if ((tile.x > this->width / 2) and (tile.y > this->height / 2))
            {
                factor4 = factor4 + tile.numberOfRobots;
            }
        }
        std::cout << "Safety factor: " << factor1 * factor2 * factor3 * factor4 << "\n";
    }

    static std::string const ToString(std::vector<Tile> tiles)
    {
        std::string output{};
        size_t lastY = tiles.empty() ? 0 : tiles[0].y;
        for (size_t i=0; i<tiles.size(); i++)
        {
            output = output + ((lastY == tiles[i].y) ? "" : "\n") + Tile::ToString(tiles[i]);
            lastY = tiles[i].y;
        }
        return output;
    }

    static std::string const ToString(std::vector<Robot> robots)
    {
        return boost::algorithm::join(robots |
                boost::adaptors::transformed([](const Robot &robot)
                {
                    return Robot::ToString(robot);
                }), "\n");
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // std::string filename("example_input"); // 12
    std::string filename("input");
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

    Area area{};
    if (filename == "example_input")
    {
        area = Area(11, 7); 
    }
    else
    {
        area = Area(101, 103);
    }

    area.ParseInput(lines);
    std::cout << "tiles size: " << area.tiles.size() << "\n";
    std::cout << Area::ToString(area.tiles) << "\n";
    std::cout << Area::ToString(area.robots) << "\n";

    std::cout << "Area initial position\n";
    area.InitializeRobots();
    std::cout << Area::ToString(area.tiles) << "\n";
    std::cout << "Area after 100 seconds\n";
    area.MoveRobots(100);
    std::cout << Area::ToString(area.tiles) << "\n";
    area.CalculateSafetyFactor();

    return EXIT_SUCCESS;
}
