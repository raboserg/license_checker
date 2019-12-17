#include "http_server.h"

static const auto port = U("9090");
static const auto path = U("/res");
static const auto host = U("http://localhost");

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

utility::string_t build_address(const int argc,
                                const std::vector<utility::string_t> &argvec) {
  utility::string_t addr = argvec[0];
  const utility::string_t port = argvec[1];
  const utility::string_t path = argvec[2];

  ucout << addr.c_str() << ", " << port.c_str() << ", " << path.c_str()
        << std::endl;

  addr.append(utility::conversions::to_string_t(U(":") + argvec[1]));
  return uri_builder(addr).append_path(path).to_uri().to_string();
}

#ifdef _WIN32
int wmain(int argc, wchar_t *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  utility::string_t address;
  if (argc == 4) {
    const std::vector<utility::string_t> argvec = {
        utility::conversions::to_string_t(argv[1]),
        utility::conversions::to_string_t(argv[2]),
        utility::conversions::to_string_t(argv[3])};
    address = build_address(argc, argvec);
  } else {
    const std::vector<utility::string_t> argvec = {host, port, path};
    address = build_address(argc, argvec);
  }

  on_initialize(address);

  std::cout << "Press ENTER to exit." << std::endl;
  std::string line;
  std::getline(std::cin, line);
  on_shutdown();
  return 0;
}
