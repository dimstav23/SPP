# we ping nixpkgs soureces here according to https://nix.dev/tutorials/towards-reproducibility-pinning-nixpkgs
{ pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/3c52ea8c9216a0d5b7a7b4d74a9d2e858b06df5c.tar.gz") {}
}:
pkgs.mkShell {
  # find packages here: https://search.nixos.org/packages
  # add libraries here
  buildInputs = [
    pkgs.libndctl
    pkgs.glib
  ];
  # add tools here
  nativeBuildInputs = [
    pkgs.gcc10
    pkgs.pkg-config
    pkgs.pandoc
    pkgs.m4
    pkgs.bc
    pkgs.bashInteractive
    pkgs.cmake
    pkgs.dos2unix
    pkgs.binutils-unwrapped
    pkgs.gdb
    pkgs.python
    pkgs.valgrind
    pkgs.autoreconfHook
    pkgs.numactl
    (pkgs.python39.withPackages (ps: [
     	ps.matplotlib
	ps.pyyaml
    ]))
    (pkgs.callPackage ./impure-clang.nix {})
  ];

  shellHook = ''
    export CMAKE_C_COMPILER=clang
    export CC=clang
    export CMAKE_CXX_COMPILER=clang++
    export CXX=clang++
    #export CMAKE_C_COMPILER=${pkgs.gcc}/bin/gcc
    #export CC=${pkgs.gcc}/bin/gcc
    #export CMAKE_CXX_COMPILER=${pkgs.gcc}/bin/g++
    #export CXX=${pkgs.gcc}/bin/g++
    export BINUTILS_DIR=${pkgs.binutils-unwrapped}/include
  '';

  ## Disable mapping address randomization and provide PMDK with appropriately low mapping address hint:
  PMEM_MMAP_HINT = 0;

  ## If your machine is not equipped with PM, export the following flag to use flush instructions instead of msync:
  PMEM_IS_PMEM_FORCE = 1;

}

