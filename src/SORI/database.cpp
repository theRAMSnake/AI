#include "database.hpp"

namespace sori
{

Database::Database(const std::string& filename)
    : mDb(filename)
{
    mDb << "CREATE TABLE IF NOT EXISTS params(name text primary key, value text)";
    mDb << "CREATE TABLE IF NOT EXISTS pops(id integer, pop blob)";
    mDb << "CREATE TABLE IF NOT EXISTS task_scores(name text, score integer)";
}

std::string Database::loadParameterStr(const std::string& name)
{
    std::string result;
    mDb << "SELECT value FROM params WHERE name = ?" << name >>
        [&result](std::string f1) {result = f1;};
    return result;
}

void Database::saveParameterStr(const std::string& name, const std::string& value)
{
    mDb << "INSERT OR REPLACE INTO params VALUES(?, ?)" << name << value;
}

std::list<Pop> Database::loadPops()
{
    std::list<Pop> result;
    mDb << "SELECT pop FROM pops ORDER BY id" >> [&result](std::vector<std::uint8_t> p) {
        result.push_back(popFromBinary(p));
    };

    return result;
}

TaskScores Database::loadScores()
{
    TaskScores result;

    mDb << "SELECT name, score FROM task_scores" >> [&result](std::string name, int score) {
        result[name] = score;
    };

    return result;
}

void Database::savePops(const std::list<Pop>& pops)
{
    mDb << "BEGIN";
    mDb << "DELETE FROM pops";
    auto ps = mDb << "INSERT INTO pops VALUES(?, ?)";
    int i = 0;
    for(const auto& p : pops)
    {
        ps << i << popToBinary(p);
        ++i;
        ps++;
    }
    mDb << "COMMIT";
}

void Database::saveScores(const TaskScores& scores)
{
    mDb << "BEGIN";
    mDb << "DELETE FROM task_scores";
    auto ps = mDb << "INSERT INTO task_scores VALUES(?, ?)";
    for(const auto& t : scores)
    {
        ps << t.first << t.second;
        ps++;
    }
    mDb << "COMMIT";
}

}
