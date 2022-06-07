#ifndef SRCS_HTTP_CONST_CONST_ERROR_CONTENTS_HPP
#define SRCS_HTTP_CONST_CONST_ERROR_CONTENTS_HPP

#include <string>

const std::string content_400 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>400</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>400 Bad Request</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_403 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>403</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>403 Forbidden</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_404 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>404</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>404 Not Found</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_405 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>405</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>405 Method Not Allowed</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_408 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>408</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>408 Request Timeout</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_500 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>500</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>500 Internal Server Error</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_501 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>501</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>501 Not Implemented</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_505 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>505</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>505 HTTP Version Not Supported</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_520 = "<!DOCTYPE html>\n"
                                "<html>\n"
                                "    <head>\n"
                                "        <title>520</title>\n"
                                "    </head>\n"
                                "    <body>\n"
                                "<h2>520 Unknown Error</h2>\n"
                                "default error page\n"
                                "    </body>\n"
                                "</html>";

const std::string content_301 = "<html>\n"
                                "    <head>"
                                "        <title>301 Moved Permanently</title>"
                                "    </head>\n"
                                "<body>\n"
                                "<center><h1>301 Moved Permanently</h1></center>\n"
                                "default page\n"
                                "</body>\n"
                                "</html>";

#endif /* SRCS_HTTP_CONST_CONST_ERROR_CONTENTS_HPP */
