#include <fstream>

class Serializable {
public:
  virtual void serialize(std::ofstream &file) const = 0;
};
