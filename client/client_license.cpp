// client_license.cpp
//
#include "client_license.h"

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
#include <pplx/threadpool.h>

/*
requestDTO
{
  "file": "string",
  "mac": "string",
  "unp": "string"
}
*/
// const web::http::uri address = U("https://jsonplaceholder.typicode.com/");
// const utility::string_t path = U("/posts");

const web::http::uri address = U("https://reqbin.com");
const utility::string_t path = U("/echo/post/json");

//{"login":"login","password":"password"}

int main_run() {
  // crossplat::threadpool::initialize_with_threads(1);
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
