#include "ant/annotation.h"
#include "ant/annotator.h"
#include <chrono>
#include <filesystem>
#include <iostream>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: ant-benchmark <sample_size> \n";
    return -1;
  }

  int n = atoi(argv[1]);

  std::filesystem::path bench_dir = ".ant_bench";
  if (std::filesystem::remove_all(bench_dir)) {
    std::cout << "clearing previous benchmark run...\n";
  }
  std::filesystem::create_directory(bench_dir);

  Annotator::init(bench_dir / ".ant");

  Annotator a(bench_dir, bench_dir);

  std::cout << "Running benchmarks...\n";

  auto write_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < n; i++) {
    FileLocation loc("test", i);
    a.addAnnotation(loc, "test");
  }
  auto write_stop = std::chrono::high_resolution_clock::now();
  long double write_throughput =
      n / (std::chrono::duration_cast<std::chrono::milliseconds>(write_stop -
                                                                 write_start)
               .count() /
           1000.0);

  std::cout << write_throughput << " writes/s " << 1000.0 / write_throughput
            << " ms/write\n";

  auto read_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < n; i++) {
    a.getAnnotations("test");
  }
  auto read_stop = std::chrono::high_resolution_clock::now();
  long double read_throughput =
      n / (std::chrono::duration_cast<std::chrono::milliseconds>(read_stop -
                                                                 read_start)
               .count() /
           1000.0);

  std::cout << read_throughput << " reads/s " << 1000.0 / read_throughput
            << " ms/read\n";

  std::filesystem::remove_all(bench_dir);
}
