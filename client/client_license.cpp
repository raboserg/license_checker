// client_license.cpp
//
#include "client_license.h"

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>
//#include <pplx/threadpool.h>


//#if (defined(_MSC_VER) && (_MSC_VER >= 1800)) && !CPPREST_FORCE_PPLX
//class direct_executor : public pplx::scheduler_interface {
//public:
//	virtual void schedule(concurrency::TaskProc_t proc, _In_ void *param) {
//		proc(param);
//	}
//};
//
//static std::shared_ptr<pplx::scheduler_interface> g_executor;
//std::shared_ptr<pplx::scheduler_interface> __cdecl get_scheduler() {
//	if (!g_executor) {
//		g_executor = std::make_shared<direct_executor>();
//	}
//
//	return g_executor;
//}
//#else
//std::shared_ptr<pplx::scheduler_interface> __cdecl get_scheduler() {
//	return pplx::get_ambient_scheduler();
//}
//#endif
//
//class TaskOptionsTestScheduler : public pplx::scheduler_interface {
//public:
//	TaskOptionsTestScheduler() : m_numTasks(0), m_scheduler(get_scheduler()) {}
//
//	virtual void schedule(pplx::TaskProc_t proc, void *param) {
//		pplx::details::atomic_increment(m_numTasks);
//		m_scheduler->schedule(proc, param);
//	}
//
//	long get_num_tasks() { return m_numTasks; }
//
//private:
//	pplx::details::atomic_long m_numTasks;
//	pplx::scheduler_ptr m_scheduler;
//
//	TaskOptionsTestScheduler(const TaskOptionsTestScheduler &);
//	TaskOptionsTestScheduler &operator=(const TaskOptionsTestScheduler &);
//};

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
  //TaskOptionsTestScheduler sched;
	//long n = 0;
	//auto t1 = pplx::create_task([&n]() { n++; }, sched); // run on sched
	//t1.wait();

  auto postJson =

      pplx::create_task(
          []() {
            
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
            //ucout << jsonObject.to_string();
            /*ucout << jsonObject[U("first_name")].as_string() << " "
                  << jsonObject[U("last_name")].as_string() << " ("
                  << jsonObject[U("id")].as_string() << ")" << std::endl;*/
          });

  return 0;
}
