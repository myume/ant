#include <optional>
#include <print>
#include <string_view>

#include "ant/annotator.h"

enum Command { Init, Add, Remove };

static void help() {
  std::println("Usage: ant [options] [command]\n");
  std::println(R"#(Commands:
  init        - initialize the annotations directory
  add         - add an annotation
  rm          - remove an annotation)#");
  std::println();
  std::println(R"#(Options:
  --version   - the ant version
  -s          - the source code directory
  -o          - path to output/annotations directory)#");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    help();
    return -1;
  }

  std::optional<Command> command = std::nullopt;
  std::string source = ".", output = ".ant";

  for (int i = 1; i < argc; i++) {
    std::string_view value = argv[i];
    if (value == "init") {
      command = Command::Init;
    } else if (value == "add") {
      command = Command::Add;
    } else if (value == "rm") {
      command = Command::Remove;
    }

    if (value == "-s") {
      source = argv[i++];
    }

    if (value == "-o") {
      output = argv[i++];
    }

    if (value == "--version") {
      std::println("{}", ANT_VERSION);
      return 0;
    }
  }

  if (!command) {
    std::println("Invalid Command");
    help();
    return -1;
  }

  switch (command.value()) {
    case Init:
      Annotator::init(output);
      break;
    case Add: {
      Annotator ant(source, output);
      ant.addAnnotation();
      break;
    }
    case Remove: {
      Annotator ant(source, output);
      ant.removeAnnotation();
      break;
    }
  }
}
