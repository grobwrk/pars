{
  description = "clang flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }: let
    system = "x86_64-linux"; # Or your system like "aarch64-linux", "x86_64-darwin"
    pkgs = nixpkgs.legacyPackages.${system};
    pythonPackages = ps: [
      ps.pip
      ps.sphinx
      ps.breathe
    ];
    pythonEnv = pkgs.python3.withPackages pythonPackages;
  in {
    devShells.${system}.default = pkgs.mkShell {
       buildInputs = [
         pkgs.llvmPackages_19.clang
         pkgs.llvmPackages_19.lld
         pkgs.cmake
         pkgs.ninja
         pkgs.vcpkg
         pkgs.doxygen
         pkgs.graphviz
         pythonEnv
       ];
    };
  };
}
