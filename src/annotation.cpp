#include "ant/annotation.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>

std::string escape_json_string(std::string s) {
  std::string escaped = "";
  for (auto c : s) {
    switch (c) {
      case '"':
        escaped += "\\";
        break;
    }
    escaped += c;
  }

  return escaped;
};

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

std::filesystem::path FileLocation::getPath() const { return filepath; };

int FileLocation::getRow() const { return row; };

std::string FileLocation::toString() const {
  return std::format("{}:{}", filepath.string(), row);
};

Annotation::Annotation(std::string annotation, FileLocation location)
    : annotation(annotation), location(location) {
#ifndef BENCHMARK_MODE
  std::ifstream file(location.getPath());
  for (int i = 0; i < location.getRow(); i++) {
    if (!getline(file, source)) {
      throw std::runtime_error(std::format("Line {} does not exist in {}",
                                           location.getRow(),
                                           location.getPath().string()));
    };
  }
#endif
};

Annotation::Annotation(std::string annotation, FileLocation location,
                       std::string hash)
    : annotation(annotation), location(location), source(hash) {};

void Annotation::serialize(std::ofstream &file) const {
  std::println(file, "ANNOTATION {}", annotation);
  std::println(file, "HASH {}", source);
  std::println(file, "ROW {}", location.getRow());
}

std::optional<Annotation>
Annotation::deserialize(std::ifstream &file,
                        std::filesystem::path source_path) {
  std::string annotation_line, hash_line, row_line;
  if (!getline(file, annotation_line) || !getline(file, hash_line) ||
      !getline(file, row_line)

  )
    return std::nullopt;

  annotation_line = annotation_line.substr(annotation_line.find(" ") + 1);
  hash_line = hash_line.substr(hash_line.find(" ") + 1);

  auto separator = row_line.find(" ");
  if (row_line.substr(0, separator) != "ROW") {
    throw std::runtime_error(
        std::format("Invalid line in annotations file, found {}", row_line));
  }

  int row = stoi(row_line.substr(separator + 1));
  return Annotation(annotation_line, FileLocation(source_path, row), hash_line);
};

std::string Annotation::display() const {
  std::string res = location.toString();
  res += "\n";
  res += "source: " + source + "\n";
  res += "> " + annotation;

  return res;
};

std::string Annotation::json() const {
  std::string json = "{";

  json +=
      std::format("\"annotation\": \"{}\",", escape_json_string(annotation));
  json += std::format("\"source\": \"{}\",", escape_json_string(source));
  json += std::format("\"row\": {}", location.getRow());
  json += "}";
  return json;
}

FileLocation Annotation::getLocation() const { return location; };
