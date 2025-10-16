{ stdenv
, qtbase
, full
, cmake
, wrapQtAppsHook
, opencv2
, pkg-config
}:
stdenv.mkDerivation {
  pname = "stirtrace-static-nogui";
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
    pkg-config
  ];

  # Enable static linking
  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_FIND_LIBRARY_SUFFIXES=.a"
    "-DCMAKE_EXE_LINKER_FLAGS=-static"
  ];

  patchPhase = ''
    substituteInPlace main.cpp \
      --replace "/home/mhilde/src/QT_StirTrace/haarcascade_frontalface_default.xml" "$out/share/haarcascade_frontalface_default.xml" \
      --replace "bool useGUI = true;" "bool useGUI = false;"
  '';

  # If the CMakeLists.txt has an install step, this installPhase is not needed.
  # The Qt default project however does not have one.
  installPhase = ''
    install -D --mode +r $src/haarcascade_frontalface_default.xml $out/share/haarcascade_frontalface_default.xml
    install -D stirtrace $out/bin/stirtrace

    # Strip debug symbols to reduce size
    strip $out/bin/stirtrace
  '';
}