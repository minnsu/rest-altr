#include <string>

#include <boost/asio.hpp>
#include <boost/json/object.hpp>

#ifndef MYNET
#define MYNET
namespace mynet {
    class websocket{
    private:
    
    public:

    };
    
    extern boost::json::object default_header;
    extern boost::json::object default_tr_header;

    std::string json2header(boost::json::object& header, std::string method, std::string path);
    // json2body: Content-Type is always application/json, and just transform json to string.
    std::string json2params(boost::json::object& params);
    std::pair<boost::json::object, boost::json::object> packet_split(std::string& packet);

    class http {
    private:
        boost::asio::io_service io;
        boost::asio::ip::tcp::resolver resolver;

        std::string send_packet(std::string& domain, int port, std::string& packet);

    public:
        http();


        std::string post(std::string& domain, int port, std::string& header, std::string& body);
        std::string post(std::string& domain, int port, std::string& path, boost::json::object& header, boost::json::object& body);
        std::string post(std::string& url, boost::json::object& header, boost::json::object& body);

        std::string get(std::string& domain, int port, std::string& header);
        std::string get(std::string& domain, int port, std::string& path, boost::json::object& header, boost::json::object& params);
        std::string get(std::string& url, boost::json::object& header, boost::json::object& params);
    };
    
};
#endif