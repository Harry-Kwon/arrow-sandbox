#include <arrow/api.h>
#include <arrow/array.h>
#include <iostream>

int main(int argc, char **argv) {
  std::vector<int64_t> data{
      1,
      2,
      3,
      4,
  };
  auto arr = std::make_shared<arrow::Int64Array>(data.size(),
                                                 arrow::Buffer::Wrap(data));

  std::cout << arr->ToString() << std::endl;
}
