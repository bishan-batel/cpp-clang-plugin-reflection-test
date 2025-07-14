{
  description = "A Nix-flake-based C/C++ development environment";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { 
    self, nixpkgs, flake-utils, ...
    }: flake-utils.lib.eachDefaultSystem (system: let
      pkgs = nixpkgs.legacyPackages.${system};
      libPath = with pkgs; [ ]; 
    in {

      devShells = { 
        default = pkgs.mkShell.override {
          stdenv = pkgs.llvmPackages_20.stdenv;
        } {
            name = "na-engine";
            packages = with pkgs; [
              ninja
              sccache
              cmake
              extra-cmake-modules
              gdb
            ];

            buildInputs = with pkgs; [
              llvmPackages_20.lldb 
              llvmPackages_20.clang-tools
              llvmPackages_20.clang

              pkg-config
            ];

            nativeBuildInputs = with pkgs; [ 
              llvmPackages_20.clang
            ] ++ libPath;

            LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath libPath;
          };
      };
    });
}
