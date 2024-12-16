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

struct Tile
{
    size_t x;
    size_t y;
    char symbol;
    Tile *left = nullptr;
    Tile *right = nullptr;
    Tile *up = nullptr;
    Tile *down = nullptr;

    Tile(const size_t _x, const size_t _y, const char _symbol) :
             x(_x), y(_y), symbol(_symbol) {}

    bool IsBox() const 
    {
        return this->symbol == 'O';
    }

    bool IsWall() const 
    {
        return this->symbol == '#';
    }

    bool IsFree() const
    {
        return this->symbol == '.';
    }

    bool MoveLeft()
    {
        if (this->left->IsFree())
        {
            // std::cout << "free\n";
            (*(this->left)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        else if (this->left->IsWall())
        {
            // std::cout << "wall\n";
            return false;
        }
        // box
        // std::cout << "box\n";
        if (this->left->MoveLeft())
        {
            (*(this->left)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    bool MoveRight()
    {
        if (this->right->IsFree())
        {
            // std::cout << "free\n";
            (*(this->right)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        else if (this->right->IsWall())
        {
            // std::cout << "wall\n";
            return false;
        }
        // box
        // std::cout << "box\n";
        if (this->right->MoveRight())
        {
            (*(this->right)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    bool MoveUp()
    {
        if (this->up->IsFree())
        {
            // std::cout << "free\n";
            (*(this->up)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        else if (this->up->IsWall())
        {
            // std::cout << "wall\n";
            return false;
        }
        // box
        // std::cout << "box\n";
        if (this->up->MoveUp())
        {
            (*(this->up)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    bool MoveDown()
    {
        if (this->down->IsFree())
        {
            // std::cout << "free\n";
            (*(this->down)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        else if (this->down->IsWall())
        {
            // std::cout << "wall\n";
            return false;
        }
        // box
        // std::cout << "box\n";
        if (this->down->MoveDown())
        {
            (*(this->down)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    static std::string const ToString(const Tile &tile)
    {
        return std::string(1, tile.symbol);
    }
    friend std::ostream &operator<<(std::ostream &os, const Tile &tile)
    {
        return os << Tile::ToString(tile);
    }
};

struct Instruction
{
    char symbol;

    Instruction (const char _symbol) : symbol(_symbol) {}

    static std::string const ToString(const Instruction &instruction)
    {
        return std::string(1, instruction.symbol);
    }
    friend std::ostream &operator<<(std::ostream &os, const Instruction &instruction)
    {
        return os << Instruction::ToString(instruction);
    }
};

struct Warehouse
{
    std::vector<Tile> tiles;
    std::vector<Instruction> instructions;
    Tile *robot;
    ssize_t width;
    ssize_t height;

    Warehouse () = default;

    void ParseInput(const std::vector<string> &input)
    {
        bool parsingInitialAreaDimensions = true;
        size_t idRobot{};
        for (size_t y=0; y<input.size(); y++)
        {
            if (input[y].find("#") != std::string::npos)
            {
                if (parsingInitialAreaDimensions)
                {
                    this->width = input[y].size();
                    // this->height = input.size();
                    parsingInitialAreaDimensions = false;
                }
                for (size_t x=0; x<input[y].size(); x++)
                {
                    this->tiles.emplace_back(x, y, input[y][x]);
                    if (input[y][x] == '@')
                    {
                        idRobot = y*this->width + x;
                        // this->robot = &this->tiles[y*this->width + x];
                    }
                }
                continue;
            }
            if (input[y].empty()) 
            {
                this->height = y;
                this->robot = &this->tiles[idRobot];
                continue;
            }

            for (size_t x=0; x<input[y].size(); x++)
            {
                this->instructions.emplace_back(input[y][x]);
            }
        }
        this->AddNeighbours();
        std::cout << "width: " << this->width << ", height: " << this->height << "\n";
        std::cout << "robot at x: " << this->robot->x << ", y: " << this->robot->y << "\n";
    }

    void AddNeighbours()
    {
        for (size_t i=0; i<this->tiles.size(); i++)
        {
            // → → →
            if (((i+1) < this->tiles.size()) and ((i+1)%this->width != 0))  tiles[i].right = &tiles[i+1];
            // ← ← ←
            if ((i > 0) and (i % this->width != 0))
            {
                // std::cout << "left of " << i << " is " << i-1 << "\n";
                tiles[i].left = &tiles[i-1];
            }
            // ↑ ↑ ↑
            if (i > (this->width-1))  tiles[i].up = &tiles[i-this->width];
            // ↓ ↓ ↓
            if ((i+this->width) < this->width * this->height)  tiles[i].down = &tiles[i+this->width];
        }
    }

    Tile &GetTile(const size_t x, const size_t y)
    {
        return *std::find_if(this->tiles.begin(), this->tiles.end(), [x,y](Tile &tile)
                {
                    return (x == tile.x) and (y == tile.y);
                });
    }

    void ExecuteInstruction(const Instruction &instruction)
    {
        if (instruction.symbol == '<')
        {
            // std::cout << "Moving left\n";
            if ( (*(this->robot)).MoveLeft())
            {
                this->robot = &this->GetTile(this->robot->x-1, this->robot->y);
            }
        }
        else if (instruction.symbol == '>')
        {
            // std::cout << "Moving right\n";
            if ( (*(this->robot)).MoveRight())
            {
                this->robot = &this->GetTile(this->robot->x+1, this->robot->y);
            }
        }
        else if (instruction.symbol == '^')
        {
            // std::cout << "Moving up\n";
            if ( (*(this->robot)).MoveUp())
            {
                this->robot = &this->GetTile(this->robot->x, this->robot->y-1);
            }
        }
        else if (instruction.symbol == 'v')
        {
            // std::cout << "Moving down\n";
            if ( (*(this->robot)).MoveDown())
            {
                this->robot = &this->GetTile(this->robot->x, this->robot->y+1);
            }
        }
        else    std::cout << "Wrong instruction!\n";
    }

    void ExecuteInstructions()
    {
        for (const auto &instruction : this->instructions)
        {
            this->ExecuteInstruction(instruction);
            // std::cout << Warehouse::ToString(this->tiles) << "\n";
        }
        std::cout << Warehouse::ToString(this->tiles) << "\n";
    }

    void CalculateSumOfBoxes()
    {
        size_t total=0;
        for (const auto &tile : this->tiles)
        {
            if (tile.IsBox())
            {
                total = total + tile.x + 100 * tile.y;
            }
        }
        std::cout << "total: " << total << "\n";
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

    static std::string const ToString(std::vector<Instruction> instructions)
    {
        return boost::algorithm::join(instructions |
                boost::adaptors::transformed([](const Instruction &instruction)
                {
                    return Instruction::ToString(instruction);
                }), "");
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // std::string filename("example_input_small"); // 2028
    // std::string filename("example_input"); // 10092
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

    Warehouse warehouse{};
    warehouse.ParseInput(lines);
    std::cout << Warehouse::ToString(warehouse.tiles) << "\n";
    std::cout << Warehouse::ToString(warehouse.instructions) << "\n";
    warehouse.ExecuteInstructions();
    warehouse.CalculateSumOfBoxes();
    

    return EXIT_SUCCESS;
}
