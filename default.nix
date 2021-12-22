# we pin nixpkgs sources here according to https://nix.dev/tutorials/towards-reproducibility-pinning-nixpkgs
{ pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/3c52ea8c9216a0d5b7a7b4d74a9d2e858b06df5c.tar.gz") {}
}:
pkgs.mkShell {
  # find packages here: https://search.nixos.org/packages
  # add libraries here
  buildInputs = [
    pkgs.libndctl
  ];
  # add tools here
  nativeBuildInputs = [
    pkgs.gdb
    pkgs.pkg-config
    pkgs.pandoc
    pkgs.m4
    pkgs.bashInteractive
    pkgs.cmake
  ];
  shellHook = ''
    export PATH=$PATH:$PWD/llvm-project/build/bin
  '';

  ## Disable mapping address randomization and provide PMDK with appropriately low mapping address hint:
  #PMEM_MMAP_HINT = 0;

  ## If your machine is not equipped with PM, export the following flag to use flush instructions instead of msync:
  #PMEM_IS_PMEM_FORCE = 1;
}
