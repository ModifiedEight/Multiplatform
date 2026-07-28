#!/bin/sh
# Linux cross-compile launcher for the MCPE 0.8.1 iOS port.
# Mirrors the NBCraft approach: iOS 8.0 SDK + cctools-port ld64 + ldid.
set -e

scriptroot="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
platformdir="$scriptroot"

# Default build target(s). Override with NBC_TARGETS to build a custom set.
# Fat binary: armv7 floor lowered to iOS 5.0; arm64 floor is iOS 7.0 (the
# minimum the iOS 8.0 SDK supports for 64-bit). lipo fuses the slices below,
# so the .ipa runs on old 32-bit armv7 devices and modern arm64 ones (e.g.
# iPhone 6s Plus / iOS 15.5) from a single binary (~a couple MB larger).
NBC_TARGETS="${NBC_TARGETS:-armv7-apple-ios5.0 arm64-apple-ios7.0}"

# Which client to build: "classic" (minecraftpe) or "newadditions"
# (minecraftpe-newadditions). Select via the NBC_CLIENT env var or a CLI arg:
#   ./build.sh                 # classic ModifiedEight
#   ./build.sh newadditions    # ModifiedEight New Additions
#   NBC_CLIENT=newadditions ./build.sh
NBC_CLIENT="${NBC_CLIENT:-classic}"
case "$1" in
    newadditions|--newadditions|na) NBC_CLIENT=newadditions ;;
    classic|--classic|"") : ;;
esac

if [ "$NBC_CLIENT" = "newadditions" ]; then
    bin='modifiedeight-newadditions'
    ipaname='ModifiedEight-NewAdditions.ipa'
else
    NBC_CLIENT=classic
    bin='modifiedeight'
    ipaname='ModifiedEight.ipa'
fi
export NBC_CLIENT
entitlements="$platformdir/mcpe.entitlements"

workdir="$platformdir/build/work"
sdk="$workdir/sdks/ios-sdk"
export NBC_SDK="$sdk"
mkdir -p "$workdir/sdks"
cd "$workdir"

# ---- iOS SDK ----------------------------------------------------------------
sdkver=2
if ! [ -d "$sdk" ] || [ "$(cat sdks/sdkver 2>/dev/null)" != "$sdkver" ]; then
    printf '\nDownloading iOS 8.0 SDK...\n\n'
    rm -rf "$sdk" iPhoneOS8.0.sdk.tar.lzma iPhoneOS8.0.sdk
    wget https://invoxiplaygames.uk/sdks/iPhoneOS8.0.sdk.tar.lzma
    tar -x --lzma -f iPhoneOS8.0.sdk.tar.lzma
    mv iPhoneOS8.0.sdk "$sdk"
    rm -f iPhoneOS8.0.sdk.tar.lzma
    printf '%s' "$sdkver" > sdks/sdkver
fi

# Make sure clang can determine the SDK version. This SDK ships an old
# binary-format SDKSettings.plist (bplist00) that modern clang does NOT parse,
# so it silently stamps the Mach-O sdk version = deployment-target min (e.g.
# 7.0). UIKit then takes the legacy _UIOldConstraintBasedLayoutSupport path and
# aborts on keyboard rotation ("Autolayout doesn't support crossing rotational
# bounds transforms"). clang DOES read SDKSettings.json, so drop one in with the
# real SDK version to force a correct sdk stamp (>= 8.0 -> modern layout path).
if [ ! -f "$sdk/SDKSettings.json" ]; then
    cat > "$sdk/SDKSettings.json" <<'JSON'
{"Version":"8.0","CanonicalName":"iphoneos8.0","MaximumDeploymentTarget":"8.0","DisplayName":"iOS 8.0"}
JSON
fi

# ---- host tooling -----------------------------------------------------------
if command -v nproc >/dev/null; then ncpus="$(nproc)"; else ncpus=2; fi

