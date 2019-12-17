#include <Jinja2CppLight/Jinja2CppLight.h>
#include <iostream>

#include "parser_ini.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace Jinja2CppLight;

#ifdef _WIN32
static const auto path_to_ini =
    "d:/project/rabo/license_checker/resources/itvpn.ini";
static const auto path_to_templite =
    "d:/project/rabo/license_checker/template/templates/test.txt";
#else
static const auto path_to_ini =
    "/home/user/projects/cpp/license_checker/resources/itvpn.ini";
static const auto path_to_templite =
    "/home/user/projects/cpp/license_checker/template/templates/test.txt";
#endif

// static const auto path_to_templite = "templates/test.txt";
struct item {
  string title;
  string name;
  string value;
};

int main(int, char *[]) {
  Parser parser(path_to_ini);

  std::string content = std::string();
  std::ifstream is_(path_to_templite, std::ios::in | std::ios::binary);
  if (!is_) {
    char buf[512];
    while (is_.read(buf, sizeof(buf)).gcount() > 0)
      content.append(buf, is_.gcount());
  }

  cout << "content: " << content << endl;
  string sfdsf = R"DELIM(
  {% for i in range(3) %}
    a[{{i}}] = image[{{i}}];
  {% endfor %}
  )DELIM";

  string source = R"DELIM(<!DOCTYPE html>
                  <html lang="en">
                  <head>
                    <title>My Webpage</title>
                  </head>
                  <body>
                  <form accept-charset="UTF-8" action="/" autocomplete="off" method="POST">
                    <table class="blueTable">
                      <tfoot>
                        <tr>
                          <td colspan="4">
                            <button type="submit" value="Submit">Save</button></div>
                          </td>
                        </tr>
                      </tfoot>
                      <tbody>
                        <tr>
                          <td>
                          </td>
                        </tr>
                        {% for item in items %}
                        <tr>
                          <td><label for="name">{{ item }}</label><br /></td>
                          <td><input name={{ item }} type="text" value={{ item }} /> <br /> </td>
                        </tr>
                        {% endfor %}
                      </tbody>
                    </table>
                  </form>
                  </body>
                  </html>)DELIM";

  Template myTemplate(source);

  item item_;
  item_.name = "dsfdsfd";
  item_.title = "ssfdf";
  item_.value = "sfsfsdfs";
  // std::vector<item> values = {"item", "item_", "item_"};

  // myTemplate.s .setValue("items", values);
  //  myTemplate.setValue("avalue", 3);
  //  myTemplate.setValue("secondvalue", 12.123f);
  //  myTemplate.setValue("weather", "rain");
  myTemplate.setValue("items", TupleValue::create("item", "item_", "item_"));

  string result = myTemplate.render();

  cout << "result: " << result << endl;

  int count = 0;
  for (auto it : parser.get_tree()) {
    cout << it.first << ", " << endl;
    for (auto s : it.second) {
      cout << s.first << ", " << s.second.data() << endl;
      count++;
    }
    count++;
    cout << endl;
  }
  cout << "count: " << count << endl;
  return 0;
}
