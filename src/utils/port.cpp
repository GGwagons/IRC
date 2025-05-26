#include <sstream>

int parse_port(const std::string &raw) {
    for (size_t i = 0; i < raw.length(); i++) {
        if (isdigit(raw[i])) continue;

        return -1;
    }

    std::stringstream stream(raw);
    int               port;

    stream >> port;

    if (stream.fail()) return -1;
    if (!stream.eof()) return -1;

    if (port < 1024 || port > 65535) return -1;

    return port;
}
