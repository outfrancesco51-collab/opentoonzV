{
  description = "Reproducible iOS build environment for OpenToonz";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs }: let
    system = "aarch64-darwin";
    pkgs = import nixpkgs { inherit system; };

    # Merge all Qt5 dev outputs so CMake can find all components in one prefix
    qt5Env = pkgs.symlinkJoin {
      name = "qt5-env";
      paths = with pkgs.qt5; [ qtbase.dev qtsvg.dev qtmultimedia.dev qtscript.dev qttools.dev ];
    };
  in {
    packages.${system}.default = pkgs.stdenv.mkDerivation {
      pname = "opentoonz-ios";
      version = "latest";
      
      # The source is the root of the repository
      src = ../.;

      nativeBuildInputs = with pkgs; [
        cmake
        ninja
        pkg-config
        qt5.wrapQtAppsHook
      ];

      buildInputs = with pkgs; [
        qt5.qtbase
        qt5.qtsvg
        qt5.qtmultimedia
        qt5.qtscript
        qt5.qttools
        boost
        libpng
        libjpeg
        libtiff
        zlib
      ];

      cmakeFlags = [
        "-DNIX_BUILD=1"
        "-DCMAKE_BUILD_TYPE=Release"
        "-DCMAKE_INSTALL_PREFIX=$out"
        "-DCMAKE_SYSTEM_NAME=iOS"
        "-DCMAKE_OSX_ARCHITECTURES=arm64"
        "-DCMAKE_SYSTEM_PROCESSOR=aarch64"
        "-DCMAKE_PREFIX_PATH=${qt5Env}"
        "-DQt5_DIR=${qt5Env}/lib/cmake/Qt5"
        "-DQt5Core_DIR=${pkgs.qt5.qtbase.dev}/lib/cmake/Qt5Core"
        "-DQt5Gui_DIR=${pkgs.qt5.qtbase.dev}/lib/cmake/Qt5Gui"
        "-DQt5Widgets_DIR=${pkgs.qt5.qtbase.dev}/lib/cmake/Qt5Widgets"
        "-DQt5Network_DIR=${pkgs.qt5.qtbase.dev}/lib/cmake/Qt5Network"
        "-DQt5OpenGL_DIR=${pkgs.qt5.qtbase.dev}/lib/cmake/Qt5OpenGL"
        "-DQt5Xml_DIR=${pkgs.qt5.qtbase.dev}/lib/cmake/Qt5Xml"
        "-DQt5Multimedia_DIR=${pkgs.qt5.qtmultimedia.dev}/lib/cmake/Qt5Multimedia"
        "-DQt5Svg_DIR=${pkgs.qt5.qtsvg.dev}/lib/cmake/Qt5Svg"
        "-DQt5Script_DIR=${pkgs.qt5.qtscript.dev}/lib/cmake/Qt5Script"
        # OpenToonz-specific flags for mobile/iOS
        "-DWITH_WINTAB=OFF"
        "-DWITH_MIDI=OFF"
      ];

      cmakeDir = "../toonz/sources";

      installPhase = ''
        mkdir -p $out
        cmake --install . --prefix $out
      '';
    };

    devShells.${system}.default = pkgs.mkShell {
      inputsFrom = [ self.packages.${system}.default ];
      packages = with pkgs; [
        cmake
        ninja
      ];
      shellHook = ''
        export OPENTOONZ_IOS_REPO_ROOT="$PWD/.."
        export OPENTOONZ_IOS_BUILD_ROOT="$PWD/build-ios"
        echo "OpenToonz iPadOS development shell"
      '';
    };
  };
}
