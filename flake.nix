{
  description = "A basic flake with a shell";
  inputs = {
    nixpkgs = {
      url = "github:NixOS/nixpkgs/nixos-25.05";
    };
    systems = {
      url = "github:nix-systems/default";
    };
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };
  };

  outputs =
    { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in
      {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            python313
            bear
            gcc15
            libcxx.dev
          ];

          shellHook = ''
            source venv/bin/activate
            export LIBCXXABI_A_PATH="${pkgs.libcxx}/lib/libc++abi.a"
          '';
        };
      }
    );
}
