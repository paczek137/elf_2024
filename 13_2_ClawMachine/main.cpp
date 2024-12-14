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

struct Button
{
    size_t xMove;
    size_t yMove;
    size_t costs;

    Button () = default;
    Button (const size_t x, const size_t y, const size_t _costs) : 
                xMove(x), yMove(y), costs(_costs) {}

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
    uint64_t prizeX;
    uint64_t prizeY;
    size_t x=0;
    size_t y=0;
    static constexpr uint64_t CONVERSION_ERROR = 10000000000000;
    uint64_t maxButtonPress;

    Claw () = default;
    Claw (const Button &_buttonA, const Button &_buttonB, const size_t _prizeX, const size_t _prizeY) :
            buttonA(_buttonA), buttonB(_buttonB), 
            prizeX(_prizeX + Claw::CONVERSION_ERROR), prizeY(_prizeY + Claw::CONVERSION_ERROR) {}

    void CalculateMaxButtonPress()
    {
        this->prizeX = this->prizeX + Claw::CONVERSION_ERROR;
        this->prizeY = this->prizeY + Claw::CONVERSION_ERROR;
        std::vector<uint64_t> localMaxim{};
        localMaxim.push_back(this->prizeX / this->buttonA.xMove);
        localMaxim.push_back(this->prizeX / this->buttonB.xMove);
        localMaxim.push_back(this->prizeY / this->buttonA.xMove);
        localMaxim.push_back(this->prizeY / this->buttonB.xMove);
        this->maxButtonPress = *std::max_element(localMaxim.cbegin(), localMaxim.cend());
    }

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
    static constexpr size_t BUTTON_A_COSTS = 3;
    static constexpr size_t BUTTON_B_COSTS = 1;
    static constexpr size_t MAX_BUTTON_PRESS = 100;

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
                claw.buttonA = Button(x, y, BUTTON_A_COSTS);
            }
            if (line.find("Button B") != std::string::npos)
            {
                size_t x{};
                size_t y{};
                ss << line.substr(line.find("+") + 1);
                ss >> x;
                ss.str(line.substr(line.rfind("+") + 1));
                ss >> y;
                claw.buttonB = Button(x, y, BUTTON_B_COSTS);
            }
            if (line.find("Prize") != std::string::npos)
            {
                ss << line.substr(line.find("=") + 1);
                ss >> claw.prizeX;
                ss.str(line.substr(line.rfind("=") + 1));
                ss >> claw.prizeY;

                claw.CalculateMaxButtonPress();
                this->claws.push_back(claw);
                claw = Claw();
            }
        }
    }

    bool IsNotPossibleToWin(const Claw &claw)
    {
        return (((claw.buttonA.xMove * Machines::MAX_BUTTON_PRESS +
                  claw.buttonB.xMove * Machines::MAX_BUTTON_PRESS) < claw.prizeX) or
                ((claw.buttonA.yMove * Machines::MAX_BUTTON_PRESS +
                  claw.buttonB.yMove * Machines::MAX_BUTTON_PRESS) < claw.prizeY));
    }

    size_t CalculateClaw(const Claw &claw)
    {
        std::vector<size_t> possibleCosts{};

        // magic
        double a = (static_cast<double>(claw.buttonB.xMove) * claw.prizeY - static_cast<double>(claw.buttonB.yMove) * claw.prizeX) /
                        (claw.buttonA.yMove * claw.buttonB.xMove - static_cast<double>(claw.buttonA.xMove) * claw.buttonB.yMove);
        double b = (static_cast<double>(claw.prizeX) - static_cast<double>(claw.buttonA.xMove) * a) / claw.buttonB.xMove;

        double af{};
        double bf{};

        if ((std::modf(a, &af) == 0) and (std::modf(b, &bf) == 0))
        {
            return static_cast<uint64_t>(a) * Machines::BUTTON_A_COSTS + 
                    static_cast<uint64_t>(b) * Machines::BUTTON_B_COSTS;
        }
        return 0;
    }

    void Calculate()
    {
        size_t total=0;
        for (const auto &claw : this->claws)
        {
            // skipping case when there is not possible to win the prize
            // using max 100 button press
            // if (this->IsNotPossibleToWin(claw))
            // {
            //     std::cout << "Skipping..." << claw.prizeX << ":" << claw.prizeY << "\n";
            //     continue;
            // }
            total = total + this->CalculateClaw(claw);
        }
        std::cout << "total: " << total << "\n";
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

    // string filename("example_input"); // 875318608908
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

    Machines machines{};
    machines.ParseInput(lines);
    std::cout << Machines::ToString(machines.claws) << "\n";
    machines.Calculate();

    // std::cout << "max for size_t: " << std::numeric_limits<int>::max() << "\n";
    // std::cout << "max for uint64_t: " << std::numeric_limits<uint64_t>::max() << "\n";

    return EXIT_SUCCESS;
}
