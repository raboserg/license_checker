#include "stdafx.h"
#include <signal.h>

#include <pplx/threadpool.h>

#ifdef _WIN32
static const utility::string_t LIC_INI_FILE = U("lic_check_w.ini");
#else
static const utility::string_t LIC_INI_FILE = U("lic_check_l.ini");
#endif

enum {
  error_create_lic = 2,
};

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

utility::string_t create_verify_license_command() {
  //[LICENSE] check_lic_cmd = -v --lic
  //[FILES] lic_file_name = license.lic
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
    throw "Not create command varify license";
  }

  license_process_path.append(U(" "))
      .append(check_lic_cmd)
      .append(U(" "))
      .append(license_file_name);
  INFO_LOG(license_process_path.c_str());
  return license_process_path;
}

utility::string_t create_generate_machine_uid_command() {
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
    throw "Not create command generate machine uid";
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

    const utility::string_t license_process_path =
        create_verify_license_command();
    // verify license
    if (licenseChecker_->verify_license_file(license_process_path)) {
      // generate machine uid
      const utility::string_t machine_uid =
          licenseChecker_->generate_machine_uid(
              create_generate_machine_uid_command());
      INFO_LOG(machine_uid.c_str());
      DEBUG_LOG(TM("Need to get machine MAC"));

      // Need to create client here
    } else {
      INFO_LOG(TM("License is SUCCESS"));
      // end thread
    }
  } catch (boost::exception &ex) {
    utility::string_t str_utf16 =
        utility::conversions::to_string_t(boost::diagnostic_information(ex));
    ERROR_LOG(str_utf16.c_str());
    raise(SIGSEGV);
  } catch (std::system_error se) {
    if (se.code().value() == error_create_lic) {
      utility::string_t str_utf16 =
          utility::conversions::to_string_t(se.what());
      ERROR_LOG(str_utf16.c_str());
      raise(SIGSEGV);
    }
    //} catch (const utf16char *msg) {
  } catch (const std::exception &msg) {
    ERROR_LOG(utility::conversions::to_string_t(msg.what()).c_str());
    raise(SIGSEGV);
  }
}


const web::http::uri address = U("https://reqbin.com");
const utility::string_t path = U("/echo/post/json");

//{"login":"login","password":"password"}

int main_run() {
	// web::http::client::http_client sdfdsf("udfsds");
	// sdfdsf.request("sssss").
	
	//crossplat::threadpool::initialize_with_threads(40);
	
	// Create user data as JSON object and make POST request.
	auto postJson =

		pplx::create_task([]() {
		web::json::value request;

		web::http::client::http_client_config config;
		config.set_timeout(utility::seconds(30));

		//        request[U("file")] = web::json::value::string(U("file"));
		//        request[U("mac")] = web::json::value::string(U("mac"));
		//        request[U("unp")] = web::json::value::string(U("unp"));

		request[U("login")] = web::json::value::string(U("login"));
		request[U("password")] = web::json::value::string(U("password"));

		return web::http::client::http_client(address, config)
			.request(web::http::methods::POST,
				web::http::uri_builder().append_path(path).to_string(),
				request.serialize(), U("application/json"));
			})
		.then([](web::http::http_response response) { // Get the response.
			// Check the status code.
				ucout << response.to_string() << std::endl;
				ucout << response.status_code() << std::endl;
				if (response.status_code() == 201) {
					throw std::runtime_error("Returned " +
						std::to_string(response.status_code()));
				}
				// Convert the response body to JSON object.
				return response.extract_json();
			})
				.then([](web::json::value jsonObject) { // Parse the user details.
				ucout << jsonObject.to_string();
																								/*ucout << jsonObject[U("first_name")].as_string() << " "
								<< jsonObject[U("last_name")].as_string() << " ("
								<< jsonObject[U("id")].as_string() << ")" << std::endl;*/
					});

			return 0;
}


int main(int argc, const char *argv[]) {

  signal(SIGSEGV, posix_death_signal);

  // license_worker();
	main_run();

#ifdef _WIN32
  WinNT::Start_Service();
#else
  try {
    main_run();
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
