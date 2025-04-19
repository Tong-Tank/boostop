#pragma once

#include <boost/program_options.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <unordered_set>
#include <iostream>
#include <any>
#include <vector>
#include <format>
#include <regex>
#include <numeric>

namespace po = boost::program_options;

/**
 * 
 * This class for positional option, when your using position option
 * it will output -i [ --id ]
 * Now will output [-i | --id]
 * 
 * The main idea is to find a matching regular expression and delete it
 * 
 * This class does not currently support custom regular expressions. It is limited to the option value name, 
 * which has the same format as the nvidia-smi positional parameter format. 
 * If there are other requirements, you can modify the implementation of this class
 * 
 * -x [ --xxx ] [-x | --xxx] delete match -x [ --xxx ] regular expression
 */
class CustomOptionsDescription : public po::options_description
{
public:
    CustomOptionsDescription(const std::string& caption = "",
                                unsigned line_length = m_default_line_length,
                                unsigned min_description_length = 80) // The reason is 80 because the boost source code is set to 80 and it is a private variable
        : po::options_description(caption, line_length, min_description_length)
    {
        std::cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
    }

    void print(std::ostream& os, unsigned width = 0)
    {
        std::cout << "++++++++++++++++++++\n";
        std::ostringstream oss;
        po::options_description::print(oss);
        std::string output = oss.str();
        std::regex pattern(R"(-\w\s+\[\s+--\w+\s+\])");
        
        output = std::regex_replace(output, pattern, "");
        os << output;
    }

};



class ParseCmdLine : boost::noncopyable
{
public:
    ParseCmdLine(unsigned int count) :count{count}
    {
    }
    
    void addOptionsToDesc(po::options_description& desc, const std::vector<std::tuple<std::string, po::value_semantic*, std::string>>& optionTuples);

    template<typename... Descs>
    void addAllOptions(po::options_description& allDesc, Descs&&... descs) 
    {
        (allDesc.add(std::forward<Descs&&>(descs)), ...);
    }

    template<typename T = po::options_description>
    const T createDescription(std::string name)
    {
        T childDes(name);
        return childDes;
    }

    unsigned int getDeviceCount()
    {
        return count;
    }


private:
    

    bool is_option_specified(const po::variables_map& vm, const char* opt) 
    {
        return vm.count(opt) && !vm[opt].defaulted();
    }
    
    template <typename... Args>
    void conflicting_options(const po::variables_map& vm, std::string main_opt, Args&&... args) 
    {
        if (vm.count(main_opt.c_str())) {
            bool all_conflicts_specified = (is_option_specified(vm, args) && ...);
            if (all_conflicts_specified) {
                std::string conflict_str;
                ((conflict_str += (conflict_str.empty() ? "" : " and ") + std::string(args)), ...);
                throw std::logic_error("Conflicting options '" + conflict_str + "' cannot be used together with '" + std::string(main_opt) + "'.");
            }
        }
    }

private:
    

    // device count
    unsigned int count{0};
};


class CreateSMIOptions
{
public:
    CreateSMIOptions(unsigned int count) : pcmdline{new ParseCmdLine(count)}
    {
    }

    ~CreateSMIOptions()
    {
        if(pcmdline != nullptr)
        {
            delete pcmdline;
        }
    }

    void createCmdLineLine();
    void parseCmdLineArgs(int argc, char** argv);

    template<typename T>
    T getOptionValue(const std::string& name)
    {
        if (vm.count(name)) {
            return vm[name].as<T>();
        }

        return T{};
    }

    auto getVMSize()
    {
        return vm.size();
    }

    
    std::tuple<bool, std::unordered_map<std::string, std::any>> showSummaryTable();
    std::tuple<bool, std::unordered_map<std::string, std::any>> showQuery(); 
    std::tuple<bool, std::unordered_map<std::string, std::any>> showSelectQuery();
    std::vector<std::string> parseQuerySampleItem(std::vector<std::string> item);

    /**
     * return table or line 
     * More support may be needed in the future
     */
    std::string displayFormat();

    void debugInfo()
    {
        for(auto [key, _] : vm)
        {
            std::cout << "key = " << key << std::endl;
        }
    }

    // op is std::string_view type which is option need to check
    // T is std::vector<std::string> type which is unrecognized options
    // U is options_description type which is [ plus any of ] ops

