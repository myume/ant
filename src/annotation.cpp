#include "ant/annotation.h"
#include <format>
#include <print>
#include <stdexcept>
#include <string>

FileLocation::FileLocation(const std::string &location_string) {
  auto split = location_string.find(":");
  if (split == std::string::npos) {
    throw std::runtime_error("Invalid location string, location string must be "
                             "in the formation of [filepath:row]");
  }

  filepath = location_string.substr(0, split);
  row = std::stoi(location_string.substr(split + 1));
};

FileLocation::FileLocation(const std::filesystem::path &filepath, int row)
    : filepath(filepath), row(row) {};

std::filesystem::path FileLocation::getPath() { return filepath; };

int FileLocation::getRow() { return row; };

std::string FileLocation::toString() {
  return std::format("{}:{}", filepath.string(), row);
};

Annotation::Annotation(std::string annotation, FileLocation location)
    : annotation(annotation), location(location) {};

void Annotation::serialize(std::ofstream &file) {
  std::println(file, "ANNOTATION {}", annotation);
  std::println(file, "ROW {}", location.getRow());
}
