#ifndef DEMO_HTTP1HEADER_HPP_
#define DEMO_HTTP1HEADER_HPP_

#include <vector>
#include <string>
#include <map>
#include <sstream>


class HTTP1_Header {
public:
    static std::vector<std::string>& make_response404();
    static std::vector<std::string>& make_response200(int body_length);
    static std::vector<std::string>& make_response302(std::string path);
    static std::vector<std::string>& make_responseUpgrade();
};

#endif  // DEMO_HTTP1HEADER_HPP_
