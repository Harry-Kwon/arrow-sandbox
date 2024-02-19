#include <arrow/api.h>
#include <arrow/array/array_nested.h>
#include <arrow/array/builder_nested.h>
#include <arrow/status.h>
#include <arrow/type_fwd.h>
#include <arrow/visit_type_inline.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
std::vector<std::int64_t> ids{1, 2, 3, 4, 5};
std::vector<std::double_t> costs{10, 20, 30, 40, 50};
std::vector<std::vector<std::double_t>> cost_components{
    {1, 2, 3, 4, 5}, {2, 3, 4, 5}, {3, 4, 5}, {4, 5}, {5},
};

void handleStatus(arrow::Status status) {
  if (!status.ok()) {
    std::cerr << status.message() << std::endl;
    status.Abort();
  }
}

int main(int argc, char *argv[]) {
  // child vectors
  // id int64 array
  std::shared_ptr<arrow::Int64Array> id_array;
  arrow::Int64Builder id_builder;
  handleStatus(id_builder.AppendValues(ids));
  handleStatus(id_builder.Finish(&id_array));

  // cost double array
  std::shared_ptr<arrow::DoubleArray> cost_array;
  arrow::DoubleBuilder cost_builder;
  handleStatus(cost_builder.AppendValues(costs));
  handleStatus(cost_builder.Finish(&cost_array));

  // cost_components list has to be built from 2 record batches
  // 1. Double array with values
  // 2. int array with offsets

  std::shared_ptr<arrow::DoubleArray> values_array;
  arrow::DoubleBuilder values_builder;
  std::shared_ptr<arrow::Int32Array> offsets_array;
  arrow::Int32Builder offsets_builder;

  int offset = 0;
  handleStatus(offsets_builder.Append(offset));
  for (auto components : cost_components) {
    offset += components.size();
    handleStatus(values_builder.AppendValues(components));
    handleStatus(offsets_builder.Append(offset));
  }
  handleStatus(values_builder.Finish(&values_array));
  handleStatus(offsets_builder.Finish(&offsets_array));

  auto cost_components_array =
      arrow::ListArray::FromArrays(*offsets_array, *values_array);

  // compose the final structarrow from the 3 child arrays
  auto children =
      arrow::ArrayVector{id_array, cost_array, *cost_components_array};

  auto row_t = arrow::struct_({
      arrow::field("id", arrow::int64()),
      arrow::field("cost", arrow::float64()),
      arrow::field("cost_components", arrow::list(arrow::float64())),
  });

  // struct arry
  arrow::StructArray arr{row_t, id_array->length(), children};

  std::cout << arr.ToString() << std::endl;
}
