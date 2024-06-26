{
  inputs = {
	nixpkgs.url = "github:nixos/nixpkgs";
	nix-github-actions.url = "github:nix-community/nix-github-actions";
	nix-github-actions.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs = { self, nixpkgs, nix-github-actions }:
  let
    pkgs = nixpkgs.legacyPackages.x86_64-linux;

	gui_package = pkgs.qt6Packages.callPackage ./build.nix {};
	no_gui_package = pkgs.qt6Packages.callPackage ./build-nogui.nix {};
  in {

    packages.x86_64-linux = {
		gui = gui_package;
		terminal = no_gui_package;
		default = gui_package;
	};

	githubActions = nix-github-actions.lib.mkGithubMatrix { checks = self.packages; };

    devShells.x86_64-linux.default = pkgs.mkShell {
      inputsFrom = [ self.packages.x86_64-linux.default ];
      buildInputs = with pkgs; [
        gdb
        qtcreator

        # this is for the shellhook portion
        qt6.wrapQtAppsHook
        makeWrapper
        bashInteractive
      ];
      # set the environment variables that Qt apps expect
      shellHook = ''
        bashdir=$(mktemp -d)
        makeWrapper "$(type -p bash)" "$bashdir/bash" "''${qtWrapperArgs[@]}"
        # exec "$bashdir/bash"
      '';
    };
  };
}
