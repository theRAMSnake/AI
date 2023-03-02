#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>
#include "gacommon/rng.hpp"

#include "Host.hpp"

using boost::asio::ip::tcp;

Host gHost;

template<class T>
T getOrThrow(const boost::property_tree::ptree tr, const std::string& key)
{
    try
    {
        return tr.get<T>(key);
    }
    catch(...)
    {
        throw std::runtime_error("Cannot find: " + key);
    }
}
std::string to_string(const std::vector<std::byte>& val)
{
    std::string result;
    // Not efficient, but we do not care
    for(const auto& v : val)
    {
        result += std::to_string(static_cast<int>(v)) + " ";
    }

    return result;
}

std::string to_string(const HostState& src)
{
    switch(src)
    {
        case HostState::Empty: return "Empty";
        case HostState::Ready: return "Ready";
        case HostState::Running: return "Running";
        case HostState::Stopping: return "Stopping";
    }

    return "Uknown";
}

std::string executeOperation(const boost::property_tree::ptree& tr)
{
    auto opName = getOrThrow<std::string>(tr, "operation");
    if(opName == "getState")
    {
        return to_string(gHost.getState());
    }
    else if(opName == "listProjects")
    {
        return boost::algorithm::join(gHost.listProjects(), ", ");
    }
    else if(opName == "createProject")
    {
        auto name = getOrThrow<std::string>(tr, "name");

        gHost.createProject(name);

        return "Done";
    }
    else if(opName == "loadProject")
    {
        auto name = getOrThrow<std::string>(tr, "name");
        gHost.loadProject(name);
        return "Done";
    }
    else if(opName == "deleteProject")
    {
        auto name = getOrThrow<std::string>(tr, "name");
        gHost.deleteProject(name);
        return "Done";
    }
    else if(opName == "run")
    {
        auto time = getOrThrow<int>(tr, "minutes");

        gHost.run({static_cast<size_t>(time)});

        return "Done";
    }
    else if(opName == "stop")
    {
        gHost.stop();
        return "Done";
    }
    else if(opName == "set")
    {
        auto key = getOrThrow<std::string>(tr, "key");
        auto value = getOrThrow<std::string>(tr, "value");

        gHost.set(key, value);

        return "Done";
    }
    else if(opName == "recentStats")
    {
        return gHost.printLatestStatistics();
    }
    else if(opName == "getConfig")
    {
        auto result = gHost.getConfig();
        std::stringstream str;
        boost::property_tree::write_json(str, result);
        return str.str();
    }
    else
    {
        return "Unknown operation";
    }
}

std::string handleMessage(const std::string& input)
{
    try
    {
        boost::property_tree::ptree tree;
        std::stringstream str(input);
        boost::property_tree::read_json(str, tree);
        return executeOperation(tree);
    }
    catch(std::exception& ex)
    {
        return "Error: " + std::string(ex.what());
    }
}

class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    static boost::shared_ptr<Connection> create(boost::asio::io_service& ioService)
    {
        return boost::shared_ptr<Connection>(new Connection(ioService));
    }

    tcp::socket& socket()
    {
        return mSocket;
    }

    void start()
    {
        boost::asio::async_read(mSocket, boost::asio::buffer(mInputHeader.data(), mInputHeader.size()),
            boost::bind(&Connection::handleReadHeader, shared_from_this(), boost::asio::placeholders::error));
    }

private:
    void handleWrite(const boost::system::error_code err)
    {
    }

    void handleRead(const boost::system::error_code err)
    {
        if (err)
        {
        }
        else
        {
            std::cout << "Input message: " << mInputBuffer << std::endl;
            auto responce = handleMessage(mInputBuffer);
            if(responce.size() > 4294967294)
            {
                std::cout << "Responce size is to big, send error instead" << std::endl;
                responce = "Error: responce is to big";
            }
            mOutBuffer.resize(responce.size() + 4);
            mOutBuffer[0] = static_cast<unsigned int>(responce.size()) & 0xFF;
            mOutBuffer[1] = (static_cast<unsigned int>(responce.size()) >> 8) & 0xFF;
            mOutBuffer[2] = (static_cast<unsigned int>(responce.size()) >> 16) & 0xFF;
            mOutBuffer[3] = (static_cast<unsigned int>(responce.size()) >> 24) & 0xFF;
            std::copy(responce.begin(), responce.end(), mOutBuffer.begin() + 4);

            boost::asio::async_write(mSocket, boost::asio::buffer(mOutBuffer),
                boost::bind(&Connection::handleWrite, shared_from_this(), boost::asio::placeholders::error));
        }
    }

    void handleReadHeader(const boost::system::error_code err)
    {
        if (err)
        {
        }
        else
        {
            mInputBuffer.resize(static_cast<unsigned int>(mInputHeader[0]) | static_cast<unsigned int>(mInputHeader[1]) << 8);
            std::cout << "Input header: " << mInputBuffer.size() << std::endl;
            boost::asio::async_read(mSocket, boost::asio::buffer(mInputBuffer.data(), mInputBuffer.size()),
                boost::bind(&Connection::handleRead, shared_from_this(), boost::asio::placeholders::error));
        }
    }

    Connection(boost::asio::io_service& ioService)
        : mSocket(ioService)
    {
        mInputHeader.resize(2);
    }

    tcp::socket mSocket;
    std::string mInputBuffer;
    std::string mOutBuffer;
    std::string mInputHeader;
};

class Server
{
    // Message format for this server: 16bit length + content
public:
    Server(boost::asio::io_service& ioService)
        : mAcceptor(ioService, tcp::endpoint(tcp::v4(), 38539))
        , mIoService(ioService)
    {
        startAccept();
    }
private:
    void startAccept()
    {
        auto newConn = Connection::create(mIoService);
        mAcceptor.async_accept(newConn->socket(), boost::bind(&Server::handleAccept, this, newConn, boost::asio::placeholders::error
            ));
    }

    void handleAccept(boost::shared_ptr<Connection> conn, const boost::system::error_code err)
    {
        if(!err)
        {
            conn->start();
        }

        startAccept();
    }

    tcp::acceptor mAcceptor;
    boost::asio::io_service& mIoService;
};

int main()
{
    Rng::seed(time(0));
    try
    {
        boost::asio::io_service service;
        Server s(service);
        service.run();
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
