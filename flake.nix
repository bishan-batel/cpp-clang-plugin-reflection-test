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
      llvm = pkgs.llvmPackages_20;
    in {

      devShells = { 
        default = pkgs.mkShell.override {
          stdenv = llvm.stdenv;
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
              llvm.lldb 
              llvm.clang-tools
              llvm.clang
              llvm.clang.cc
              llvm.llvm
              llvm.libclang

              pkg-config
            ];

            nativeBuildInputs = with pkgs; [ 
              llvm.clang
            ] ++ libPath;

            LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath libPath;
          };
      };
    });
}
