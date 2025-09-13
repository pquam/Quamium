#include <string>

class Server {
    private:

        const int httpVersion = 11;

        std::string input;
        std::string scheme;
        std::string host;
        std::string path;
        std::string port;
        std::string url;

        std::string headers;
        std::string body;

        //std::string getHeaders(bool refresh = false);
        //std::string parseBody();

    public:

        Server();

        void setInput(std::string);
        std::string parseInputToURL(std::string);


        /*
        determines if a content refresh is required.
        defaults to false.
        */ 
        std::string getBody(bool refresh = false);

        std::string httpGet(std::string);
        
};