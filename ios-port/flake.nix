{
  description = "Reproducible iOS build environment for OpenToonz";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs }: let
    system = "aarch64-darwin";
    pkgs = import nixpkgs { inherit system; };
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
        qt6.wrapQtAppsHook
      ];

      buildInputs = with pkgs; [
        qt6.qtbase
        qt6.qtsvg
        qt6.qtmultimedia
        boost
        libpng
        libjpeg
        libtiff
        zlib
      ];

      cmakeFlags = [
        "-G=Ninja"
        "-DCMAKE_SYSTEM_NAME=iOS"
        "-DCMAKE_OSX_SYSROOT=iphoneos"
        "-DCMAKE_OSX_ARCHITECTURES=arm64"
        "-DCMAKE_SYSTEM_PROCESSOR=arm64"
        "-DCMAKE_BUILD_TYPE=Release"
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
