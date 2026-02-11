#include <string>

class Annotator {
private:
  std::string source_dir;
  std::string ant_dir;

public:
  Annotator(std::string source_dir, std::string ant_dir = ".ant");

  void init();

  void addAnnotation();

  void removeAnnotation();
};
