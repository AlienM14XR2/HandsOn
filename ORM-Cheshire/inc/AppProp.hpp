#ifndef APPPROP_H_
#define APPPROP_H_

#include <string>

struct AppProp {
    struct mysql {
        std::string uri;
        int port;
        std::string user;
        std::string password;
        std::string toServer() {
            std::string server("tcp://");
            server.append(uri).append(":").append(std::to_string(port));
            return server;
        }
    };
    struct mysqlx {
        std::string uri;
        int port;
        std::string user;
        std::string password;
    };
    struct pqxx {
        std::string uri;
        int port;
        std::string dbname;
        std::string user;
        std::string password;
        // "hostaddr=127.0.0.1 port=5432 dbname=jabberwocky user=derek password=derek1234"
        std::string toString() {
            std::string hostaddr("hostaddr=");
            std::string portNum(" port=");
            std::string d(" dbname=");
            std::string u(" user=");
            std::string p(" password=");
            hostaddr.append(uri);
            portNum.append(std::to_string(port));
            d.append(dbname);
            u.append(user);
            p.append(password);
            hostaddr.append(portNum).append(d).append(u).append(p);
            return hostaddr;
        }
    };
    AppProp::mysql my;
    AppProp::mysqlx myx;
    AppProp::pqxx pqx;
};

#endif
