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
    auto target = std::string{};
    if (path == "/") {
      target = "./resources/index.html";
    } else {
      target = "./resources" + path;
    }

    std::ifstream ifs(target, std::ios::binary);
    if (!ifs.is_open())
      return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};

    auto ext = extensionOf(target);
    auto mimetype = std::string{"text/html"};
    if (ext == "ico") {
      mimetype = "image/x-icon";
    } else if (ext == "js") {
      mimetype = "application/javascript";
    } else if (ext == "css") {
      mimetype = "text/css";
    }

    auto body = std::string{};
    char buffer[1024];
    while (!ifs.eof()) {
      ifs.read(buffer, 1024);
      body.append(buffer, ifs.gcount());
    }
    return HttpResponse{"HTTP/1.1 200 OK", mimetype, body};
  });
}
