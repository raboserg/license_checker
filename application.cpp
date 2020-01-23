#include "stdafx.h"
#include <signal.h>

//#include <pplx/threadpool.h>

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

enum {
  error_create_lic = 2, // mybe create process
};

////////////////////////////////////////////////////////////////
#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) && !CPPREST_FORCE_PPLX
class direct_executor : public pplx::scheduler_interface {
public:
  virtual void schedule(concurrency::TaskProc_t proc, _In_ void *param) {
    proc(param);
  }
};

static std::shared_ptr<pplx::scheduler_interface> g_executor;
std::shared_ptr<pplx::scheduler_interface> __cdecl get_scheduler() {
  if (!g_executor) {
    g_executor = std::make_shared<direct_executor>();
  }
  return g_executor;
}
#else
std::shared_ptr<pplx::scheduler_interface> __cdecl get_scheduler() {
  return pplx::get_ambient_scheduler();
}
#endif

class TaskOptionsTestScheduler : public pplx::scheduler_interface {
public:
  TaskOptionsTestScheduler() : m_numTasks(0), m_scheduler(get_scheduler()) {}

  virtual void schedule(pplx::TaskProc_t proc, void *param) {
    pplx::details::atomic_increment(m_numTasks);
    m_scheduler->schedule(proc, param);
  }

  long get_num_tasks() { return m_numTasks; }

private:
  pplx::details::atomic_long m_numTasks;
  pplx::scheduler_ptr m_scheduler;

  TaskOptionsTestScheduler(const TaskOptionsTestScheduler &);
  TaskOptionsTestScheduler &operator=(const TaskOptionsTestScheduler &);
};
////////////////////////////////////////////////////////////////

utility::string_t input_handle();

typedef utils::Singleton<Parser> PARSER;

void posix_death_signal(int signum) {
  //  const std::error_category fdsfsd = utility::details::linux_category();
  //  std::cout << fdsfsd << std::endl;
  CRITICAL_LOG(TM("ABORTED"));
  signal(signum, SIG_DFL); // resend signal
  ucout << input_handle();
  lic::os_utilities::sleep(1000);
  exit(3);
}

utility::string_t make_verify_license_cmd() {
  //[LICENSE] check_lic_cmd = -v --lic
  //[FILES] lic_file_name = license_item.lic
  //[FILES] lic=D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);
  utility::string_t license_process_path = parser_->get_value(U("FILES.lic"));
  const utility::string_t check_lic_cmd =
      parser_->get_value(U("LICENSE.check_lic_cmd"));
  const utility::string_t license_file_name =
      parser_->get_value(U("FILES.lic_file_name"));

  if (license_process_path.empty() && check_lic_cmd.empty() &&
      license_file_name.empty()) {
    throw std::runtime_error("Not create command varify license");
  }

  license_process_path.append(U(" "))
      .append(check_lic_cmd)
      .append(U(" "))
      .append(license_file_name);
  INFO_LOG(license_process_path.c_str());
  return license_process_path;
}

utility::string_t make_generate_machine_uid_cmd() {
  //[LICENSE] prod = 2
  //[LICENSE] make_lic_cmd = -g -s --prod
  //[FILES]	lic = D:\work\itvpn_setup\itvpn\bin\x64\lic.exe
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);
  utility::string_t license_process_path;
  license_process_path = parser_->get_value(U("FILES.lic"));
  const utility::string_t make_lic_cmd =
      parser_->get_value(U("LICENSE.make_lic_cmd"));
  const utility::string_t license_prod = parser_->get_value(U("LICENSE.prod"));
  const utility::string_t license_uid =
      parser_->get_value(U("LICENSE.license_uid"));

  if (license_process_path.empty() && make_lic_cmd.empty() &&
      license_prod.empty()) {
    throw std::runtime_error("Not create command generate machine uid");
  }

  license_process_path.append(U(" "))
      .append(make_lic_cmd)
      .append(U(" "))
      .append(license_prod);
  INFO_LOG(license_process_path.c_str());
  return license_process_path;
}

