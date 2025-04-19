#include "optionHandler.hpp"

#include <string_view>
#include <format>
#include <vector>

constexpr std::string_view error_with_plus = "ERROR: Option --{} is not valid for this command. Please run sdc-smi -h for help";
constexpr std::string_view error_with_op = "ERROR: Option --{} is not recognized. Please run sdc-smi -h for help";


OptionHandler::OptionHandler(CreateSMIOptions* cso) : cso{cso}
{
    
}

void OptionHandler::handler()
{

}

void OptionStartWithQueryOP::handler( )
{
    if (cso->getvm().count("query-gpu"))
    {
        // cso->updatevm("query-gpu", cso->unrecognized_options, cso->selectQueryMandatoryOptions, false);
        // if (!cso->getvm().count("format"))
        // {
        //     std::cout << "--format= switch is missing, Please run sdc-smi -h for help\n";
        // }


        // cso->updatevm("query-gpu", cso->unrecognized_options, cso->selectQueryPlusOptions, true, cso->selectQueryMandatoryOptions);
        
        // std::cout << "format = " << cso->getvm()["format"].as<std::string>() << std::endl;
        // std::cout << "filename = " << cso->getvm()["filename"].as<std::string>() << std::endl;
        // std::cout << "loop = " << cso->getvm()["loop"].as<int>() << std::endl;
    }
}

void OptionQueryOP::handler()
{
    if (cso->getvm().count("query")) 
    {
        cso->updatevm("query", cso->unrecognized_options, cso->queryOptionsPlus);
        std::cout << "delay = " << cso->getvm()["loop"].as<int>() << std::endl;
    }
    
}

void OptionPositionOP::handler()
{
    auto firstArgs = cso->getFirstArgs();
    if (!firstArgs.empty())
    {
        std::cout << "firstArgs = " << firstArgs << std::endl;
        if (firstArgs == "dmon")
        {
            cso->updatevm(firstArgs, cso->unrecognized_options, cso->dmonOptions);
            if (cso->getvm().count("help"))
            {
                cso->dmonOptions.print(std::cout, 0);
            }
            
            cso->debugInfo();
            std::cout << "id = " << cso->getvm()["id"].as<int>() << std::endl;
            std::cout << "delay = " << cso->getvm()["delay"].as<int>() << std::endl;
            std::cout << "count = " << cso->getvm()["count"].as<int>() << std::endl;
        }
        else if (firstArgs == "pmon")
        {
            /* code */
        }
        
        
        
    }
    

}

void OptionSummaryOP::handler()
{
    if (cso->getvm().size() == 0)
    {
        cso->updatevm("summary", cso->unrecognized_options, cso->summaryOptions);
        std::cout << "loop = " << cso->getvm()["loop"].as<int>() << std::endl;
    }
}
    
    

Context::Context(std::shared_ptr<OptionHandler> optionHandler) : optionHandler{optionHandler}
{}

void Context::handler() const
{
    if (optionHandler)
    {
        optionHandler->handler();
    }
    
}