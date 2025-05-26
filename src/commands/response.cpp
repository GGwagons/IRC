#include <sys/socket.h>

#include <iomanip>
#include <sstream>

#include "numerics.hpp"

int send_numeric_response(int fd, const std::string &prefix, int code) {
    std::string message = getMessage(code);

    std::ostringstream reply;
    reply << ":" << prefix << " " << std::setw(3) << std::setfill('0') << code
          << " :" << message << "\r\n";

    message = reply.str();
    return send(fd, message.c_str(), message.length(), 0);
}

int send_detailed_numeric_response(int fd, const std::string &prefix,
                                   const std::string &details, int code) {
    std::string message = getMessage(code);

    std::ostringstream reply;
    reply << ":" << prefix << " " << std::setw(3) << std::setfill('0') << code
          << " " << details << " :" << message << "\r\n";

    message = reply.str();
    return send(fd, message.c_str(), message.length(), 0);
}
