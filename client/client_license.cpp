// client_license.cpp
//
#include "client_license.h"

int main_run() {
  // Create user data as JSON object and make POST request.
  auto postJson =
      pplx::create_task([]() {
        web::json::value jsonObject;
        jsonObject[U("first_name")] = web::json::value::string(U("atakan"));
        jsonObject[U("last_name")] = web::json::value::string(U("sarioglu"));

        return web::http::client::http_client(U("http://localhost:9090"))
            .request(web::http::methods::POST,
                     web::http::uri_builder(U("http://localhost:9090"))
                         .append_path(U("/res"))
                         .to_string(),
                     jsonObject.serialize(), U("application/json"));
      })
          .then([](web::http::http_response response) { // Get the response.
            // Check the status code.
            ucout << response.to_string() << endl;
            ucout << response.status_code() << endl;
            if (response.status_code() != 201) {
              throw std::runtime_error("Returned " +
                                       std::to_string(response.status_code()));
            }
            // Convert the response body to JSON object.
            return response.extract_json();
          })
          .then([](web::json::value jsonObject) { // Parse the user details.
            ucout << jsonObject[U("first_name")].as_string() << " "
                  << jsonObject[U("last_name")].as_string() << " ("
                  << jsonObject[U("id")].as_string() << ")" << std::endl;
          });

  return 0;
}
