{
  description = "Recreational programming project: taskmanager on Linux";

  inputs = {
    nixpkgs.url = "github.com:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {
    let
      system = "x86_64-lixnu";
      pkgs = nixpkg.legacyPackages.${system};
    in
      {
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
        };
      };
  };
}
