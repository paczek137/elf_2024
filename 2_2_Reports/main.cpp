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

enum class Sign
{
    ZERO = 0,
    NEGATIVE = 1,
    POSITIVE = 2
};

Sign getSign(const int num1, const int num2)
{
    if ((num2 - num1) > 0) return Sign::POSITIVE;
    else if ((num2 - num1) < 0) return Sign::NEGATIVE;
    return Sign::ZERO;
}

// struct Report
// {
//     std::vector<int> levels;
//     int positiveSigns;
//     int negativeSigns;
//     int zeroSigns;
//     int differ;
// };

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 4
    string filename("input");
    ifstream input_file(filename);

    string line{};
 
    if (not input_file.is_open())
    {
        cout << "Could not open the file: " << filename << endl;
        return EXIT_FAILURE;
    }

    vector<string> lines{};

    while (getline(input_file, line, '\n')) 
    {
        lines.push_back(line);
    }

    using Report = std::vector<int>;
    // using Reports = std::vector<Report>;
    // Reports reports = Reports{};
    int totalSafeRerports = 0;

    for (auto const& line : lines)
    {
        std::stringstream ss(line);
        Report report{};
        bool isSafe = true;
        while (!ss.eof())
        {
            int temp;
            ss >> temp;
            report.push_back(temp);
        }
        const Sign sign = getSign(report[1], report[0]);
        for (size_t i=0; i<report.size()-1; i++)
        {
            if ( not (getSign(report[i+1], report[i]) == sign))
            {
                isSafe = false;
                break;
            } 
            if (abs(report[i+1] - report[i]) > 3)
            {
                isSafe = false;
                break;
            }
        } 

        // for (size_t i=0; i<report.levels.size()-1; i++)
        // {
        //     Sign s = getSign(report.levels[i+1], report.levels[i]);
        //     if (Sign::POSITIVE == s) report.positiveSigns++;
        //     else if (Sign::NEGATIVE == s) report.negativeSigns++;
        //     else report.zeroSigns++;

        //     if (abs(report.levels[i+1] - report.levels[i]) > 3) report.differ++;

        // }

        // std::cout << "pos: " << report.positiveSigns << ", neg: " << report.negativeSigns
        //               << " zero: " << report.zeroSigns << ", diff: " << report.differ << "\n";
        // if (report.positiveSigns > 1)
        // {
        //     if ((report.negativeSigns > 1) ||
        //         (report.zeroSigns > 1) ||
        //         (report.differ > 1))
        //     {
        //         isSafe = false;
        //     }
        // }
        // else if (report.negativeSigns > 1)
        // {
        //     if ((report.positiveSigns > 1) ||
        //         (report.zeroSigns > 1) ||
        //         (report.differ > 1))
        //     {
        //         isSafe = false;
        //     }
        // }
        if (isSafe)
        {
            // std::cout << "safe \n";
            totalSafeRerports = totalSafeRerports + 1;
            continue;
        }

        for (size_t i=0; i<report.size(); i++)
        {
            Report tolerateRerpot = report;
            tolerateRerpot.erase(tolerateRerpot.begin() + i);
            const Sign sign = getSign(tolerateRerpot[1], tolerateRerpot[0]);
            isSafe = true;
            for (size_t i=0; i<tolerateRerpot.size()-1; i++)
            {
                if ( not (getSign(tolerateRerpot[i+1], tolerateRerpot[i]) == sign))
                {
                    isSafe = false;
                    break;
                } 
                if (abs(tolerateRerpot[i+1] - tolerateRerpot[i]) > 3)
                {
                    isSafe = false;
                    break;
                }
            }
            if (isSafe) break;
        }
        if (isSafe)
        {
            // std::cout << "safe \n";
            totalSafeRerports = totalSafeRerports + 1;
        }
    }

    std::cout << "totalSafeRerports: " << totalSafeRerports << "\n";

    return EXIT_SUCCESS;
}
