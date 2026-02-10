{
  mkShell,
  cmake,
  clang-tools,
  codespell,
}:
mkShell {
  packages = [
    cmake
    clang-tools
    codespell
  ];
}
