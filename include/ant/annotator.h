#pragma once

#include "ant/annotation.h"
#include <filesystem>
#include <fstream>
#include <string>

class AnnotatorMetadata : public Serializable {
private:
  std::string version;
  static constexpr std::string filename = ".ant";

public:
  AnnotatorMetadata();

  void serialize(std::ofstream &file) override;

  static AnnotatorMetadata deserialize(const std::filesystem::path &ant_dir);
};

class Annotator {
private:
  std::string source_dir;
  std::string ant_dir;
  AnnotatorMetadata meta;

public:
  Annotator(const std::string &source_path,
            const std::string &ant_dir = ".ant");

  static void init(const std::filesystem::path &ant_dir);

  void addAnnotation(FileLocation &location, std::string data);

  void removeAnnotation();
};
