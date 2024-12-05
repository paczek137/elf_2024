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

int multiply(const std::string &s)
{
    std::regex reg("[0-9]{1,3}");
    std::vector<string> result{};
    std::smatch match;
    auto searchStart(s.cbegin());

    while (std::regex_search(searchStart, s.cend(), match, reg))
    {
        result.push_back(match[0]);
        // std::cout << match[0] << "\n";
        searchStart = match.suffix().first;
    }
    
    if (result.empty() or result.size() != 2)
    {
        std::cout << "Wrong input\n";
        return 0;
    }
    return std::stoi(result[0]) * std::stoi(result[1]);
}

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 48
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }

    
    std::string input{};

    while (getline(input_file, line, '\n')) 
    {
        // lines.push_back(line);
        input = input + line;
    }

    int total = 0;
    bool enabled = true;
    std::cout << input << "\n";
    // std::regex reg("/[0-9]{,3},[0-9]{,3}/");
    std::regex reg("mul\\([0-9]{1,3},[0-9]{1,3}\\)");
    std::vector<string> result{};
    std::smatch match;
    auto searchStart(input.cbegin());

    while (std::regex_search(searchStart, input.cend(), match, reg))
    {
        auto foundDo = match.prefix().str().rfind("do()");
        auto foundDont = match.prefix().str().rfind("don't()");
        if (foundDont != std::string::npos)
        {
            std::cout << "found dont, foundDont: " << foundDont << ", foundDo: " << foundDo << "\n";
            if (foundDo != std::string::npos)
            {
                if (foundDont > foundDo)
                {
                    enabled = false;
                }
            }
            else
            {
                enabled = false;
            }
        }
        else if (foundDo != std::string::npos)
        {
            std::cout << "found do, foundDont: " << foundDont << ", foundDo: " << foundDo << "\n";
            if (foundDont != std::string::npos)
            {
                if (foundDo > foundDont)
                {
                    enabled = true;
                }
            }
            else
            {
                enabled = true;
            }
        }
        if (enabled)
        {
            result.push_back(match[0]);
            std::cout << match.prefix().str() << "\n";
        }
        
        searchStart = match.suffix().first;
    }
    
    if (result.empty())
    {
        std::cout << "Wrong input\n";
        return 0;
    }

    for (const auto &el : result)
    {
        std::cout << el << "\n";
        total = total + multiply(el);
    }
    

    std::cout << "total: " << total << "\n";

    return EXIT_SUCCESS;
}
