{
  mkShellNoCC,
  nodejs,
  vscodium,
}:
mkShellNoCC {
  packages = [
    nodejs
    vscodium
  ];
}
