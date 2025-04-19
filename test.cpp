#include "test.hpp"
#include "optionHandler.hpp"

#include <memory>
#include <optional>
#include <algorithm>
#include <format>
#include <iomanip>

/**
 * TODO
 * Need add value check
 * This file to large need split
 * 
 */


 /**
  * space num is 34
  * TODO may need a interface to change this value.
  */
static void help_optionalPosOp_output()
{
    std::cout << "Device Monitoring:\n";
    std::cout << "dmon" << std::setw(30) << " " << "Displays devices stats in scrolling format.\n" << std::setw(34) << " " << "\"sdc-smi dmon -h\" for more information.\n";
    std::cout << "daemon" << std::setw(28) << " " << "Runs in backsground and monitor devices as a daemon process.\n" << std::setw(34) << " " << "This is an experimental feature. Not supported on Windows baremetal " << "\"sdc-smi daemon -h\" for more information." << std::endl;
}

static void help_query_gpu_output()
{
    // TODO need add more info for support
    std::cout << "\"timestamp\"\nThe timestamp of when the query was made in fomat \"YYYY/MM/DD HH:MM:SS.mes.\"\n" << std::endl;
    std::cout << "\"driver_version\"\nThe version of the installed SDC driver. This is an alphanumeric string.\n" << std::endl;
    std::cout << "\"count\"\nThe number of SDC GPUs in the system.\n" << std::endl;
    std::cout << "\"name\" or \"gpu_name\"\nThe official product name of teh GPU. This is an alphanumeric string. For all products.\n" << std::endl;
    std::cout << "\"serial\" or \"gpu_serial\"\nThis number matches the serial number physocally printed on each board. It is a globally unique immutable alphanumeric value.\n" << std::endl;
    std::cout << "\"uuid\" or \"gpu_uuid\"\nThis value is the globally unique immutable alphanumeric identifier of the GPU. It does not correspond to any physical label on the board.\n" << std::endl;
}

void ParseCmdLine::addOptionsToDesc(po::options_description& desc, const std::vector<std::tuple<std::string, po::value_semantic*, std::string>>& optionTuples)
{
    for (const auto& tuple : optionTuples)
    {
        const auto& name = std::get<0>(tuple);
        auto valueSemantic = std::get<1>(tuple);
        const auto& description = std::get<2>(tuple);

        if (valueSemantic) {
            desc.add_options()(name.c_str(), valueSemantic, description.c_str());
        } else {
            desc.add_options()(name.c_str(), description.c_str());
        }
    }
}