# ---- compiler selection -----------------------------------------------------
# This decomp relies on undefined-behaviour that clang 14 tolerates but newer
# clang (17/18+) miscompiles into a runtime trap (udf #0xfe -> 0xdefe) when you
# go online. So PREFER clang-14 if it is installed. Override with CC/CXX env.
#   Ubuntu/Debian:  sudo apt install clang-14 llvm-14-dev libc++-14-dev libc++abi-14-dev
pick() { for c in "$@"; do command -v "$c" >/dev/null 2>&1 && { command -v "$c"; return 0; }; done; return 1; }

clangbin="${CC:-$(pick clang-14 clang)}"
clangxxbin="${CXX:-$(pick clang++-14 clang++)}"
ar="${AR:-$(pick llvm-ar-14 llvm-ar)}"
ranlib="${RANLIB:-$(pick llvm-ranlib-14 llvm-ranlib)}"

if [ -z "$clangbin" ] || [ -z "$clangxxbin" ]; then
    printf 'clang not found! Install clang-14 (recommended):\n'
    printf '  sudo apt install -y clang-14 llvm-14-dev libc++-14-dev libc++abi-14-dev\n'
    exit 1
fi

# Warn loudly if we ended up on a non-14 clang (works, but online may crash).
clangver="$("$clangbin" -dumpversion 2>/dev/null | cut -d. -f1)"
case "$clangver" in
    14) printf 'Using clang %s (%s)\n' "$clangver" "$clangbin" ;;
    *)  printf '\n*** WARNING: using clang %s (%s), NOT clang-14. ***\n' "$clangver" "$clangbin"
        printf '*** Online/multiplayer may crash (0xdefe). Install clang-14:\n'
        printf '***   sudo apt install -y clang-14 llvm-14-dev libc++-14-dev libc++abi-14-dev\n\n' ;;
esac

for dep in "$ar" "$ranlib" "$clangbin" "$clangxxbin" cmake cmp wget tar; do
    command -v "$dep" >/dev/null || { printf '%s not found!\n' "$dep"; exit 1; }
done

if [ -z "$LLVM_CONFIG" ]; then
    llvmcfg="$(pick llvm-config-14 llvm-config || true)"
    if [ -n "$llvmcfg" ]; then export LLVM_CONFIG="$llvmcfg"
    else export LLVM_CONFIG=false; fi
fi

# ---- cross toolchain (cctools-port ld64 + lipo/strip + ldid) ----------------
toolchainver=1
[ "$(cat toolchain/toolchainver 2>/dev/null)" != "$toolchainver" ] && rm -rf toolchain

mkdir -p toolchain/bin
export PATH="$workdir/toolchain/bin:$PATH"

ccache="$(command -v ccache || true)"
printf '#!/bin/sh\nexec %s %s "$@"\n' "$ccache" "$clangbin" > toolchain/bin/mcpe-clang
printf '#!/bin/sh\nexec %s %s "$@"\n' "$ccache" "$clangxxbin" > toolchain/bin/mcpe-clang++
chmod +x toolchain/bin/mcpe-clang toolchain/bin/mcpe-clang++

if [ -d "$ROOT/prebuilt/ios-toolchain/bin" ]; then
    cp -rn "$ROOT/prebuilt/ios-toolchain/bin/"* "$workdir/toolchain/bin/" 2>/dev/null || true
fi

if [ ! -x toolchain/bin/ld64.ld64 ] || [ ! -x toolchain/bin/lipo ] || [ ! -x toolchain/bin/cctools-strip ] || [ ! -x toolchain/bin/install_name_tool ]; then
    printf '\nBuilding cctools-port (ld64)...\n\n'
    cctools_commit=fee8115127bb849d7481ea0015f181d3ebbd33cf
    rm -rf cctools-port-*
    wget -O- "https://github.com/Un1q32/cctools-port/archive/$cctools_commit.tar.gz" | tar -xz
    cd "cctools-port-$cctools_commit/cctools"
    ./configure --enable-silent-rules --with-llvm-config="$LLVM_CONFIG" CC=mcpe-clang CXX=mcpe-clang++
    make -C ld64 -j"$ncpus"
    strip ld64/src/ld/ld
    mv ld64/src/ld/ld "$workdir/toolchain/bin/ld64.ld64"
    make -C libmacho -j"$ncpus"
    make -C libstuff -j"$ncpus"
    make -C misc strip lipo install_name_tool -j"$ncpus"
    strip misc/strip misc/lipo misc/install_name_tool
    mv misc/strip "$workdir/toolchain/bin/cctools-strip"
    mv misc/lipo "$workdir/toolchain/bin/lipo"
    mv misc/install_name_tool "$workdir/toolchain/bin/install_name_tool"
    cd "$workdir"
    rm -rf "cctools-port-$cctools_commit"
    printf '%s' "$toolchainver" > toolchain/toolchainver
