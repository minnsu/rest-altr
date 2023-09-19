#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <iostream>
#include <regex>

#include <boost/asio/ssl.hpp>
#include <boost/json/src.hpp>

#include "include/mynet.hpp"

using namespace std;
using namespace boost::asio;

// mynet namespace

boost::json::object mynet::default_header = {
    {"Content-Type", "application/json; charset=utf-8"},
    {"Host", ""},
};
boost::json::object mynet::default_tr_header = {
    {"Content-Type", "application/json; charset=utf-8"},
    {"Host", ""},
    {"authorization", "Bearer "},
    {"appkey", ""},
    {"appsecret", ""},
    {"tr_id", ""},
    {"tr_cont", ""},
    {"custtype", "P"},
};

string mynet::json2header(boost::json::object& header, string method, string path) {
    string header_str;
    header_str += method + " " + path + " HTTP/1.1" + "\r\n";
    for(auto it : header) {
        header_str += string(it.key().data()) + ":" + string(it.value().get_string()) + "\r\n";
    }
    header_str += "Content-Length:";
    return header_str;
}
string mynet::json2params(boost::json::object& params) {
    string params_str = "?";
    for(auto it : params) {
        params_str += string(it.key().data()) + "=" + string(it.value().get_string()) + "&";
    }
    params_str.pop_back();
    return params_str;
}

pair<boost::json::object, boost::json::object> mynet::packet_split(string& packet) {
    size_t pos = packet.find("\r\n\r\n");
    string header_str = packet.substr(0, pos);
    string body_str = packet.substr(pos+4);

    // header parsing
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

    // body parsing
    boost::json::object body = boost::json::parse(body_str).as_object();
    return {header, body};
}


// mynet::http::http class

mynet::http::http() : io(), resolver(io) {};

string mynet::http::send_packet(string& domain, int port, string& packet) {    
    ssl::context context(ssl::context::sslv23); // Why 'context' can not be a member variable? [Segmentation Fault Occured]
    ssl::stream<ip::tcp::socket> socket(this->io, context);

    ip::tcp::resolver::query query(domain, to_string(port));
    ip::tcp::resolver::iterator iter = this->resolver.resolve(query);
    
    connect(socket.lowest_layer(), iter);
    socket.handshake(ssl::stream_base::client);
    try {
        write(socket, buffer(packet));

        boost::asio::streambuf response;
        read_until(socket, response, "}");
        
        istream response_stream(&response);
        stringstream response_data;
        response_data << response_stream.rdbuf();

        return response_data.str();
    } catch(exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return "";
    }
}

void extract_from_url(string& url, string& domain, int& port, string& path) {
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

string mynet::http::post(string& domain, int port, string& header, string& body) { // POST: lowest abstract method
    string packet = header + to_string(body.length()) + "\r\n\r\n" + body;
    return send_packet(domain, port, packet);
}
string mynet::http::post(string& domain, int port, string& path, boost::json::object& header, boost::json::object& body) { // POST: second level abstract method
    string header_str = mynet::json2header(header, string("POST"), path);
    string body_str = boost::json::serialize(body);
    return post(domain, port, header_str, body_str);    
}
string mynet::http::post(string& url, boost::json::object& header, boost::json::object& body) { // POST: highest abstract method
    string domain;
    int port;
    string path;
    extract_from_url(url, domain, port, path);
    return post(domain, port, path, header, body);
}


string mynet::http::get(string& domain, int port, string& header) {
    string packet = header + "0\r\n\r\n";
    cout << "\n\n" + packet + "\n\n" << endl;
    return send_packet(domain, port, packet);
}
string mynet::http::get(string& domain, int port, string& path, boost::json::object& header, boost::json::object& params) {
    string params_str = mynet::json2params(params);
    string path_with_params = path + params_str;
    string header_str = mynet::json2header(header, string("GET"), path_with_params);
    return get(domain, port, header_str);
}
string mynet::http::get(string& url, boost::json::object& header, boost::json::object& params) {
    string domain;
    int port;
    string path;
    extract_from_url(url, domain, port, path);
    cout << domain << " " << port << " " << path << endl;
    return get(domain, port, path, header, params);
}