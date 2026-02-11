#include <exception>
#include <optional>
#include <print>
#include <string_view>

#include "ant/annotation.h"
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

void add_help() {
  std::println("Usage: ant add [source_path:line_number] <annotation>\n");
}
void remove_help() {
  std::println("Usage: ant remove [source_path:line_number]\n");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    help();
    return -1;
  }

  std::optional<Command> command = std::nullopt;
  std::string source = ".", output = ".ant";
  std::string annotation;
  std::optional<FileLocation> location;

  for (int i = 1; i < argc; i++) {
    std::string_view value = argv[i];
    if (value == "init") {
      command = Command::Init;
    } else if (value == "add") {
      if (i < argc - 3) {
        add_help();
        return -1;
      }

      try {
        location = FileLocation(argv[++i]);
      } catch (std::exception &e) {
        std::println("{}", e.what());
        add_help();
        return -1;
      }
      annotation = argv[++i];
      command = Command::Add;
    } else if (value == "rm") {
      if (i < argc - 2) {
        remove_help();
        return -1;
      }
      try {
        location = FileLocation(argv[++i]);
      } catch (std::exception &e) {
        std::println("{}", e.what());
        remove_help();
        return -1;
      }
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

  try {
    switch (command.value()) {
      case Init:
        Annotator::init(output);
        break;
      case Add: {
        if (!location) {
          std::println("Missing location argument for add command");
          add_help();
          return -1;
        }
        Annotator ant(source, output);
        ant.addAnnotation(location.value(), annotation);
        std::println("Successfully added annotation to {}",
                     location.value().toString());
        break;
      }
      case Remove: {
        if (!location) {
          std::println("Missing location argument for remove command");
          remove_help();
          return -1;
        }
        Annotator ant(source, output);
        ant.removeAnnotation();
        std::println("Successfully removed annotation from {}",
                     location.value().toString());
        break;
      }
    }
  } catch (std::exception &e) {
    std::println("{}", e.what());
    return -1;
  }
}
