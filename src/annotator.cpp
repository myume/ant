#include "ant/annotator.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_set>

AnnotatorMetadata::AnnotatorMetadata() : version(ANT_VERSION) {}

void AnnotatorMetadata::serialize(std::ofstream &file) {
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
    meta.version = line.substr(version_start + 1);
  } else {
    throw std::runtime_error("Missing version in .ant file");
  };

  return meta;
};

Annotator::Annotator(const std::string &source_dir, const std::string &ant_dir)
    : source_dir(source_dir), ant_dir(ant_dir) {
  if (!std::filesystem::exists(ant_dir)) {
    throw std::runtime_error(".ant dir not found, has ant been initialized?");
  }

  meta = AnnotatorMetadata::deserialize(ant_dir);
};

void Annotator::init(const std::filesystem::path &ant_dir) {
  if (std::filesystem::exists(ant_dir)) {
    std::println("ant has already been initialized in {}", ant_dir.string());
    return;
  }

  std::filesystem::create_directory(ant_dir);
  std::ofstream file(ant_dir / ".ant");

  AnnotatorMetadata().serialize(file);

  std::println("Successfully initialized ant in {}", ant_dir.string());
};

void Annotator::addAnnotation(const FileLocation &location, std::string data) {
  if (!std::filesystem::exists(location.getPath())) {
    throw std::runtime_error(
        "Could not add annotation - Path to source does not exist");
  }

  if (std::filesystem::is_directory(location.getPath()))
    throw std::runtime_error("Cannot add annotation for directory");

  std::filesystem::create_directories(ant_dir /
                                      location.getPath().parent_path());
  std::ofstream file(
      std::format("{}/{}.ant", ant_dir, location.getPath().string()),
      std::ios::app);

  Annotation annotation(data, location);

  annotation.serialize(file);
};

void Annotator::removeAnnotation(const FileLocation &location) {
  auto annotations = getAnnotations(location.getPath());

  std::string filepath =
      std::format("{}/{}.ant", ant_dir, location.getPath().string());

  std::string tempfile = std::format("{}.tmp", filepath);
  std::ofstream out(tempfile, std::ios::app);

  std::unordered_set<int> rows;
  for (auto &annotation : std::ranges::views::reverse(annotations)) {

    // dedupe rows
    int row = annotation.getLocation().getRow();
    if (rows.contains(row) || row == location.getRow())
      continue;
    rows.insert(row);

    annotation.serialize(out);
  }

  std::filesystem::rename(tempfile, filepath);
};

std::vector<Annotation>
Annotator::getAnnotations(const std::filesystem::path &path) {
  std::string filepath = std::format("{}/{}.ant", ant_dir, path.string());
  if (!std::filesystem::exists(filepath)) {
    throw std::runtime_error(
        std::format("No annotations found for file {}", filepath));
  }

  std::vector<Annotation> annotations;

  std::ifstream input(filepath);
  std::string annotation_line, row_line;
  while (getline(input, annotation_line) && getline(input, row_line)) {
    annotation_line = annotation_line.substr(annotation_line.find(" ") + 1);
    auto separator = row_line.find(" ");
    if (row_line.substr(0, separator) != "ROW") {
      throw std::runtime_error(
          std::format("Invalid line in annotations file, found {}", row_line));
    }
    int row = stoi(row_line.substr(separator + 1));

    Annotation anno(annotation_line, FileLocation(filepath, row));
    annotations.push_back(anno);
  }

  return annotations;
};
