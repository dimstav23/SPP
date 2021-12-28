  { stdenv, wrapCC, runtimeShell }:
  wrapCC (stdenv.mkDerivation {
    name = "impure-clang";
    dontUnpack = true;
    installPhase = ''
      mkdir -p $out/bin
      for bin in ${toString (builtins.attrNames (builtins.readDir ./../.local/spp.llvm.12.0.0/bin))}; do
        cat > $out/bin/$bin <<EOF
  #!${runtimeShell}
  exec "${toString ./.}/../.local/spp.llvm.12.0.0/bin/$bin" "\$@"
  EOF
        chmod +x $out/bin/$bin
      done
    '';
    passthru.isClang = true;
  })

