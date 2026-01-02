{
  description = "Recreational programming project: taskmanager on Linux";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
      {
        packages.${system}.default = pkgs.stdenv.mkDerivation {
          pname = "tum";
          version = "0.1.0";

          src = ./src;

          buildInputs = [ pkgs.ncurses ];

          buildPhase = ''
            gcc main.c -o tum
          '';

          installPhase = ''
            mkdir -p $out/bin
            cp tum $out/bin/'';
        };

        devShells.${system}.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            clang-tools
            ncurses
            ncurses.dev
            man-pages
            gdb
          ];

          shellHook = ''
            echo "C dev environment loaded!"
          '';

          buildPhase = ''
            gcc main.c -o tum && ./tum
          '';
        };
      };
}