void CreateSMIOptions::createCmdLineLine()
{
    // create main description
    pcmdline->addOptionsToDesc(mainDesc, {{"help,h", nullptr, "produce help message"}, {"version", nullptr, "input file"}});

    // create list option
    // listOptions = pcmdline->createDescription(std::string{"List OPTIONS"});
    pcmdline->addOptionsToDesc(listOptions, {{"list-gps,L", nullptr, "Display a list of GPUs connected to the system"}, {"list-excluded-gpus", nullptr, "Display a list of excluded GPUs in the system"}});

    // create summary Options
    pcmdline->addOptionsToDesc(summaryOptions, {
        {"id,i", po::value<std::vector<int>>()->multitoken()->notifier([&, this](const auto& values){
            for (auto &&value : values)
            {
                if (value + 1 > pcmdline->getDeviceCount())
                {
                    throw po::validation_error(po::validation_error::invalid_option_value, "--id", "error");
                }
                
            }
            
        }), "Target a specific GPU"}, 
        {"filename,f", po::value<std::string>(), "Log to a specified file, rather than to stdout"}, 
        {"loop,l", po::value<int>(), "Probe until Ctrl+C at specified second interval"}
        }
    );
    
    // create query options / plus any of options
    pcmdline->addOptionsToDesc(queryOptions, {{"query,q", nullptr, "Display GPU or Unit info.\n"}});

    pcmdline->addOptionsToDesc(queryOptionsPlus, {
                                                    {"id,i", po::value<std::vector<int>>()->multitoken()->notifier([&, this](const auto& values){
                                                        for (auto &&value : values)
                                                        {
                                                            if (value + 1 > pcmdline->getDeviceCount())
                                                            {
                                                                throw po::validation_error(po::validation_error::invalid_option_value, "--id", "error");
                                                            }
                                                            
                                                        }
                                                        
                                                    }), "Target a specific GPU"},
                                                    {"unit,u", nullptr, "Show unit, rather than GPU, attributes"}, {"id,i", po::value<std::vector<int>>(), "Target a specific GPU or Unit"},
                                                    {"filename,f", po::value<std::string>(), "Log to a specified file, ranther than to stdout"},
                                                    {"xml-format,x", nullptr, "When showing xml output"},
                                                    {
                                                        "display,d", po::value<std::vector<std::string>>()->multitoken()/*->notifier([&](const auto& values)
                                                        {
                                                            try
                                                            {
                                                                for (auto &&value : values)
                                                                {
                                                                    if(query_display_ops.find(value) == query_display_ops.end())
                                                                    {
                                                                        throw po::validation_error(po::validation_error::invalid_option_value, "--display", value);
                                                                    }
                                                                }
                                                            }
                                                            catch(const po::validation_error& e)
                                                            {
                                                                //std::cout << std::format("Failed to parse --{} flags\n", e.get_option_name());
                                                                exit(1);
                                                            }
                                                        })*/, "Display only selected information: MEMORY, UTILIZATION.\n Doesn't work with -u or -x flags"
                                                    },
                                                    {"loop,l", po::value<int>(), "Probe until Ctrl+C at specified second interval"},
                                                    {"loop-ms", po::value<int>(), "Probe until Ctrl+C at specified millisecond interval"}
                                                });
    
    // create select query options
    pcmdline->addOptionsToDesc(selectQueryOptions, {{
        "query-gpu", po::value<std::vector<std::string>>()->multitoken()->notifier([&](const auto& values){
            try
            {
                for(auto&& value : values)
                {
                    if (query_gpu_ops.find(value) == query_gpu_ops.end())
                    {
                        //throw po::validation_error(po::validation_error::invalid_option_value, "--query-gpu", value);
                    }   
                }
            }
            catch(const po::error& e)
            {
                std::cerr << "Error: " << e.what() << std::endl;
                std::cout << std::format("Failed to parse  flags\n");
                exit (1);
            }
            
        }), "Information about GPU.\n Call --help-query-gpu for more info."
    }});
    
    
    // create select query options Mandatory
    pcmdline->addOptionsToDesc(selectQueryMandatoryOptions, {{"format", po::value<std::string>()->required(), "Specify the output format"}});

    // create select query options plus
    pcmdline->addOptionsToDesc(selectQueryPlusOptions, {{
        {"id,i", po::value<std::vector<int>>()->notifier([this](const auto& idx){
            try
            {
                for(auto& id : idx)
                {
                    if(id + 1 > pcmdline->getDeviceCount())
                    {
                        throw po::validation_error(po::validation_error::invalid_option_value, "--id", "error");
                    }
                }
                
            }
            catch(const po::validation_error& e)
            {
                std::cout << std::format("Failed to parse --{} flags\n", e.get_option_name());
                exit (1);
            }
            
        }), "Target a specific GPU"},
        {"filename,f", po::value<std::string>(), "Log to a specified file, rather than to stdout"},
        {"loop,l", po::value<int>(), "Probe until Ctrl+C at specified second interval"},
        {"loop-ms", po::value<int>(), "Probe until Ctrl+C at specified millisecond interval"}
    }});
               

       
    // create Hidden Options which include help-query-gpu, help-query-compute-apps, help-query-compute-apps and so on
    // TODO need more hidden options which feature supported.
    hiddenOptions.add_options()
        ("help-query-gpu", "List of valid properties to query for the switch \"--query-gpu\"");
    
    //dmon [plus any of] option
    pcmdline->addOptionsToDesc(dmonOptions,{{
        {"id,i",    po::value<int>()->value_name("[-i | --id]"), "Comman separated Enumeration index, PCI bus ID or UUID"},
        {"delay,d", po::value<int>()->value_name("[-d | --delay]")->default_value(1), "Collection delay/interval in seconds [default=1sec]"},
        {"count,c", po::value<int>()->value_name("[-c | --count]"), "Collection specified number of samples and exit"},
        {"help,h", po::value<int>()->value_name("[-h | --help]"), "Display format specifiers."}
    }});


    pcmdline->addAllOptions(allDesc, mainDesc, listOptions, queryOptions, selectQueryOptions, hiddenOptions, dmonOptionsHelp);
}

