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

struct Rules
{
    int before;
    int after;
};

using PageNumbers = std::vector<int>;

struct Pages
{
    std::vector<Rules> rules;
    std::vector<PageNumbers> pages;

    void ParseRules(const std::vector<std::string> &input)
    {
        for (const auto &line : input)
        {
            std::stringstream ss(line);
            int temp;
            char c;
            Rules rule{};
            ss >> temp;
            rule.before = temp;
            ss >> c;
            ss >> temp;
            rule.after = temp;
            this->rules.push_back(rule);
        }
        // for (const auto &el : this->rules)
        // {
        //     std::cout << el.before << " " << el.after << "\n";
        // }
    }

    void ParsePageNumbers(const std::vector<std::string> &input)
    {
        // std::cout << "parsepagenumbers: size: " << input.size() << "\n";
        for (const auto &line : input)
        {
            std::stringstream ss(line);
            PageNumbers pageNumbers{};
            while (!ss.eof())
            {
                int temp{};
                char c{};
                ss >> temp;
                ss >> c;
                pageNumbers.push_back(temp);
            }
            if (not pageNumbers.empty()) this->pages.push_back(pageNumbers);
        }
        // for (const auto &el : this->pages)
        // {
        //     for (const auto &ell : el)
        //     {
        //         std::cout << ell << " ";    
        //     }
        //     std::cout << "\n";    
        // }
    }

    bool CheckRow(const PageNumbers &pages)
    {
        // std::cout << pages[0] << ", " << pages[1] << "...\n";
        for (const auto &rule : this->rules)
        {
            std::vector r = {rule.before, rule.after};
            if (not std::all_of(r.cbegin(), r.cend(), [pages](const int i)
                {
                    return std::find(pages.cbegin(), pages.cend(), i) != pages.cend();
                }))
            {
                continue;
            }
            // std::cout << "matching rule first: " << rule.before << ", after: " << rule.after << "\n";
            if (std::find(pages.cbegin(), pages.cend(), rule.after) < std::find(pages.cbegin(), pages.cend(), rule.before))
            {
                return false;
            }
        }
        return true;
    }
    void Check()
    {
        int total = 0;

        for (const auto &row : this->pages)
        {
            if (not this->CheckRow(row))
            {
                continue;
            }
            total = total +  *(row.begin() + row.size() / 2);
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

    // string filename("example_input"); // 143
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }
    
    std::vector<std::string> lines{};
    std::vector<std::string> lines2{};

    int i{};
    bool nowNumbers = false;
    while (getline(input_file, line, '\n')) 
    {
        // std::cout << i++ << "\n";
        // if (not std::isalpha(line[line.size()-1]) and line.size() > 1)
        // {
        //     line.pop_back();    
        // }

        if (line.size() == 0)
        {
            std::cout << "numbers\n";
            nowNumbers = true;
            continue;
        }

        if (nowNumbers)
        {
            lines2.push_back(line);
            continue;
        }
        lines.push_back(line);
        // input = input + line;
    }

    Pages pages{};
    pages.ParseRules(lines);
    pages.ParsePageNumbers(lines2);
    pages.Check();

    // int total = 0;
        

    // std::cout << "total: " << total << "\n";

    return EXIT_SUCCESS;
}
