#include <filesystem>
#include <string>

class AnnotatorMetadata {
private:
  std::string version;
  static constexpr std::string filename = ".ant";

public:
  AnnotatorMetadata();

  void serialize(const std::filesystem::path &ant_dir);

  static AnnotatorMetadata deserialize(const std::filesystem::path &ant_dir);
};

class Annotator {
private:
  std::string source_dir;
  std::string ant_dir;
  AnnotatorMetadata meta;

public:
  Annotator(std::string source_dir, std::string ant_dir = ".ant");

  static void init(const std::filesystem::path &ant_dir);

  void addAnnotation();

  void removeAnnotation();
};
