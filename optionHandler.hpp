#pragma once

#include <string>
#include <vector>
#include <memory>
#include <boost/program_options.hpp>
#include <unordered_set>

#include "test.hpp"

namespace po = boost::program_options;

class CreateSMIOptions;

// class OptionCheckHandler
// {
// public:
// public:
//     CreateSMIOptions* cso;
// public:
//     OptionCheckHandler(CreateSMIOptions* cso);
//     bool checkOptionConfilct( );
//     std::string getTargetOp( );

//     /**
//      * If you want check a option need with some plus option at end. 
//      * The function is used to ensure that the plus option of option can only come from descriptions
//      */
//     template<typename... Descriptions>
//     bool checkPlusInDescriptions(const std::string& option, const Descriptions&... descriptions) 
//     {
//         std::unordered_set<std::string> optionNames;
//         for (auto &&[key, _] : cso->getvm())
//         {
//             std::cout << "key = " << key << std::endl;
//             if (key != option)
//             {
//                 optionNames.insert(key); // not include itself
//             } 
//         }
        
//         auto checkOptions = [&](const auto& desc) {
//             for (const auto& option : desc.options()) 
//             {
//                 std::cout << "option key = " << option->long_name() << std::endl;
//                 if(optionNames.count(option->long_name()) != 0)
//                 {
//                     return false;
//                 }

//             }

//             return true;
//         };

//         return (checkOptions(descriptions) && ...);
//     }
//     /**
//      * return true means option conflict with 
//      * 
//      */

//     template<typename... Descriptions>
//     bool checkConflictInDescriptions(const std::string& option, const Descriptions&... descriptions) 
//     {
//         std::unordered_set<std::string> optionNames;
//         for (auto &&[key, _] : cso->getvm())
//         {
//             std::cout << "- key = " << key << std::endl;
//             if (key != option)
//             {
//                 optionNames.insert(key); // not include itself
//             } 
//         }
        
//         auto checkOptions = [&](const auto& desc) {
//             for (const auto& option : desc.options()) 
//             {
//                 std::cout << "- option key = " << option->long_name() << std::endl;
//                 if(optionNames.count(option->long_name()) != 0)
//                 {
//                     return true;
//                 }
    
//             }

//             return false;
//         };

//         return (checkOptions(descriptions) && ...);
//     }

// };

class OptionHandler
{
public:
    OptionHandler(CreateSMIOptions* cso);
    virtual void handler();
    
    virtual ~OptionHandler(){}
 
public:
    CreateSMIOptions* cso;
    // OptionCheckHandler* handler;
};

class OptionStartWithQueryOP : public OptionHandler
{
public:
    OptionStartWithQueryOP(CreateSMIOptions* cso) : OptionHandler(cso){}
    void handler();
};

class OptionQueryOP : public OptionHandler
{
public:
    OptionQueryOP(CreateSMIOptions* cso) : OptionHandler(cso) {}
    void handler();
};

class OptionPositionOP : public OptionHandler
{
public:
    OptionPositionOP( CreateSMIOptions* cso)  : OptionHandler(cso) {}
    void handler();
};

class OptionSummaryOP : public OptionHandler
{
public:
    OptionSummaryOP( CreateSMIOptions* cso)  : OptionHandler(cso) {}
    void handler();
};

class Context 
{
public:
    Context(std::shared_ptr<OptionHandler> optionHandler);
    ~Context() {}
    void setStrategy(std::shared_ptr<OptionHandler> optionHandler) {
        this->optionHandler = optionHandler;
    }
    void handler() const;
private:
    std::shared_ptr<OptionHandler> optionHandler;
};