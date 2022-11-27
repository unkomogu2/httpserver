#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <HttpServer.hpp>
#include <fstream>
#include <iostream>

const int port = 8000;

int main(int argc, char const* argv[]) {
  std::cout << "Server running on http://localhost:" << port << std::endl;
  HttpServer server(port);
  server.run([](const HttpRequest& request) {
    const auto path = request.header.path;
    if (path.empty())
      return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};

    if (path == "/") {
      auto target = "./resources/index.html";
      std::ifstream ifs(target, std::ios::binary);
      if (!ifs.is_open())
        return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};

      auto body = std::string{};
      char buffer[1024];
      while (!ifs.eof()) {
        ifs.read(buffer, 1024);
        body.append(buffer, ifs.gcount());
      }
      return HttpResponse{"HTTP/1.1 200 OK", "text/html", body};

    } else {
      auto target = "./resources" + path;
      std::ifstream ifs(target, std::ios::binary);
      if (!ifs.is_open())
        return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};

      auto mimetype = std::string{"text/html"};
      if (endsWith(target, ".ico")) {
        mimetype = "image/x-icon";
      } else if (endsWith(target, ".js")) {
        mimetype = "application/javascript";
      } else if (endsWith(target, ".css")) {
        mimetype = "text/css";
      }

      auto body = std::string{};
      char buffer[1024];
      while (!ifs.eof()) {
        ifs.read(buffer, 1024);
        body.append(buffer, ifs.gcount());
      }
      return HttpResponse{"HTTP/1.1 200 OK", mimetype, body};
    }
  });
}