void CreateSMIOptions::parseCmdLineArgs(int argc, char** argv)
{
    using namespace std::literals;
    try
    {
        createCmdLineLine();
        std::string firstArgs{argv[1]};
        if (!firstArgs.starts_with("--"s) && !firstArgs.starts_with("-"s))
        {
            positionalArgs = argv[ 1];
            if (positionalOps.count(positionalArgs) == 0)
            {
                std::cerr << "Not Support this \n";
                exit(0);
            }
        }
        
        auto parser = positionalArgs.empty() ? po::command_line_parser(argc, argv) : po::command_line_parser(argc - 1, argv + 1);
        auto parsed = parser.options(allDesc).allow_unregistered().style(po::command_line_style::unix_style).run();
        po::store( parsed, vm);
        po::notify(vm);

        unrecognized_options = po::collect_unrecognized(parsed.options, po::include_positional);

        if(checkOptionsHasConflict())
        {
            std::cout << std::format("Please run sdc-smi -h for help\n");
            exit(1);
        }
        
        std::shared_ptr<OptionHandler> optionStartWithQueryOP = std::make_shared<OptionStartWithQueryOP>(this);
        std::shared_ptr<OptionHandler> optionQueryOP = std::make_shared<OptionQueryOP>(this);
        std::shared_ptr<OptionHandler> optionPositionOP = std::make_shared<OptionPositionOP>(this);
        std::shared_ptr<OptionHandler> optionalSummaryOP = std::make_shared<OptionSummaryOP>(this);

        Context context(optionStartWithQueryOP);
        context.handler();
        context.setStrategy(optionQueryOP);
        context.handler();
        context.setStrategy(optionPositionOP);
        context.handler();
        context.setStrategy(optionalSummaryOP);
        context.handler();

        if (vm.count("help")) 
        {
            if(getVMSize() > 1)
            {
                std::cout << "Please run \"sdc-smi --help\\-h\" get more help." << std::endl;
                exit(1);
            }
            helpOptionFunction();
            help_optionalPosOp_output();
            exit(0);
        }

        if (vm.count("list-gps") || vm.count("list-excluded-gpus")) 
        {
            if(getVMSize() > 1)
            {
                std::cout << "Please run \"sdc-smi --help\\-h\" get more help." << std::endl;
                exit(1);
            }
        }
    }
    catch(const po::unknown_option& ue)
    {
        std::cout << "---" << ue.what() << std::endl;
    }
    catch(const po::error& e)
    {
        std::cout << "---" << e.what() << std::endl;
    }
}

/**
 * TODO need support with the [plus any of]
 * eg. sdc-smi -q & sdc-smi -q -i xx -f xx -d xx 
 * these two different show different format
 * 
 * A better design is needed to distinguish the display. this is just POC
 */
std::string CreateSMIOptions::displayFormat()
{
    if (vm.count("query"))
    {
        return "query";
    }
    else if (vm.count("query-gpu")) // TODO need support Regular expression matching
    {
        return "query-gpu";
    }
    else
    {
        return "summary";
    }
    
    return std::string{""};
}

std::tuple<bool, std::unordered_map<std::string, std::any>> CreateSMIOptions::showSummaryTable()
{
    std::unordered_map<std::string, std::any> m;
    m["id"] = std::vector<int>{};
    m["filename"] = std::string{""};
    m["loop"] = int(0); //ms
    for (const auto& option : summaryOptions.options())
    {
        if (auto name = option->long_name(); vm.count(name))
        {
            if (name == "id")
            {
                m[name] = vm[name].as<std::vector<int>>();
                auto mk = vm[name].as<std::vector<int>>();
                for (auto &&v : mk)
                {
                    std::cout << "v = " << v << std::endl;
                }
                
            }
            else if(name == "filename")
            {
                m[name] = vm[name].as<std::string>();
            }
            else if (name == "loop")
            {
                m[name] = vm[name].as<int>();
            }
            
            
            
        }
    }

    return std::make_tuple(true, m); 
}

/**
 * TODO think there is no same sample item
 * 
 */
std::vector<std::string> CreateSMIOptions::parseQuerySampleItem(std::vector<std::string> items)
{
    std::vector<std::string> s_;
    for (auto &&item : items)
    {
        std::replace(item.begin(), item.end(), '.', '_');
        s_.push_back(item);
    }
    return s_;
}

/**
 * TODO need using boost::any to storage the value
 * so there not need convert the value with type before insert
 * into the map
 */
std::tuple<bool, std::unordered_map<std::string, std::any>> CreateSMIOptions::showQuery()
{
    std::unordered_map<std::string, std::any> m;
    
    m["id"] = std::vector<int>{};
    m["filename"] = std::string{""};
    m["loop"] = 0; //ms
    m["display"] = std::vector<std::string>{};
    for (const auto& option : queryOptionsPlus.options())
    {
        if (auto name = option->long_name(); vm.count(name))
        {
            if (name == "id")
            {
                m[name] = vm[name].as<std::vector<int>>();
            }
            else if(name == "filename")
            {
                m[name] = vm[name].as<std::string>();
            }
            else if (name == "loop")
            {
                m[name] = vm[name].as<int>();
            }
            else if (name == "display")
            {
                m[name] = vm[name].as<std::vector<std::string>>();  
            }
        }
    }
    
    return std::make_tuple(true, m);
}

std::tuple<bool, std::unordered_map<std::string, std::any>> CreateSMIOptions::showSelectQuery()
{
    std::unordered_map<std::string, std::any> m;
    /**
     * TODO Now only support query-gpu
     * 
     */
    // if (!vm.count("query-gpu"))
    // {
    //     return {false,m};
    // }

    // if(!checkOptionsOnlyHaveDesc(queryOptionsPlus))
    // {
    //     return {false, m};
    // }

    m["id"] = std::vector<int>{};
    m["filename"] = "";
    m["loop"] = 0; //ms
    for (const auto& option : selectQueryPlusOptions.options())
    {
        if (vm.count(option->long_name()))
        {
            m[option->long_name()] = vm[option->long_name()];
        }
        
    }
    
    return {true, m};
}