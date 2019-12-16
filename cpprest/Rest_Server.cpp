#include "http_server.h"
#include "usr_interrupt_handler.hpp"

static std::unique_ptr<http_server> g_http_server;

void on_initialize(const utility::string_t &addr) {

  g_http_server = std::unique_ptr<http_server>(new http_server(addr));
  g_http_server->open().wait();

  ucout << utility::string_t(U("Listening for requests at: ")) << addr
        << std::endl;

  return;
}

void on_shutdown() {
  g_http_server->close().wait();
  return;
}

utility::string_t build_address(const int argc, char **argv) {

  utility::string_t path = U("/res");
  utility::string_t port = U("9090");
  utility::string_t addr = U("http://localhost");

  if (argc == 4) {
    addr = argv[1];
    port = argv[2];
    path = argv[3];
  }

  addr.append(utility::conversions::to_string_t(U(":") + port));
  uri_builder uri(addr);
  uri.append_path(path);

  ucout << addr.c_str() << ", " << port.c_str() << ", " << path << std::endl;

  return uri.to_uri().to_string();
}

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  cfx::InterruptHandler::hookSIGINT();

  on_initialize(build_address(argc, argv));

  std::cout << "Press Ctrl-C to exit." << std::endl;
  cfx::InterruptHandler::waitForUserInterrupt();
  //  std::cout << "Press ENTER to exit." << std::endl;
  //  std::string line;
  //  std::getline(std::cin, line);
  on_shutdown();
  return 0;
}