    template<typename T, typename U/* typename E = std::monostate*/ >
    void updatevm(std::string op, const T& t, const U& u, bool check = true/* const E& e = {}*/)
    {
        /**
         * may be can use require
         */
        // static_assert(std::is_same_v<T, std::vector<std::string>> || std::is_same_v<U, po::options_description> || std::is_same_v<E, po::options_description> || std::is_same_v<E, std::monostate>, "T Or U type is not correct.");
        auto parsed = po::command_line_parser(t).options(u).allow_unregistered().style(
            po::command_line_style::unix_style).run();
        po::store(parsed, vm);
        po::notify(vm);

        debugInfo();
        
        if(check)
        {
            for (const auto& opt : parsed.options) 
            {
                std::cout << "string_key = " << opt.string_key << std::endl;
                if (!u.find_nothrow(opt.string_key, false))
                {
                    // if constexpr (std::is_same_v<E, po::options_description>)
                    // {
                    //     if(e.find_nothrow(opt.string_key, false))
                    //         continue;
                    // }
                    
                    
                    std::cerr << std::format("Error: {} is not valid for this command, please run \'sdc-smi -h\' for help.\n", op);
                    break;
                }
            }
        }
        
    }


    auto getvm() const
    {
        return vm;
    }

    auto getFirstArgs() const
    {
        return positionalArgs;
    }
private:
    
    

    void helpOptionFunction()
    {
        std::cout << mainDesc << std::endl;
        std::cout << listOptions << std::endl;
        std::cout << summaryOptions << std::endl;
        std::cout << queryOptions << std::endl;
        std::cout << queryOptionsPlus << std::endl;
        std::cout << selectQueryOptions << std::endl;
        std::cout << selectQueryPlusOptions << std::endl;
    }
    // bool checkOptionsOnlyHaveDesc(po::options_description& desc);

    [[nodiscard( "need to check conflict op" )]] bool checkOptionsHasConflict()
    {

        return std::accumulate(vm.begin(), vm.end(), 0, [&](int count, const auto& pair) {
            return count + (mutuallyMutex.count(pair.first) > 0);
        }) >= 2;

    }


public:
    // description
    po::options_description mainDesc{pcmdline->createDescription(std::string{"sdcml-smi [OPTION1 [ARG1]] [OPTION2 [ARG2]] ..."})}; //main description
    po::options_description listOptions{pcmdline->createDescription(std::string{"List OPTIONS"})}; // list option
    po::options_description summaryOptions{pcmdline->createDescription(std::string{"SUMMARY OPTIONS:\n  <no arguments> show a summary of GPUs connected to the system\n[plus any of]"})}; // summary Options
    po::options_description queryOptions{pcmdline->createDescription(std::string{"QUERY OPTIONS"})}; // query options
    po::options_description queryOptionsPlus{pcmdline->createDescription(std::string{"[plus any of]"})}; //plus any of options
    po::options_description selectQueryOptions{pcmdline->createDescription(std::string{"SELECTIVE QUERY OPTIONS:\n    Allows the caller to pass an explicit list of properties to query."})}; //select query options
    po::options_description selectQueryMandatoryOptions{pcmdline->createDescription(std::string{"[Mandatory]"})}; // select query options Mandatory
    po::options_description selectQueryPlusOptions{pcmdline->createDescription(std::string{"[plus any of]"})}; //select query options plus
    po::options_description hiddenOptions{pcmdline->createDescription(std::string{"Hidden Options"})}; // Hidden Options for option name which start with query like query-gpu query-supported-clocks ...
    // po::options_description hiddenOptionsWithPositional{pcmdline->createDescription(std::string{"Hidden Options With Positional"})}; // Hidden Options for option without -- or -, like dmon/daemon
    po::options_description dmonOptions{pcmdline->createDescription(std::string{"GPU statistics are displayed in scrolling format with one line per sampling interval. Metrics to be monitored can be adjusted based on the width of termianl window.\nUsage: sdc-smi dmon [options]\n\nOptions include"})};
    po::options_description dmonOptionsHelp{pcmdline->createDescription(std::string{"dmon help output"})};
    std::vector<std::string> unrecognized_options;
private:
    ParseCmdLine* pcmdline;
    po::options_description allDesc;
    po::variables_map vm;
    std::string positionalArgs{""};
    


    // validate value
    // May be putinfo func and to check.
    std::unordered_set<std::string> query_display_ops{"MEMORY", "UTILIZATION"};
    std::unordered_set<std::string> query_gpu_ops{"timestamp", "driver_version", "count", "name", "gpu_name"};
    std::unordered_set<std::string> positionalOps {"dmon", "daemon", "replay", "pmon"};
    std::unordered_set<std::string> mutuallyMutex {"query", "query-gpu", "dmon", "daemon", "replay", "pmon"};
    
};

/**
 * Need add what you are want 
 */

