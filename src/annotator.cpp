#include "ant/annotator.h"
#include "ant/annotation.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

AnnotatorMetadata::AnnotatorMetadata() : version(ANT_VERSION) {}

void AnnotatorMetadata::serialize(std::ofstream &file) const {
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
    : source_dir(source_dir), ant_dir(ant_dir + "/.ant") {
  if (!std::filesystem::exists(this->ant_dir)) {
    throw std::runtime_error(".ant dir not found, has ant been initialized?");
  }

  meta = AnnotatorMetadata::deserialize(this->ant_dir);
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
  const FileLocation source_location =
      FileLocation(source_dir / location.getPath(), location.getRow());
  if (!std::filesystem::exists(source_location.getPath())) {
    throw std::runtime_error(std::format(
        "Could not add annotation - Path to source does not exist {}",
        source_location.getPath().string()));
  }

  if (std::filesystem::is_directory(source_location.getPath()))
    throw std::runtime_error("Cannot add annotation for directory");

  std::filesystem::create_directories(ant_dir /
                                      source_location.getPath().parent_path());
  std::ofstream file(std::format("{}/{}.ant", ant_dir.string(),
                                 source_location.getPath().string()),
                     std::ios::app);

  Annotation annotation(data, source_location);

  annotation.serialize(file);
};

void Annotator::removeAnnotation(const FileLocation &location) {
  auto annotations = getAnnotations(location.getPath());
  const FileLocation source_location =
      FileLocation(source_dir / location.getPath(), location.getRow());

  std::string filepath = std::format("{}/{}.ant", ant_dir.string(),
                                     source_location.getPath().string());

  std::vector<Annotation> filtered;
  for (const auto &annotation : annotations) {
    if (annotation.getLocation().getRow() != location.getRow()) {
      filtered.push_back(annotation);
    }
  };

  if (filtered.size() != annotations.size())
    writeAnnotations(filtered, filepath);
};

std::vector<Annotation>
Annotator::getAnnotations(const std::filesystem::path &path) {
  std::string relative_path = path.string();
  if (path.string().starts_with(ant_dir.parent_path().string())) {
    relative_path =
        path.string().substr(ant_dir.parent_path().string().size() + 1);
  }
  std::string filepath =
      std::format("{}/{}.ant", ant_dir.string(), relative_path);
  if (!std::filesystem::exists(filepath)) {
    throw std::runtime_error(std::format("No annotations found for file"));
  }

  std::vector<Annotation> raw_annotations;
  std::ifstream input(filepath);
  while (auto anno = Annotation::deserialize(input, source_dir / path)) {
    raw_annotations.push_back(anno.value());
  }

  std::vector<Annotation> annotations;
  std::unordered_set<int> rows;
  for (auto anno : std::ranges::views::reverse(raw_annotations)) {
    if (!rows.contains(anno.getLocation().getRow())) {
      rows.insert(anno.getLocation().getRow());
      annotations.push_back(anno);
    }
  }

  std::stable_sort(annotations.begin(), annotations.end(),
                   [](const Annotation &a, const Annotation &b) {
                     return a.getLocation().getRow() < b.getLocation().getRow();
                   });

  if (annotations.size() != raw_annotations.size())
    writeAnnotations(annotations, filepath);

  return annotations;
};

void Annotator::writeAnnotations(const std::vector<Annotation> &annotations,
                                 const std::filesystem::path &filepath) {
  std::string tempfile = std::format("{}.tmp", filepath.string());
  std::ofstream out(tempfile, std::ios::app);
  for (auto &annotation : annotations) {
    annotation.serialize(out);
  }

  std::filesystem::rename(tempfile, filepath);
};
