#include "ant/annotator.h"
#include <string>

Annotator::Annotator(std::string source_dir, std::string ant_dir)
    : source_dir(source_dir), ant_dir(ant_dir) {};

void Annotator::init() {};

void Annotator::addAnnotation() {};

void Annotator::removeAnnotation() {};
