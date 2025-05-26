#pragma once

#include <sys/types.h>

#include "MessageBuffer.hpp"

class Client {
   private:
    Client();

    bool          _isAuth;
    bool          _login;
    int           _clientSock;
    int           _epollFd;
    int           _port;
    MessageBuffer _messageBuffer;
    std::string   _ip;
    std::string   _nickName;
    std::string   _realName;
    std::string   _userName;

    Client(const Client &copy);
    Client &operator=(const Client &copy);

   public:
    Client(int epoll_fd, int fd, const std::string &ip_address,
           const unsigned short port);
    ~Client();

    int         getSocket() const;
    std::string getIp() const;
    int         getPort() const;

    void        setUserName(const std::string &userName);
    std::string getUserName() const;

    void        setNickName(const std::string &nickName);
    std::string getNickName() const;

    void        setRealName(const std::string &realName);
    std::string getRealName() const;

    bool getIsAuth() const;
    void setIsAuth();

    bool getLoggedIn() const;
    void setLoggedIn();

    MessageBuffer &getMessageBuffer();

    bool ensure_auth() const;
    bool is_registered() const;
    bool ensure_registerd() const;

    int send_numeric_response(const std::string &prefix, int code) const;
    int send_detailed_numeric_response(const std::string &prefix,
                                       const std::string &details,
                                       int                code) const;

    std::string get_prefix() const;
    ssize_t     send(std::string msg) const;
};