void license_worker() {
  try {
    const std::unique_ptr<LicenseChecker> licenseChecker_ =
        std::make_unique<LicenseChecker>();
    // verify license_item
    const bool is_license_update =
        licenseChecker_->verify_license_file(make_verify_license_cmd());
    if (is_license_update) {
      // generate machine uid
      const utility::string_t machine_uid =
          licenseChecker_->generate_machine_uid(
              make_generate_machine_uid_cmd());
      INFO_LOG(machine_uid.c_str());

      DEBUG_LOG(TM("Need to get machine MAC"));
      //////////////////////////////////////
      // Need to create client here
      //////////////////////////////////////
    } else {
      INFO_LOG(TM("License is SUCCESS"));
      // end thread
    }
  } catch (const boost::exception &ex) {
    utility::string_t str_utf16 =
        utility::conversions::to_string_t(boost::diagnostic_information(ex));
    ERROR_LOG(str_utf16.c_str());
    raise(SIGSEGV);
  } catch (const std::system_error se) {
    if (se.code().value() == error_create_lic) {
      utility::string_t str_utf16 =
          utility::conversions::to_string_t(se.what());
      ERROR_LOG(str_utf16.c_str());
      raise(SIGSEGV);
    }
  } catch (const std::exception &msg) {
    ERROR_LOG(utility::conversions::to_string_t(msg.what()).c_str());
    raise(SIGSEGV);
  }
}

// const web::http::uri address = U("https://reqbin1.com");
// const utility::string_t path = U("/echo/post/json");
const web::http::uri address = U("http://192.168.105.69");
const utility::string_t path =
    U("/license-manager/rest/host/get-host-licenses");

int main_run_1() {

  web::http::client::http_client_config config;
  config.set_validate_certificates(false);
  config.set_timeout(utility::seconds(60));

  /*config.set_ssl_context_callback(
      [](boost::asio::ssl::context &context) -> void {
        context.load_verify_file(std::string("./snicloudflaresslcom.crt"));
      });*/

  web::json::value request_data;
  web::http::http_response response;
  web::http::client::http_client client(address, config);

  request_data[U("file")] = web::json::value::string(U("file"));
  request_data[U("mac")] = web::json::value::string(U("mac"));
  request_data[U("unp")] = web::json::value::string(U("unp"));

  /*const utility::string_t url =
      web::http::uri_builder().append_path(path).to_string();*/
  // DEBUG_LOG(client.base_uri().query().c_str());
  while (true) {
    try {
      const utility::string_t content_type = U("application/json");
      response =
          client
              .request(web::http::methods::POST,
                       web::http::uri_builder().append_path(path).to_string(),
                       request_data.serialize(), content_type)
              .get();

      utility::string_t host_ = client.base_uri().host();
      utility::string_t path_ = client.base_uri().path();
      utility::string_t query_ = client.base_uri().query();

      if (response.status_code() == 200) {
        ucout << response.to_string() << std::endl;
        ucout << response.status_code() << std::endl;
      }
      break;
    } catch (web::http::http_exception &ex) {
      ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
    }
  }
  return 0;
}

enum sdfdsf {
  NEW_HOST = 1,   // "Новый хост";
  HOST_SUSPENDED, // "Хост приостановлен";
  NO_LICENSE,     // "Лицензии ещё нет"
  LICENSE_ISSUE   // "Выпуск лицензии";
};

web::json::value make_request_message() {
  const std::unique_ptr<Parser> parser_ =
      std::make_unique<Parser>(LIC_INI_FILE);

  const std::unique_ptr<LicenseChecker> licenseChecker_ =
      std::make_unique<LicenseChecker>();

  // get mac
  const utility::string_t mac = parser_->get_value(U("LICENSE.mac"));
	lic::license_constanst::FILES_lic;


  // get unp
  const utility::string_t unp = parser_->get_value(U("LICENSE.unp"));
  // generate machine uid
  const utility::string_t uid =
      licenseChecker_->generate_machine_uid(make_generate_machine_uid_cmd());

  INFO_LOG(uid.c_str());

  web::json::value message;
  message[U("unp")] = web::json::value::string(unp);
  message[U("request")] = web::json::value::string(uid);
  message[U("mac")] = web::json::value::string(mac);
  // message[U("mac")] = web::json::license_value::string(U("mac"));
  return message;
}

