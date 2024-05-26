{ stdenv
, qtbase
, full
, cmake
, wrapQtAppsHook
, opencv2
}:
stdenv.mkDerivation {
  pname = "stirtrace";
  version = "4.0";

  # The QtQuick project we created with Qt Creator's project wizard is here
  src = ./QT_StirTrace_v4.0;

  buildInputs = [
    qtbase
    full
	opencv2
  ];

  nativeBuildInputs = [
    cmake
    wrapQtAppsHook
  ];

  patchPhase = ''
    substituteInPlace main.cpp \
	  --replace "/home/mhilde/src/QT_StirTrace/haarcascade_frontalface_default.xml" "$out/share/haarcascade_frontalface_default.xml"
  '';

  # If the CMakeLists.txt has an install step, this installPhase is not needed.
  # The Qt default project however does not have one.
  installPhase = ''
    install -D --mode +r $src/haarcascade_frontalface_default.xml $out/share/haarcascade_frontalface_default.xml
    install -D stirtrace $out/bin/stirtrace
  '';
}
