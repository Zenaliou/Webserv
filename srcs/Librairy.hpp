#ifndef LIBRAIRY_HPP
#define LIBRAIRY_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <unistd.h>        // read, write, close
#include <sys/types.h>
#include <sys/socket.h>    // socket, bind, listen, accept
#include <netinet/in.h>    // sockaddr_in
#include <arpa/inet.h>     // htons, inet_addr
#include <fcntl.h>         // fcntl
#include <poll.h>          // poll (ou <sys/epoll.h> si epoll)
#include <sys/stat.h>      // stat
#include <dirent.h>        // opendir (autoindex)
#include <cerrno>
#include <sys/wait.h>
#include <signal.h>

#endif