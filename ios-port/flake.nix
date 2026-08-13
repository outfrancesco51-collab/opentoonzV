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
      paths = with pkgs.qt5; [ qtbase.dev qtsvg.dev qtmultimedia.dev qtscript.dev qttools.dev qtserialport.dev ];
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
        qt5Env
        qt5.qtbase
        qt5.qtsvg
        qt5.qtmultimedia
        qt5.qtscript
        qt5.qttools
        qt5.qtserialport
        boost
        libpng
        libjpeg
        libtiff
        zlib
        opencv4
        glew
        superlu
        lzo
      ];

      preConfigure = ''
        export CMAKE_PREFIX_PATH="${qt5Env}:$CMAKE_PREFIX_PATH"
      '';

      cmakeFlags = [
        "-DNIX_BUILD=1"
        "-DCMAKE_BUILD_TYPE=Release"
        "-DCMAKE_INSTALL_PREFIX=$out"
        "-DCMAKE_SYSTEM_NAME=iOS"
        "-DCMAKE_OSX_ARCHITECTURES=arm64"
        "-DCMAKE_SYSTEM_PROCESSOR=aarch64"
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
