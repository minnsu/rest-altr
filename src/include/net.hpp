#include <string>

#include <boost/asio.hpp>
#include <boost/json/object.hpp>

namespace net {
    class websocket{
    private:
    
    public:

    };
    std::string json2header(boost::json::object& header, std::string method, std::string path);
    std::string json2params(boost::json::object& params);

    void extract_from_url(std::string& url, std::string& domain, int& port, std::string& path);
    std::pair<boost::json::object, boost::json::object> packet_split(std::string& packet);

    namespace https {
        extern boost::asio::io_service io;
        extern boost::asio::ip::tcp::resolver resolver;
        std::string send_packet(std::string& domain, int port, std::string& packet);

        std::string post(std::string& domain, int port, std::string& header, std::string& body);
        std::string post(std::string& domain, int port, std::string& path, boost::json::object& header, boost::json::object& body);
        std::string post(std::string& url, boost::json::object& header, boost::json::object& body);

        std::string get(std::string& domain, int port, std::string& header);
        std::string get(std::string& domain, int port, std::string& path, boost::json::object& header, boost::json::object& params);
        std::string get(std::string& url, boost::json::object& header, boost::json::object& params);
    };
};