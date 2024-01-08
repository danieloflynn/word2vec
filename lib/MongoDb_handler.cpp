#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "MongoDb_handler.h"

MongoDb_handler::MongoDb_handler(std::string environmentalVarKey)
{
    std::string URIStr = getEnvironmentalVariable(environmentalVarKey);
    mongocxx::instance instance{};
}

std::string MongoDb_handler::getEnvironmentalVariable(const std::string &envVarKey)
{

    auto key = envVarKey.c_str();
    // Use the secure version of getenv, ie. _dupenv_s to fetch environment variable.
    if (getenv(key) != nullptr)
    {
        std::string environmentVarValue(getenv(key));
        std::cout << environmentVarValue << '/n';
        return environmentVarValue;
    }
    else
    {
        std::cout << "Value not found" << '\n';
        return "";
    }
}