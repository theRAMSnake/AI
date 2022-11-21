#pragma once
#include <string>
#include <sstream>
#include "pop.hpp"
#include "../../sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

namespace sori
{

class Database
{
public:
    Database(const std::string& filename);

    template<class T>
    T loadParameter(const std::string& name)
    {
        auto str = loadParameterStr(name);
        std::stringstream s(str);
        T result;
        s >> result;

        return result;
    }

    std::list<Pop> loadPops();
    TaskScores loadScores();

    template<class T>
    void saveParameter(const std::string& name, const T t)
    {
        saveParameterStr(name, std::to_string(t));
    }

    void savePops(const std::list<Pop>& pops);
    void saveScores(const TaskScores& scores);

private:
    std::string loadParameterStr(const std::string& name);
    void saveParameterStr(const std::string& name, const std::string& value);

    sqlite::database mDb;
};

}