web::json::value connect() {

  std::chrono::seconds time_try_connection_ = 2s;

  /*const web::http::uri address =
      U("http://192.168.105.69/license-manager/rest/host/get-host-licenses");*/

  const web::http::uri address =
      U("http://192.168.105.70/license-manager/rest/host/get-host-licenses1");

  web::http::client::http_client_config config;
  config.set_validate_certificates(false);
  config.set_timeout(utility::seconds(65));

  const web::json::value message = make_request_message();
  TRACE_LOG(message.to_string().c_str());

  web::http::client::http_client client(address, config);
  web::http::http_request request(web::http::methods::POST);
  request.set_body(message.serialize(), U("application/json"));

  web::http::http_response response;
  web::json::value license_value = web::json::value::null();

  auto start = std::chrono::steady_clock::now();
  for (;;) {
    try {
      response = client.request(request).get();
      break;
    } catch (web::http::http_exception &ex) {
      ucout << ex.error_code().value() << std::endl; // 12029
      if (std::chrono::steady_clock::now() > (start + time_try_connection_)) {
        ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
        std::throw_with_nested(std::runtime_error(ex.what()));
      }
    }
  }

  bool status = response.status_code() == web::http::status_codes::OK;
  if (status) {
    response.content_ready().wait();
    license_value = response.extract_json().get();
    ucout << response.to_string() << std::endl;
    TRACE_LOG(response.to_string().c_str());
  } else {
    utility::string_t error_msg(U("Fault connection: status code - "));
    error_msg.append(utility::conversions::to_string_t(
        std::to_string(response.status_code())));
    //???ucout << response.extract_json().get() << std::endl;
    ERROR_LOG(error_msg.c_str());
  }
  return license_value;
}

web::json::value receive_license() {

  const web::http::uri address =
      U("http://192.168.105.69/license-manager/rest/host/get-host-licenses");

  // const web::http::uri address =
  //    U("http://192.168.105.69/license-manager/rest/host/get-host-licenses1");

  web::http::client::http_client_config config;
  config.set_validate_certificates(false);
  config.set_timeout(utility::seconds(65));

  web::json::value message = make_request_message();

  web::http::client::http_client client(address, config);
  web::http::http_request request(web::http::methods::POST);
  request.set_body(message.serialize(), U("application/json"));

  web::json::value license_value = web::json::value::null();

  for (;;) {
    try {
      web::http::http_response response = client.request(request).get();
      bool status = response.status_code() == web::http::status_codes::OK;
      if (status) {
        response.content_ready().wait();
        license_value = response.extract_json().get();
        ucout << response.to_string() << std::endl;
        TRACE_LOG(response.to_string().c_str());
        break;
      } else {
        utility::string_t error_msg(U("Fault connection: status code - "));
        error_msg.append(utility::conversions::to_string_t(
            std::to_string(response.status_code())));
        ERROR_LOG(error_msg.c_str());
        break;
      }
    } catch (std::invalid_argument &ex) {
      ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
    } catch (web::http::http_exception &ex) {
      ucout << ex.error_code().value() << std::endl;
      // 12029
      ERROR_LOG(utility::conversions::to_string_t(ex.what()).c_str());
      if (ex.error_code().value() == 0)
        break;
    }
  }
  return license_value;
}

void license_parser() {
  try {
    web::json::value license_value = connect();
    if (license_value.is_null()) {

    } else {
      if (license_value[U("hostLicenses")].is_null()) {
        //"Хост приостановлен";
        //"Лицензии ещё нет";
        TRACE_LOG(license_value.to_string().c_str());
      } else {
        web::json::array licenses = license_value[U("hostLicenses")].as_array();
        web::json::value license_item = licenses[licenses.size() - 1];
        const utility::string_t license =
            license_item[U("license")].as_string();
        const utility::string_t license_exp_date =
            license_item[U("licenseExpirationDate")].as_string();
        utility::string_t license_msg(U("data: ") + license_exp_date +
                                      U("; lic: ") + license);
        TRACE_LOG(license_msg.c_str());

        // save license to file
      }
    }
  } catch (const std::exception &ex) {
    std::throw_with_nested(std::runtime_error(ex.what()));
  } catch (const boost::exception &ex) {
    std::throw_with_nested(
        std::runtime_error(boost::diagnostic_information(ex)));
  }
}

int main(int argc, const char *argv[]) {

  setlocale(LC_ALL, "ru_RU.UTF-8");
  signal(SIGSEGV, posix_death_signal);

  try {
    license_parser();
  } catch (const std::runtime_error &err) {
    ERROR_LOG(utility::conversions::to_string_t(err.what()).c_str());
  }
  // license_worker();
  // main_run_1();
  // receive_license();
  // license_parser();
  // main_run();
  // lic::os_utilities::sleep(1000);
#ifdef _WIN32
  WinNT::Start_Service();
#else
  try {
    main_run_1();
  } catch (lic::license_exception &ex) {
    //    ERROR_LOG(utility::conversions::to_string_t(std::string(ex.what())));
    std::cout << ex.what() << std::endl;
  }

  //  LinuxNoficitator linuxNoficitator_;
  //  linuxNoficitator_.run_notify(argc, argv);

#endif
  ucout << input_handle();
  return 0;
}

utility::string_t input_handle() {
  utility::string_t cmd;
  ucout << U("Input <Enter> for close") << std::endl;
  getline(ucin, cmd);
  return cmd;
}
