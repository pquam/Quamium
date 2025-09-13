
#include <iostream>
#include <ostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>


#include "server.h"


    Server::Server() {

        url = "https://patrick.quam.computer";
        scheme = "https";
        port = "443";
        getBody();

    }

    void Server::setInput(std::string input) {
        this->input = input;
    }


    std::string Server::getBody(bool refresh) {

        if (refresh) {
            url = parseInputToURL(input);
            std::cout << "Making HTTP request to: " << url << std::endl;
            body = httpGet(url);
            std::cout << "HTTP response body length: " << body.length() << std::endl;
        }
        
        return body;
    }

    /*
    TODO currently this resets the URL no matter what the input is.
    This should be updated to only update the URL if something changes.
    */
    std::string Server::parseInputToURL(std::string input) {

        std::cerr << "initial input: " + input << std::endl;

        int i = 0;

        if (input.find("://") != std::string::npos) {
            if (input[4] == 's') {
                scheme = "https";
                port = "443";
                i=8;
            }
            else {
                scheme = "http";
                port = "80";
                i=7;
            }

        }

        std::cerr << scheme << std::endl;

        host = "";
        for (; i < input.length(); i++) {

            if (input[i] == '/') {
                i++;
                break;
            }
            
            host += input[i];
        }

        std::cerr << " host " + host << std::endl;

        path = "/";
        for (;i < input.length(); i++) {

            if (input[i] == ':') {
                i++;
                break;
            }

            path += input[i];
        }

        std::cerr << "path " + path << std::endl;

        if (i < input.length()) {
            port = "";
            for (;i < input.length(); i++) {
                port += input[i];
            }
        }

        std::cerr << "port " + port << std::endl;       
        

        url = scheme + "://" + host + path;

        if (input == url) {
            std::cerr << " input and url match! " << std::endl;
        }
        std::cerr << "url: " + url << std::endl;

        return url;
    }


    namespace asio  = boost::asio;
    namespace ssl   = asio::ssl;
    namespace beast = boost::beast;
    namespace http  = beast::http;
    using tcp       = asio::ip::tcp;

    std::string Server::httpGet(std::string url) {

        asio::io_context ioc;

        // Resolver: host -> endpoints
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);

        if (scheme == "https") {
            // TLS context
            ssl::context ctx(ssl::context::tls_client);
    
            // Load system root certs (Linux/Unix); on some systems you might need to provide a bundle path
            ctx.set_default_verify_paths();
            ctx.set_verify_mode(ssl::verify_peer);
    
            // TCP connect
            beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
            beast::get_lowest_layer(stream).connect(results);
    
            // SNI (ServerNameIndication) for virtual hosts + cert matching
            if(!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                beast::error_code ec{static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category()};
                throw beast::system_error{ec};
            }
    
            // TLS handshake (client)
            stream.handshake(ssl::stream_base::client);
    
            // Build request
            http::request<http::empty_body> req{http::verb::get, path, httpVersion};
            req.set(http::field::host, host);
            req.set(http::field::user_agent, "Quamium");
            req.set(http::field::connection, "close");
    
            // Send
            http::write(stream, req);
    
            // Receive
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);
    
            // Graceful TLS shutdown
            beast::error_code ec;
            stream.shutdown(ec); // ignore EOF errors on some servers
    
            return res.body();
    
        } else {
            // Plain TCP (HTTP)
            beast::tcp_stream stream(ioc);
            stream.connect(results);
    
            http::request<http::empty_body> req{http::verb::get, path, httpVersion};
            req.set(http::field::host, host);
            req.set(http::field::user_agent, "beast-client");
            req.set(http::field::connection, "close");
    
            http::write(stream, req);
    
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);
    
            beast::error_code ec;
            if (stream.socket().shutdown(tcp::socket::shutdown_both, ec) 
                && ec != beast::errc::not_connected) {
                    std::cerr << "Shutdown error: " << ec.message() << "\n";
            }
    
            return res.body();
        }
    }