#include "ant/annotator.h"

#include <filesystem>
#include <fstream>
#include <print>
#include <stdexcept>
#include <string>

AnnotatorMetadata::AnnotatorMetadata() : version(ANT_VERSION) {}

void AnnotatorMetadata::serialize(const std::filesystem::path &ant_dir) {
  std::ofstream file(ant_dir / filename);
  std::println(file, "VERSION {}", version);
}

AnnotatorMetadata
AnnotatorMetadata::deserialize(const std::filesystem::path &ant_dir) {
  std::ifstream file(ant_dir / filename);
  std::string line;
  std::getline(file, line);

  AnnotatorMetadata meta;

  auto version_start = line.find(" ");
  if (line.starts_with("VERSION") && version_start != std::string::npos) {
    meta.version = line.substr(version_start);
  } else {
    throw std::runtime_error("Missing version in .ant file");
  };

  return meta;
};

Annotator::Annotator(std::string source_dir, std::string ant_dir)
    : source_dir(source_dir), ant_dir(ant_dir) {};

void Annotator::init(const std::filesystem::path &ant_dir) {
  if (std::filesystem::exists(ant_dir)) {
    std::println("ant has already been initialized in {}", ant_dir.string());
    return;
  }

  std::filesystem::create_directory(ant_dir);
  AnnotatorMetadata().serialize(ant_dir);

  std::println("Successfully initialized ant in {}", ant_dir.string());
};

void Annotator::addAnnotation() {};

void Annotator::removeAnnotation() {};
