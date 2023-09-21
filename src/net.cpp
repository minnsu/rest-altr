#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <regex>

#include <boost/asio/ssl.hpp>
#include <boost/json/src.hpp>
#include <boost/regex.hpp>

#include "include/net.hpp"

using namespace std;
using namespace boost::asio;

// net namespace ----------------------------------------------------------------------------------

// Transform json header to raw string header
string net::json2header(boost::json::object& header, string method, string path) {
    string header_str;
    header_str += method + " " + path + " HTTP/1.1" + "\r\n";
    for(auto it : header) {
        header_str += string(it.key().data()) + ":" + string(it.value().get_string()) + "\r\n";
    }
    header_str += "Content-Length:";
    return header_str;
}

// Transform json params(GET) to raw string(?a=a&b=b...)
string net::json2params(boost::json::object& params) {
    string params_str = "?";
    for(auto it : params) {
        params_str += string(it.key().data()) + "=" + string(it.value().get_string()) + "&";
    }
    params_str.pop_back();
    return params_str;
}

// Split raw packet to json header and body.
pair<boost::json::object, boost::json::object> net::packet_split(string& packet) {
    size_t pos = packet.find("\r\n\r\n");
    string header_str = packet.substr(0, pos);
    string body_str = packet.substr(pos+4);

    boost::json::object header;
    string line;
    string remain = header_str;
    size_t pos_h;
    while((pos_h = remain.find("\r\n")) != string::npos) {
        line = remain.substr(0, pos_h);
        remain = remain.substr(pos_h+2);

        if((pos_h = line.find(": ")) != string::npos) {
            header[line.substr(0, pos_h)] = line.substr(pos_h + 2);
        }
    }
    if((pos_h = remain.find(": ")) != string::npos) {
        header[remain.substr(0, pos_h)] = remain.substr(pos_h + 2);
    }

    boost::json::object body = boost::json::parse(body_str).as_object();
    return {header, body};
}

// Extract domain, port, and path from url.
void net::extract_from_url(string& url, string& domain, int& port, string& path) {
    regex regURL(R"((?:https?:\/\/)?([-a-zA-Z0-9]+(?:.[-a-zA-Z0-9])+):([1-9]+[0-9]*)(\/(?:[-a-zA-Z0-9]*\/?)+)*)");
    smatch ss;

    if(regex_match(url, ss, regURL)) {
        domain = ss[1].str();
        port = stoi(ss[2].str());
        path = ss[3].str();
    } else {
        fprintf(stderr, "URL format is not appropriate.\n");
        exit(1);
    }
}

// ------------------------------------------------------------------------------------------------

// net::https namespace ---------------------------------------------------------------------------

boost::asio::io_service net::https::io;
boost::asio::ip::tcp::resolver net::https::resolver(net::https::io);
ssl::context net::https::context(ssl::context::sslv23);

// Send raw packet.
string net::https::send_packet(string& domain, int port, string& packet) {    
    ssl::stream<ip::tcp::socket> socket(net::https::io, context);

    ip::tcp::resolver::query query(domain, to_string(port));
    ip::tcp::resolver::iterator iter = net::https::resolver.resolve(query);
    
    connect(socket.lowest_layer(), iter);
    socket.handshake(ssl::stream_base::client);
    try {
        write(socket, buffer(packet));
        
        boost::asio::streambuf response;
        read_until(socket, response, boost::regex("}$"));
        
        istream response_stream(&response);
        stringstream response_data;
        response_data << response_stream.rdbuf();

        return response_data.str();
    } catch(exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return "";
    }
}

// POST: lowest abstract level method
string net::https::post(string& domain, int port, string& header, string& body) {
    string packet = header + to_string(body.length()) + "\r\n\r\n" + body;
    return send_packet(domain, port, packet);
}

// POST: second abstract level method
string net::https::post(string& domain, int port, string& path, boost::json::object& header, boost::json::object& body) {
    string header_str = net::json2header(header, string("POST"), path);
    string body_str = boost::json::serialize(body);
    return post(domain, port, header_str, body_str);    
}

// POST: highest abstract level method
string net::https::post(string& url, boost::json::object& header, boost::json::object& body) {
    string domain;
    int port;
    string path;
    extract_from_url(url, domain, port, path);
    return post(domain, port, path, header, body);
}

// GET: lowest abstract level method
string net::https::get(string& domain, int port, string& header) {
    string packet = header + "0\r\n\r\n";
    return send_packet(domain, port, packet);
}

// GET: second abstract level method
string net::https::get(string& domain, int port, string& path, boost::json::object& header, boost::json::object& params) {
    string params_str = net::json2params(params);
    string path_with_params = path + params_str;
    string header_str = net::json2header(header, string("GET"), path_with_params);
    return get(domain, port, header_str);
}

// GET: highest abstract level method
string net::https::get(string& url, boost::json::object& header, boost::json::object& params) {
    string domain;
    int port;
    string path;
    extract_from_url(url, domain, port, path);
    cout << domain << " " << port << " " << path << endl;
    return get(domain, port, path, header, params);
}

// ------------------------------------------------------------------------------------------------


// net::websocket namespace -----------------------------------------------------------------------

net::websocket::websocket() : io(), resolver(this->io), socket(this->io) {};
net::websocket::websocket(string& domain, int port) : io(), resolver(this->io), socket(this->io) {
    if(this->connect(domain, port))
        cout << "Connect to " + domain + " has succeed." << endl;
    else
        cout << "Connect to " + domain + " has failed." << endl;
};

bool net::websocket::connect(string& domain, int port) {
    ip::tcp::resolver::query query(domain, to_string(port));
    ip::tcp::resolver::iterator iter = net::https::resolver.resolve(query);
    
    boost::system::error_code ec;
    boost::asio::connect(this->socket, iter, ec);
    if(ec) {
        cout << ec.message() << endl;
        return false;
    }
    return true;
}

string net::websocket::send(string& packet) {
    try {
        write(socket, buffer(packet));
        
        boost::asio::streambuf response;
        read_until(socket, response, boost::regex("}$"));
        
        istream response_stream(&response);
        stringstream response_data;
        response_data << response_stream.rdbuf();

        return response_data.str();
    } catch(exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return "";
    }
}

string net::websocket::send(string& header, string& body) {
    string packet = header + to_string(body.length()) + "\r\n\r\n" + body;
    cout << packet << endl;
    return net::websocket::send(packet);
}

string net::websocket::send(string& path, boost::json::object& header, boost::json::object& body) {
    string header_str = net::json2header(header, string("POST"), path);
    string body_str = boost::json::serialize(body);
    return net::websocket::send(header_str, body_str);
}


string net::websocket::read() {
    boost::system::error_code ec;
    this->socket.read_some(buffer(this->packet_buffer), ec);
    if(ec) {
        cout << ec.message() << endl;
        return "";
    }
    return string(this->packet_buffer);
}

// ------------------------------------------------------------------------------------------------