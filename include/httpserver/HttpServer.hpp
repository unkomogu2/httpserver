#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <common/StringUtils.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef __linux__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>  // for memset
#include <sys/socket.h>
#include <unistd.h>  // for close

#elif _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#endif

const size_t BUFFER_SIZE = 8192;

HttpRequest parseRequest(const std::string &recvData) {
  std::vector<std::string> headerbody = split(recvData, "\r\n\r\n");
  auto header = headerbody[0];
  auto lines = split(header, "\r\n");
  if (lines.empty()) return {};
  auto message = split(lines[0], " ");
  auto requestHeader = HttpRequestHeader{};
  requestHeader.method = message[0];
  requestHeader.path = message[1];
  requestHeader.version = message[2];
  size_t i = 1;
  while (lines[i].empty()) {
    auto pair = split(lines[i], ": ");
    requestHeader.headers[pair[0]] = pair[1];
    ++i;
  }
  auto body = headerbody[1];
  return HttpRequest{requestHeader, body};
}

struct HttpServer {
  HttpServer(unsigned short port) {
#ifdef _WIN32
    WSADATA data;
    WSAStartup(MAKEWORD(2, 0), &data);
#endif
    sockaddr_in srcAddr;
    memset(&srcAddr, 0, sizeof(srcAddr));
    srcAddr.sin_port = htons(port);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    const char yes = 1;
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    bind(m_socket, (sockaddr *)&srcAddr, sizeof(srcAddr));
    listen(m_socket, 1);
  }

  ~HttpServer() {
#ifdef _WIN32
    WSACleanup();
#endif
  }

  template <class RequestHandler>
  void run(RequestHandler &&handler) {
    int client;
    sockaddr_in dstAddr;
    socklen_t dstAddrSize = sizeof(dstAddr);

    while (1) {
      client = accept(m_socket, (sockaddr *)&dstAddr, &dstAddrSize);
      auto recvData = std::string{};
      char buffer[BUFFER_SIZE];
      int recvCount = recv(client, buffer, BUFFER_SIZE, 0);
      recvData.append(buffer, recvCount);

      // parse request
      auto request = parseRequest(recvData);

      // handle request
      HttpResponse resp = handler(request);

      // send response
      std::stringstream ss;
      ss << resp.message << "\r\n";
      ss << "Content-Length: " << resp.body.size() << "\r\n";
      ss << "Content-Type: " << resp.mimetype << "\r\n";
      ss << "\r\n";
      ss << resp.body;
      auto response = ss.str();
      send(client, response.data(), response.size(), 0);

#ifdef __linux__
      close(client);
#elif _WIN32
      closesocket(client);
#endif
    }
  }

  int m_socket;
};