fi

instnametool="$(pick llvm-install-name-tool-14 llvm-install-name-tool install_name_tool || echo "$workdir/toolchain/bin/install_name_tool")"
if [ -z "$instnametool" ] || [ ! -x "$instnametool" ]; then
    instnametool="$workdir/toolchain/bin/install_name_tool"
fi
if [ -x "$instnametool" ] && [ ! -x "$workdir/toolchain/bin/install_name_tool" ]; then
    ln -sf "$instnametool" "$workdir/toolchain/bin/install_name_tool"
fi

if [ ! -x toolchain/bin/ldid ]; then
    if command -v ldid >/dev/null; then
        ln -sf "$(command -v ldid)" toolchain/bin/ldid
    else
        printf '\nBuilding ldid...\n\n'
        ldid_commit=ef330422ef001ef2aa5792f4c6970d69f3c1f478
        rm -rf ldid-*
        wget -O- "https://github.com/ProcursusTeam/ldid/archive/$ldid_commit.tar.gz" | tar -xz
        cd "ldid-$ldid_commit"
        make CXX=mcpe-clang++
        strip ldid
        mv ldid "$workdir/toolchain/bin"
        cd "$workdir"
        rm -rf "ldid-$ldid_commit"
    fi
fi

# ---- cmake build per target -------------------------------------------------
if [ -z "$DEBUG" ]; then build=Release; else build=Debug; fi

built=
for target in $NBC_TARGETS; do
    printf '\nBuilding for %s\n\n' "$target"
    export NBC_TARGET="$target"
    mkdir -p "build-$NBC_CLIENT-$target"
    cd "build-$NBC_CLIENT-$target"
    cmake "$platformdir/../.." \
        -DCMAKE_BUILD_TYPE="$build" \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DNBC_PLATFORM=ios \
        -DMCPE_IOS=ON \
        -DNBC_CLIENT="$NBC_CLIENT" \
        -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
        -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
        -DCMAKE_AR="$(command -v "$ar")" \
        -DCMAKE_RANLIB="$(command -v "$ranlib")" \
        -DCMAKE_INSTALL_NAME_TOOL="$instnametool" \
        -DCMAKE_C_COMPILER="$platformdir/ios-cc" \
        -DCMAKE_CXX_COMPILER="$platformdir/ios-c++" \
        -DCMAKE_FIND_ROOT_PATH="$NBC_SDK/usr" \
        -DCMAKE_C_FLAGS="${CFLAGS:-}" \
        -DCMAKE_CXX_FLAGS="${CXXFLAGS:-}" \
        -DJSONCPP_WITH_TESTS=OFF \
        -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF \
        -DWERROR="${WERROR:-OFF}"
    cmake --build . --parallel "$ncpus"
    found="$(find . -maxdepth 3 -type f -name "$bin" | head -1)"
    [ -n "$found" ] && built="$built $PWD/$found"
    cd "$workdir"
done

# ---- lipo + sign ------------------------------------------------------------
if [ -n "$built" ]; then
    # shellcheck disable=2086
    lipo -create $built -output "$platformdir/$bin"
    [ -z "$DEBUG" ] && [ -z "$NOSTRIP" ] && cctools-strip -no_code_signature_warning "$platformdir/$bin" || true
    if command -v ldid >/dev/null; then
        ldid -S"$entitlements" "$platformdir/$bin"
    fi
    printf '\nBuilt binary: %s\n' "$platformdir/$bin"
    [ -n "$NBC_NO_IPA" ] || IPA_NAME="$ipaname" "$platformdir/build-ipa.sh" "$platformdir/$bin"
else
    printf '\nNo binary produced (build failed).\n'
    exit 1
fi
