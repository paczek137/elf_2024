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

template<typename InputIt, typename T>
bool nextPermutationWithRepetition(InputIt begin, InputIt end, T from_value, T to_value) {
    auto it = std::find_if_not(std::make_reverse_iterator(end),
                               std::make_reverse_iterator(begin),
                               [&to_value](auto current) { return to_value == current; });

    if (it == std::make_reverse_iterator(begin))
        return false;

    auto bound_element_iterator = std::prev(it.base());

    (*bound_element_iterator)++;
    std::fill(std::next(bound_element_iterator), end, from_value);

    return true;
}

struct Equation
{
    long result;
    std::vector<long> values;
    std::vector<int> operators;

    char getOperator(const int op) const
    {
        if (op == 1) return '+';
        else if (op == 2) return '*';
        return ' ';
    }
    void Print() const
    {
        std::cout << result << ": ";
        for (size_t i=0; i<values.size(); i++)
        {
            std::cout << values[i];
            if (i<operators.size())
            {
                std::cout << " " << this->getOperator(operators[i]) << " ";
            }
        }
    }
};

struct Equations
{
    std::vector<Equation> equations;
    const std::vector<char> VALID_OPERATORS = {'+', '*'};

    void ParseInput(const std::vector<std::string> &input)
    {
        for (auto const& line : input)
        {
            std::cout << line << "\n";
            std::stringstream ss(line);
            Equation equation{};
            char c;
            ss >> equation.result;
            ss >> c;
            while (!ss.eof())
            {
                long temp;
                ss >> temp;
                equation.values.push_back(temp);
                equation.operators.push_back(1);
                // equation.operators.push_back(' ');
            }
            equation.operators.pop_back();
            this->equations.push_back(equation);
        }
    }
    void Print()
    {
        for (const auto &el : this->equations)
        {
            el.Print();
            std::cout << "\n";
        }
    }
    // int CalcSingleOperation(const int a, const int b, const char o)
    // {
    //     if (o == '+') return a + b;
    //     else if (o == '*') return a * b;
    //     return 0;
    // }
    long CalcSingleOperation(const long a, const long b, const int o)
    {
        if (o == 1) return a + b;
        else if (o == 2) return a * b;
        return 0;
    }
    bool CheckOperator(const Equation &equation)
    {
        long result = equation.values[0];
        // if (std::any_of(equation.operators.cbegin(), equation.operators.cend(), [](const char c)
        //                 {
        //                     return c == ' ';  
        //                 }))
        // {
        //     return false;
        // }

        for (size_t i=1; i<equation.values.size(); i++)
        {
            if ((i-1) < equation.operators.size())
            {
                result = this->CalcSingleOperation(result, equation.values[i], equation.operators[i-1]);
            }
        }
        // std::cout << result << " == " << equation.result << " ??\n";
        equation.Print();
        if (result == equation.result)
        {
            std::cout << " GOOD\n";
        }
        else
        {
            std::cout << " BAD\n";
        }
        return result == equation.result;
    }
    void Calc()
    {
        long total{};
        int i{};
        // this->equations[0].operators[0] = '*';
        for (auto &el : this->equations)
        {
            std::cout << ++i/this->equations.size() << "\n";
            // for (size_t i=0; i<el.operators.size(); i++)
            // {
            //     for (const auto &op : VALID_OPERATORS)
            //     {
            //         el.operators[i] = op;
            //         total = total + this->CheckOperator(el) ? 1 : 0;
            //     }
            // }
            do
            {
                // total = total + (this->CheckOperator(el) ? 1 : 0);
                if (this->CheckOperator(el))
                {
                    total = total + el.result;
                    break;
                }
            } while (nextPermutationWithRepetition(el.operators.begin(), el.operators.end(), 1, 2));
            // break;
        }
        std::cout << "total: " << total << "\n";
    }

};

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 3749
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

    Equations equations{};
    equations.ParseInput(lines);
    equations.Print();
    std::cout << "\nCalculating...\n";
    equations.Calc();

    // std::vector<char> vec(2, 'a');

    // do {
    //     for (const auto &el : vec)
    //     {
    //         std::cout << el << " ";
    //     }
    //     std::cout << "\n";
    // } while (nextPermutationWithRepetition(vec.begin(), vec.end(), 'a', 'b'));


    return EXIT_SUCCESS;
}
