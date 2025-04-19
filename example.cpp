#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

namespace po = boost::program_options;

std::vector<std::string> filter_first_positional(int argc, char* argv[]) {
    std::vector<std::string> args;
    bool skipped = false;
    for (int i = 1; i < argc; ++i) {  // 跳过程序名 argv‌:ml-citation{ref="6" data="citationList"}
        std::string arg = argv[i];
        if (!skipped && (arg.find("--") != 0 && arg.find('-') != 0)) {
            skipped = true;  // 跳过第一个非选项参数‌:ml-citation{ref="2,7" data="citationList"}
            continue;
        }
        args.push_back(arg);
    }
    return args;
}

bool hasTwoKeysInSetUsingAccumulate(const std::unordered_map<std::string, int>& myMap, const std::unordered_set<std::string>& mutuallyMutex) {
    return std::accumulate(myMap.begin(), myMap.end(), 0, [&](int count, const auto& pair) {
        return count + (mutuallyMutex.count(pair.first) > 0);
    }) <= 2;
}

int main(int argc, char* argv[]) {

    std::unordered_map<std::string, int> m{{"test", 1}, {"run", 2}, {"ctest", 1}};
    std::unordered_set<std::string> s{"test", "run", "gh"};
    std::cout << hasTwoKeysInSetUsingAccumulate(m,s) << std::endl;
    std::cout <<  m.at(0) << "xxxxx" << std::endl;

    // try {
    //     // 定义选项参数
    //     po::options_description desc("Allowed options");
    //     po::options_description main("Allowed");
    //     desc.add_options()
    //         ("help", "Print help message")
    //         ("id", po::value<int>()->value_name("[-i | --id]"), "ID parameter")
    //         ("file", po::value<std::string>(), "File path");

    //     // 过滤首个非选项参数
    //     auto filtered_args = filter_first_positional(argc, argv);

      
    //     auto parsed = po::command_line_parser(argc-1, argv+1).options(main).allow_unregistered().style(po::command_line_style::unix_style).run();
        
    //     po::variables_map vm;
    //     po::store(parsed, vm);
    //     po::notify(vm);

    //     auto unrecognized_options = po::collect_unrecognized(parsed.options, po::include_positional);
    //     for(auto ele : unrecognized_options)
    //     {
    //         std::cout << "ele = " << ele << std::endl;
    //     }

    //     if (vm.count("help")) {
    //         std::cout << desc << std::endl;
    //         return 0;
    //     }

    //     parsed = po::command_line_parser(unrecognized_options).options(desc).allow_unregistered().style(
    //         po::command_line_style::unix_style).run();
    //     po::store(parsed, vm);
    //     po::notify(vm);

    //     if (vm.count("id"))
    //     {
    //         std::cout << vm["id"].as<int>() << std::endl;
    //     }
        
    //     // 处理其他选项...
    // } catch (const po::error& e) {
    //     std::cerr << "Error: " << e.what() << std::endl;
    //     return 1;
    // }
    // return 0;
}
