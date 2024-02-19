#include <arrow/api.h>
#include <arrow/array/builder_binary.h>
#include <arrow/type_fwd.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> archers{"Legolas", "Oliver", "Merida", "Lara",
                                 "Artemis"};
std::vector<std::string> locations{"Mirkwood", "Star City", "Scotland",
                                   "London", "Greece"};
std::vector<int16_t> years{1954, 1941, 2012, 1996, -600};

int main(int argc, char *argv[]) {
  // // building a struct that references child arrays
  arrow::ArrayVector children;
  children.resize(3);

  arrow::StringBuilder str_bldr;
  str_bldr.AppendValues(archers);
  str_bldr.Finish(&children[0]);

  str_bldr.AppendValues(locations);
  str_bldr.Finish(&children[1]);

  arrow::Int16Builder year_bldr;
  year_bldr.AppendValues(years);
  year_bldr.Finish(&children[2]);

  arrow::StructArray arr{
      arrow::struct_({arrow::field("archer", arrow::utf8()),
                      arrow::field("location", arrow::utf8()),
                      arrow::field("year", arrow::int16())}),
      children[0]->length(), children};

  std::cout << arr.ToString() << std::endl;

  // alternatively, building the child arrays from the struct builder
  // define a struct type
  std::shared_ptr<arrow::DataType> st_type =
      arrow::struct_({arrow::field("archer", arrow::utf8()),
                      arrow::field("location", arrow::utf8()),
                      arrow::field("year", arrow::int16())});

  // create a structbuilder for the type
  std::unique_ptr<arrow::ArrayBuilder> tmp;
  {
    auto status =
        arrow::MakeBuilder(arrow::default_memory_pool(), st_type, &tmp);
    if (!status.ok()) {
      std::cerr << status.message() << std::endl;
      return 1;
    }
  }
  std::shared_ptr<arrow::StructBuilder> builder;
  builder.reset(static_cast<arrow::StructBuilder *>(tmp.release()));

  // extract child array builders from the struct builder
  using arrow::Int16Builder;
  using arrow::StringBuilder;
  StringBuilder *archer_builder =
      static_cast<StringBuilder *>(builder->field_builder(0));
  StringBuilder *location_builder =
      static_cast<StringBuilder *>(builder->field_builder(1));
  Int16Builder *year_builder =
      static_cast<Int16Builder *>(builder->field_builder(2));

  // append data to the child array builders
  {
    auto status = archer_builder->AppendValues(archers);
    if (!status.ok()) {
      std::cerr << status.message() << std::endl;
      return 1;
    }
  }
  {
    auto status = location_builder->AppendValues(locations);
    if (!status.ok()) {
      std::cerr << status.message() << std::endl;
      return 1;
    }
  }
  {
    auto status = year_builder->AppendValues(years);
    if (!status.ok()) {
      std::cerr << status.message() << std::endl;
      return 1;
    }
  }
  builder->AppendValues(5, nullptr);

  // call finish on the struct builder
  std::shared_ptr<arrow::Array> out;
  {
    auto status = builder->Finish(&out);
    if (!status.ok()) {
      std::cerr << status.message() << std::endl;
      return 1;
    }
  }
  std::cout << out->ToString() << std::endl;
}
