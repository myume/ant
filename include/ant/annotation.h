#pragma once

#include "serialize.h"
#include <filesystem>
#include <string>

class FileLocation {
private:
  std::filesystem::path filepath;
  int row;

public:
  FileLocation(const std::string &location_string);

  FileLocation(const std::filesystem::path &filepath, int row);

  std::filesystem::path getPath() const;

  int getRow() const;

  std::string toString() const;
};

class Annotation : public Serializable {
  std::string annotation;
  FileLocation location;
  std::string source;

public:
  FileLocation getLocation() const;

  Annotation(std::string annnotation, FileLocation location);
  Annotation(std::string annnotation, FileLocation location,
             std::string source);

  void serialize(std::ofstream &file) const override;

  static std::optional<Annotation>
  deserialize(std::ifstream &file, std::filesystem::path source_path);

  std::string display() const;
};
