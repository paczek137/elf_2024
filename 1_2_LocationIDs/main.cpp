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

int main()
{
    setlocale(LC_ALL, "");
    srand(time(0));
    
    std::filesystem::path cwd = std::filesystem::current_path().filename();
    cout << "Hello World: " << cwd << endl;

    // string filename("example_input"); // 31
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

    std::multiset<int, std::less<int>> left{};
    std::multiset<int, std::less<int>> right{};

    for (auto const& line : lines)
    {
        std::stringstream ss(line);
        int temp{};
        ss >> temp;
        left.emplace(temp);
        // cout << temp << " ";
        ss >> temp;
        right.emplace(temp);
        // cout << temp << " ";
        // break;
    }

    int totalSimilarityScore{};
    auto leftIt = left.cbegin();
    auto rightIt = right.cbegin();
    while(leftIt != left.cend())
    {
        int multiplier = right.count(*leftIt);
        totalSimilarityScore = totalSimilarityScore + multiplier * (*leftIt);
        cout << "score: " << multiplier * (*leftIt) << "\n";
        leftIt++;
        // rightIt++;
    }

    cout << "Total similarity score: " << totalSimilarityScore << "\n";



    // Camel camel{};
    // parseCamel(lines, camel);
    // camel.calculateRanks();
    // cout << camel << endl;
    // cout << "Total winning: " << camel.getTotalWinning() << endl;


    return EXIT_SUCCESS;
}
