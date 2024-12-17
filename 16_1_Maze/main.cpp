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

    bool IsReindeer() const
    {
        return (this->symbol == '<') or (this->symbol == '>') or
               (this->symbol == '^') or (this->symbol == 'v');
    }

    bool IsWall() const 
    {
        return (this->symbol == '#');
    }

    bool IsEnd() const 
    {
        return (this->symbol == 'E');
    }

    bool IsFree() const 
    {
        return (this->symbol == '.') or this->IsEnd();
    }

    bool IsLeft() const 
    {
        return (this->symbol == '<');
    }

    bool IsRight() const 
    {
        return (this->symbol == '>');
    }

    bool IsUp() const 
    {
        return (this->symbol == '^');
    }

    bool IsDown() const 
    {
        return (this->symbol == 'v');
    }

    bool IsLeftSideFree() const
    {
        if (this->IsLeft()) return this->down->IsFree();
        else if (this->IsRight()) return this->up->IsFree();
        else if (this->IsUp()) return this->left->IsFree();
        else return this->right->IsFree();
    }

    bool IsForwardSideFree() const
    {
        if (this->IsLeft()) return this->left->IsFree();
        else if (this->IsRight()) return this->right->IsFree();
        else if (this->IsUp()) return this->up->IsFree();
        else return this->down->IsFree();
    }

    bool MoveLeft()
    {
        if (not this->left->IsWall())
        {
            std::cout << "MoveLeft\n";
            (*(this->left)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    bool MoveRight()
    {
        if (not this->right->IsWall())
        {
            std::cout << "MoveRight\n";
            (*(this->right)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    bool MoveUp()
    {
        if (not this->up->IsWall())
        {
            std::cout << "MoveUp\n";
            (*(this->up)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    bool MoveDown()
    {
        if (not this->down->IsWall())
        {
            std::cout << "MoveDown\n";
            (*(this->down)).symbol = this->symbol;
            this->symbol = '.';
            return true;
        }
        return false;
    }

    void TurnClockwise()
    {
        std::cout << "TurnClockwise\n";
        if (this->IsLeft()) this->symbol = '^';
        else if (this->IsRight()) this->symbol = 'v';
        else if (this->IsUp()) this->symbol = '>';
        else this->symbol = '<';
    }

    void TurnCounterclockwise()
    {
        std::cout << "TurnCounterclockwise\n";
        if (this->IsLeft()) this->symbol = 'v';
        else if (this->IsRight()) this->symbol = '^';
        else if (this->IsUp()) this->symbol = '<';
        else this->symbol = '>';
    }

    bool MoveForward()
    {
        if (this->IsLeft()) return this->MoveLeft();
        else if (this->IsRight()) return this->MoveRight();
        else if (this->IsUp()) return this->MoveUp();
        else return this->MoveDown();
    }

    static std::string const ToString(const Tile &tile)
    {
        return std::string(1, tile.symbol);
    }
    friend std::ostream &operator<<(std::ostream &os, const Tile &tile)
    {
        return os << Tile::ToString(tile);
    }
    friend bool operator==(const Tile &l, const Tile &r)
    {
        return (l.x == r.x) and (l.y == r.y);
    }
};

struct Maze
{
    std::vector<Tile> tiles;
    size_t width;
    size_t height;
    Tile *reindeer;
    Tile *endTile;

    Maze () = default;

    void ParseInput(const std::vector<string> &input)
    {
        this->height = input.size();
        this->width = input[0].size();

        size_t idReindeer{};
        size_t idEnd{};
        for (size_t y=0; y<input.size(); y++)
        {
            for (size_t x=0; x<input[y].size(); x++)
            {
                if (input[y][x] == 'S')
                {
                    this->tiles.emplace_back(x, y, '>');
                    idReindeer = y*this->width + x;
                    continue;
                }
                if (input[y][x] == 'E')
                {
                    idEnd = y*this->width + x;
                }
                this->tiles.emplace_back(x, y, input[y][x]);
            }
        }
        this->reindeer = &this->tiles[idReindeer];
        this->endTile = &this->tiles[idEnd];
        this->AddNeighbours();
        std::cout << "width: " << this->width << ", height: " << this->height << "\n";
        std::cout << "reindeer at x: " << this->reindeer->x << ", y: " << this->reindeer->y << "\n";
        std::cout << "end at x: " << this->endTile->x << ", y: " << this->endTile->y << "\n";
        std::cout << "Initial:\n" << Maze::ToString(this->tiles) << "\n";
    }

    void AddNeighbours()
    {
        for (size_t i=0; i<this->tiles.size(); i++)
        {
            // → → →
            if (((i+1) < this->tiles.size()) and ((i+1)%this->width != 0))  tiles[i].right = &tiles[i+1];
            // ← ← ←
            if ((i > 0) and (i % this->width != 0)) tiles[i].left = &tiles[i-1];
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

    void updateReindeer()
    {
        this->reindeer = &(*std::find_if(this->tiles.begin(), this->tiles.end(), [](Tile &tile)
        {
            return tile.IsReindeer();
        }));
    }

    bool Solve2(const size_t x, const size_t y)
    {
        std::cout << Maze::ToString(this->tiles) << "\n";
        this->GetTile(x, y).symbol = '*';
        if (this->GetTile(x,y) == *this->endTile) return true;

        if (this->GetTile(x-1,y).IsFree() and this->Solve2(x-1, y)) return true;
        if (this->GetTile(x+1,y).IsFree() and this->Solve2(x+1, y)) return true;
        if (this->GetTile(x,y+1).IsFree() and this->Solve2(x, y+1)) return true;
        if (this->GetTile(x,y-1).IsFree() and this->Solve2(x, y-1)) return true;

        this->GetTile(x, y).symbol = '.';
        return false;
    }

    void Solve2()
    {
        this->reindeer->symbol = '.';
        if (this->Solve2(this->reindeer->x, this->reindeer->y))
        {
            std::cout << "solved\n";
        }
    }

    void Solve()
    {
        size_t moves=0;
        size_t turnes=0;
        while (not (*this->reindeer == *this->endTile))
        {
            // if (i++>65) break;
            // std::cout << Maze::ToString(this->tiles) << "\n";
            // std::this_thread::sleep_for(std::chrono::milliseconds(80));

            if (not this->reindeer->IsLeftSideFree() and this->reindeer->IsForwardSideFree())
            {
                this->reindeer->MoveForward();
                moves++;
                this->updateReindeer();
                continue;
            }

            if (not this->reindeer->IsLeftSideFree() and not this->reindeer->IsForwardSideFree())
            {
                this->reindeer->TurnClockwise();
                turnes++;
                continue;
            }

            if (this->reindeer->IsLeftSideFree())
            {
                this->reindeer->TurnCounterclockwise();
                turnes++;
                this->reindeer->MoveForward();
                moves++;
                this->updateReindeer();
            }
            
            // if (dir == '<') this->reindeer = &this->GetTile(this->reindeer->x-1, this->reindeer->y);
            // else if (dir == '>') this->reindeer = &this->GetTile(this->reindeer->x+1, this->reindeer->y);
            // else if (dir == '^') this->reindeer = &this->GetTile(this->reindeer->x, this->reindeer->y-1);
            // else this->reindeer = &this->GetTile(this->reindeer->x, this->reindeer->y+1);
        }
        std::cout << "Moves forward: " << moves << ", turns: " << turnes << ", score: " << moves + turnes * 1000 << "\n";
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
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    std::string filename("example_input"); // 7036
    // std::string filename("input");
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

    Maze maze{};
    maze.ParseInput(lines);
    maze.Solve2();
    

    return EXIT_SUCCESS;
}
