{
  description = "Description for the project";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    inputs@{ flake-parts, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
        "x86_64-darwin"
      ];
      perSystem =
        { pkgs, ... }:
        {
          devShells.default = pkgs.mkShell {
            packages = with pkgs; [
              root
              cmake
              pkg-config
            ];
            shellHook = ''
              printf 'CompileFlags:\n  Add: [-std=c++17, %s]\n' \
                "$(root-config --cflags | sed 's/ /, /g')" > .clangd
            '';
          };
          packages.scripts = pkgs.stdenv.mkDerivation {
            src = ./.;
            pname = "H4l-scripts";
            version = "0.1";
            buildInputs = [ pkgs.root ];
            nativeBuildInputs = with pkgs; [
              cmake
              pkg-config
            ];
          };
        };
    };
}
