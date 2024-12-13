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

struct Button
{
    size_t xMove;
    size_t yMove;

    Button () = default;
    Button (const size_t x, const size_t y) : xMove(x), yMove(y) {}

    static std::string const ToString(const Button &button)
    {
        return std::string("X+") + std::to_string(button.xMove) + std::string(", Y+") + std::to_string(button.yMove);
    }
    friend std::ostream &operator<<(std::ostream &os, const Button &button)
    {
        return os << Button::ToString(button);
    }
};

struct Claw
{
    Button buttonA;
    Button buttonB;
    size_t prizeX;
    size_t prizeY;
    size_t x=0;
    size_t y=0;

    Claw () = default;
    Claw (const Button &_buttonA, const Button &_buttonB, const size_t _prizeX, const size_t _prizeY) :
            buttonA(_buttonA), buttonB(_buttonB), prizeX(_prizeX), prizeY(_prizeY) {}

    static std::string const ToString(const Claw &claw)
    {
        return std::string("Button A: ") + Button::ToString(claw.buttonA) +
               std::string("\nButton B: ") + Button::ToString(claw.buttonB) +
               std::string("\nPrize: X=") + std::to_string(claw.prizeX) + 
               std::string(", Y=") + std::to_string(claw.prizeY);

    }
    friend std::ostream &operator<<(std::ostream &os, const Claw &claw)
    {
        return os << Claw::ToString(claw);
    }
};

struct Machines
{
    std::vector<Claw> claws;

    void ParseInput(const std::vector<string> &input)
    {
        Claw claw = Claw();
        for (const auto& line : input)
        {
            std::stringstream ss{};
            if (line.find("Button A") != std::string::npos)
            {
                size_t x{};
                size_t y{};
                ss << line.substr(line.find("+") + 1);
                ss >> x;
                ss.str(line.substr(line.rfind("+") + 1));
                ss >> y;
                claw.buttonA = Button(x, y);
            }
            if (line.find("Button B") != std::string::npos)
            {
                size_t x{};
                size_t y{};
                ss << line.substr(line.find("+") + 1);
                ss >> x;
                ss.str(line.substr(line.rfind("+") + 1));
                ss >> y;
                claw.buttonB = Button(x, y);
            }
            if (line.find("Prize") != std::string::npos)
            {
                ss << line.substr(line.find("=") + 1);
                ss >> claw.prizeX;
                ss.str(line.substr(line.rfind("=") + 1));
                ss >> claw.prizeY;

                this->claws.push_back(claw);
                claw = Claw();
            }
        }
    }

    

    static std::string const ToString(const std::vector<Claw> &claws)
    {
        return boost::algorithm::join(claws |
                boost::adaptors::transformed([](const Claw &claw)
                {
                    return Claw::ToString(claw) + "\n\n";
                }), "");
    }
};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    string filename("example_input"); // 480
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

    Machines machines{};
    machines.ParseInput(lines);
    std::cout << Machines::ToString(machines.claws) << "\n";

    return EXIT_SUCCESS;
}
