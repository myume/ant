{
  stdenv,
  cmake,
  lib,
}:
stdenv.mkDerivation {
  pname = "ant";
  version = "0.1.0";
  src = ../.;

  buildInputs = [
    cmake
  ];

  meta = {
    description = "A code annotation tool";
    homepage = "https://github.com/myume/ant";
    license = lib.licenses.mit;
    maintainers = with lib.maintainers; [myu];
    platforms = lib.platforms.all;
  };
}
