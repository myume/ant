#include <fstream>

class Serializable {
public:
  virtual void serialize(std::ofstream &file) = 0;
};
