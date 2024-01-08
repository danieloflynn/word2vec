#ifndef MONGODB_HANDLER_H
#define MONGODB_HANDLER_H

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class MongoDb_handler
{
public:
    MongoDb_handler(std::string URI);
    std::string getEnvironmentalVariable(const std::string &envVarKey);
    std::vector<std::string> getDatabases(const mongocxx::client &client);
    void createCollection(mongocxx::database &db, const std::string collectionName);
    void createDocument(mongocxx::collection &collection, const bsoncxx::document::value &document);
    void printCollection(mongocxx::collection &collection);
    void findDocument(mongocxx::collection &collection, const std::string &key, const std::string &value);
    void updateDocument(mongocxx::collection &collection, const std::string &key, const std::string &value, const std::string &newKey, const std::string &newValue);
    void deleteDocument(mongocxx::collection &collection, const bsoncxx::document::value &document);
};

#endif