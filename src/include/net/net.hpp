#include <string>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <boost/json/object.hpp>

#ifndef NET_HPP
#define NET_HPP
namespace net {
    std::string json2header(boost::json::object& header, std::string method, std::string path);
    std::string json2params(boost::json::object& params);

    void extract_from_url(std::string& url, std::string& domain, int& port, std::string& path);
    std::pair<boost::json::object, boost::json::object> packet_split(std::string& packet);

    namespace https {
        extern boost::asio::io_service io;
        extern boost::asio::ip::tcp::resolver resolver;
        extern boost::asio::ssl::context context;
        std::string send_packet(std::string& domain, int port, std::string& packet);

        std::string post(std::string& domain, int port, std::string& header, std::string& body);
        std::string post(std::string& domain, int port, std::string& path, boost::json::object& header, boost::json::object& body);
        std::string post(std::string& url, boost::json::object& header, boost::json::object& body);

        std::string get(std::string& domain, int port, std::string& header);
        std::string get(std::string& domain, int port, std::string& path, boost::json::object& header, boost::json::object& params);
        std::string get(std::string& url, boost::json::object& header, boost::json::object& params);
    };

    class websocket {
    private:
        boost::asio::io_service io;
        boost::asio::ip::tcp::resolver resolver;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws;

    public:
        websocket();
        websocket(std::string& domain, int port);

        bool connect(std::string& domain, int port);
        std::string send(std::string& packet);
        std::string read();

        void pong(std::string& ping);
        bool close();
    }; 
};
#endif