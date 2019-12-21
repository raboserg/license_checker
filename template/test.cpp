#include <iostream>
#include <jinja2cpp/reflected_value.h>
#include <jinja2cpp/template.h>
#include <jinja2cpp/template_env.h>

#include "parser_ini.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace jinja2;

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
// Enum declaration description
struct EnumDescriptor {
  // Enumeration name
  std::string enumName;
  // Namespace scope prefix
  std::string nsScope;
  // Collection of enum items
  std::vector<std::string> enumItems;
};

struct Item {
  string title;
  string name;
  string value;
};

// Enum declaration description
struct ItemDescriptor {
  // Enumeration name
  std::string name;
  // Namespace scope prefix
  std::string nsScope;
  // Collection of enum items
  std::vector<Item> items;
};

struct ToJsonIndentationTest {
  ValuesMap getObjectParam() const {
    const ValuesMap object{{"map", ValuesMap{{"array", ValuesList{1, 2, 3}}}}};
    return ValuesMap{{"obj", object}};
  }
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
  std::string source_1 = R"(
<!doctype html>
<html>
  <head>
    <title>{{ page_title }}</title>
  </head>
  <body>
    <div class="header">
      <h1>{{ page_title }}</h1>
    </div>
    <ul class="navigation">
    {% for href, caption in [
        {'href'='index.html', 'caption'='Index'},
        {'href'='downloads.html', 'caption'='Downloads'},
        {'href'='products.html', 'caption'='Products'}
      ] %}
      <li><a href="{{ href }}">{{ caption }}</a></li>
    {% endfor %}
    </ul>
    <div class="table">
      <table>
      {% for row in table %}
        <tr>
        {% for cell, num in row|list %}
          <td>{{ cell }} {{ num }}</td>
        {% endfor %}
        </tr>
      {% endfor %}
      </table>
    </div>
  </body>
</html>
)";

  string source = R"(<!DOCTYPE html>
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
                          <td><label for="name">{{ item.name }}</label><br /></td>
                          <td><input name={{ item.name }} type="text" value={{ item.value }} /> <br /> </td>
                        </tr>
                        {% endfor %}
                      </tbody>
                    </table>
                  </form>
                  </body>
                  </html>)";

  // Template myTemplate(source);

  EnumDescriptor descr;
  descr.enumName = "Animals";
  descr.nsScope = "";
  descr.enumItems = {"Dog", "Cat", "Monkey", "Elephant"};

  // ValuesMap params1 {
  //    {"enumName", descr.enumName},
  //    {"nsScope", descr.nsScope},
  //    {"items", {descr.enumItems}},
  //};

  Item item_;
  item_.name = "dsfdsfd";
  item_.value = "sfsfsdfs";

  ItemDescriptor itemDescriptor;
  itemDescriptor.name = "sfsdfds";
  itemDescriptor.nsScope = "gfdgdfgfd";
  itemDescriptor.items = {item_, item_, item_, item_, item_};

  ValuesMap params;
  //{
  //    {"enumName", itemDescriptor.name},
  //    {"nsScope", itemDescriptor.nsScope},
  //    {"items1", ValuesMap{{"name", "fdsf"}, {"value", "dsfdsf"}}},
  //    //{"items", Reflect(std::vector<Item>{item_, item_, item_})},
  //};

  TemplateEnv env_;
  env_.GetSettings().lstripBlocks = false;
  env_.GetSettings().trimBlocks = false;

  Template tpl(&env_);
  tpl.Load(source);

  std::cout << tpl.RenderAsString(params).value() << std::endl;

  params["page_title"] = "mitsuhiko's benchmark";

  // ValuesList dictEntry = {item_, item_, item_, item_, item_,
  //                        item_, item_, item_, item_, item_};

  // ValuesMap params{
  //    {"enumName", item_.name},
  //    {"nsScope", item_.title},
  //    {"items", {{item_}, {item_}}},
  //};

  /*ValuesMap dictEntry = {{"a", 1}, {"b", 2}, {"c", 3}, {"d", 4},
   {"e", 5}, {"f", 6}, {"g",7}, {"h", 8}, {"i", 9}, {"j", 10}};*/
  // jinja2::ValuesList dictEntry = {"a", "b", "c", "d", "e",
  //                                "f", "g", "h", "i", "j"};
  ValuesList table;

  for (int n = 0; n < 10; ++n)
    table.push_back(Value(params));

  params["table"] = std::move(table);

  std::cout << tpl.RenderAsString(params).value() << std::endl;

  /*for (int n = 0; n < 5000; ++n)
    tpl.RenderAsString(params).value();*/
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
