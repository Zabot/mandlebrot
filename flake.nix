{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in {
      packages.x86_64-linux = {
        default = pkgs.stdenv.mkDerivation {
          name = "mandlebrot";
          src = ./.;
        };
      };

      devShells.x86_64-linux.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          gdb
          ccls
          clang-tools
          qiv
          opencl-headers
          rocmPackages.clr
        ];
      };
    };
}
