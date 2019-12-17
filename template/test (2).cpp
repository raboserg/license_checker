#include <Jinja2CppLight/Jinja2CppLight.h>
#include "NLTemplate.h"
#include <iostream>

#include "parser_ini.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace NL::Template;

static const auto path_to_ini =
    "d:/project/rabo/license_checker/resources/itvpn.ini";
static const auto path_to_templite =
    "d:/project/rabo/license_checker/template/templates/test.txt";
// static const auto path_to_templite = "templates/test.txt";

int main(int, char *[]) {

  Parser parser(path_to_ini);
  LoaderFile loader; // Let's use the default loader that loads files from disk.
  Template templater(loader);
  // Load & parse the main template and its dependencies.
  templater.load(path_to_templite);

  templater.set("text", "itvpn.ini"); // Set a top-level variable

  templater.block("items").repeat(38);

  int count = 0;
  for (auto it : parser.get_tree()) {
    cout << it.first << ", " << endl;
    templater.block("items")[count].set("detail", it.first);
    for (auto s : it.second) {
      cout << s.first << ", " << s.second.data() << endl;
      Block &block = templater.block("items")[count].block("detailblock");
      block.set("title", s.first);
      block.set("name", s.first);
      block.set("value", s.second.data());
      count++;
    }
    count++;
    cout << endl;
  }

  cout << "count: " << count << endl;

  //// Let's fill in the data for the repeated block.
  // for (int i = 0; i < 3; i++) {
  //  // Set title and text by accessing the variable directly
  //  templater.block("items")[i].set("title", titles[i]);
  //  templater.block("items")[i].set("text", "Lorem Ipsum");

  //  // We can get a shortcut reference to a nested block
  //  Block &block = templater.block("items")[i].block("detailblock");
  //  block.set("detail", details[i]);

  //  // Disable this block for the first item in the list. Can be useful for
  //  // opening/closing HTML tables etc.
  //  if (i == 0) {
  //    block.disable();
  //  }
  //}
   //Render the template with the variables we've set above
   templater.render(cout);

  ostringstream oss;
  templater.render(oss);
  cout << oss.str() << endl;

  std::string filename = "Test.html";
  std::ofstream ostrm(filename, std::ios::binary);
  templater.render(ostrm);

  // double d = 3.14;
  // ostrm.write(reinterpret_cast<char *>(&d), sizeof d); // binary output
  // ostrm << 123 << "abc" << '\n';                       // text output

  return 0;
}
