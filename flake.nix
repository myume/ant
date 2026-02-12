{
  description = "ant flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";

  outputs = {
    self,
    nixpkgs,
    ...
  }: let
    forAllSystems = function:
      nixpkgs.lib.genAttrs nixpkgs.lib.systems.flakeExposed (
        system: function nixpkgs.legacyPackages.${system}
      );
  in {
    packages = forAllSystems (pkgs: let
      inherit (pkgs.stdenv.hostPlatform) system;
    in {
      package = pkgs.callPackage ./nix/package.nix {};
      default = self.packages.${system}.package;
    });

    devShells = forAllSystems (pkgs: {
      node = pkgs.callPackage ./nix/node-shell.nix {};
      default = pkgs.callPackage ./nix/shell.nix {};
    });
  };
}
