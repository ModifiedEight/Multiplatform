# platforms/ios — porting notes

NBCraft-style Linux cross-compile path for an iOS build of MCPE 0.8.1.

## How the build is wired
- `build.sh` is run from this directory. It downloads the iOS 8.0 SDK, builds
  `cctools-port` (`ld64`, `lipo`, `strip`) and `ldid`, then runs CMake on the
  repo root (`platforms/ios/../..`) once per target in `NBC_TARGETS`.
- `ios-cc` / `ios-c++` are the compiler wrappers CMake calls. They require
  `NBC_TARGET` and `NBC_SDK` in the environment (build.sh sets both).
- The build passes `-DMCPE_IOS=ON`, which is the single switch the CMake and the
  C/C++ source guards key off of.
- **Client variant:** `build.sh` builds one of two clients, selected by the
  `NBC_CLIENT` env var or the first CLI arg:
    - `./build.sh` (or `./build.sh classic`) → `minecraftpe` → `ModifiedEight.ipa`.
    - `./build.sh newadditions` (or `NBC_CLIENT=newadditions ./build.sh`) →
      `minecraftpe-newadditions` → `ModifiedEight-NewAdditions.ipa`.
  It passes `-DNBC_CLIENT=<variant>` to CMake (the root `CMakeLists.txt` picks the
  matching subdirectory), namespaces the build dir per client, and stamps a
  distinct bundle id / display name for New Additions so both can coexist on one
  device. Every iOS patch (guards + the pause button) lives in BOTH trees, so
  either variant cross-compiles.

## iOS-only on-screen pause button (`impl/input/TouchscreenInput.cpp`)
- The Android 0.8.1 decomp has no pause button (pause was opened by the OS
  notification shade). Under `#ifdef MCPE_IOS` a pause button (touch id 105,
  gui.png UV 200,64) is added at the top-right corner via the existing
  `field_68`/`getPauseRectangleArea()` slot; tapping it calls
  `minecraft->pauseGame(0)`. Chat (106) shifts left of it, camera (107) further
  left. On iOS the whole top-right button cluster is halved
  (`Gui::GuiScale * 26.0` → `* 13.0`) to match the reference layout; non-iOS
  platforms keep the original size/layout exactly.

## What `-DMCPE_IOS=ON` changes (root `minecraftpe/CMakeLists.txt`)
- Skips `find_package(SDL/OpenGL/CURL)`; keeps `ZLIB`.
- Adds `-DUSEGLES -DMCPE_IOS` (GLES 1.x render path, same family as Android).
- Excludes `impl/main.cpp` and `impl/AppPlatform_sdl.cpp` from the core sources
  (the UIKit shell replaces them).
- Compiles `platforms/ios/*.mm` + `*.m` into the executable.
- Links `objc` + the iOS frameworks: Foundation, CoreGraphics, QuartzCore,
  UIKit, OpenGLES, OpenAL, AVFoundation.

## Source-level guards already added
- `headers/sound/SoundEngine.hpp` — Apple routes to `SoundSystemAL` (OpenAL).
- `sound/SoundSystemAL.{hpp,cpp}` — guard now matches Apple; `<OpenAL/*>`
  headers on Apple, `<AL/*>` elsewhere.
- `headers/unigl.h` — Apple uses `<OpenGLES/ES1/*>`; no EGL on iOS.
- `headers/AppContext.hpp`, `headers/_pengine.hpp` — EGL-typed fields only for
  non-Apple GLES; Apple uses `void*` (EAGL is managed by the shell).
- `headers/main.hpp` — no SDL include on iOS.
- Networking (`CurlRestRequestJob.*`, `RestRequestJob::CreateJob`) — Curl is
  excluded on iOS (no libcurl in the public SDK); `CreateJob` returns the
  stubbed job like Android. Re-implementing networking is a TODO.

## MFi gamepad support (`GamepadMFi.h/.mm`, shared by both trees)
- `main.mm` calls `GamepadMFi_poll()` every `tick:` before `g_app->update()`,
  feeding the shared `Gamepad` state (same canonical indices as the SDL/Android
  backends): A/B/X/Y → 0..3, shoulders → 4/5, L3/R3 → 8/9, sticks → axes 0..3
  (Y negated to match SDL sign), analog triggers → axes 4/5, D-pad → hat,
  menu button (via `controllerPausedHandler`) → momentary START (7).
- Bindings UI, `ControllerHandler` and `controller_layout.txt` work unchanged.
- Only iOS 7/8-era `GameController` APIs are used (extended profile only, no
  `buttonMenu`/`microGamepad`); `GameController.framework` is linked in both
  trees' `MCPE_IOS` CMake blocks. New `.mm` files are picked up automatically
  by the existing `file(GLOB IOS_PLATFORM_SRC ...)` in both trees.

## App shell — DONE (it links + produces an ipa)
The Objective-C++ shell now exists in this directory and matches THIS decomp's
API:
- `main.mm` — `UIApplicationMain` entry, app delegate, `UIViewController` with a
  `CADisplayLink` loop calling `NinecraftApp::update()`, touch fed to
  `Multitouch`/`Mouse`, soft-keyboard text view fed to `Keyboard`.
- `EAGLView.{h,mm}` — `CAEAGLLayer` + GLES1 framebuffer (color + depth).
- `AppPlatform_iOS.{hpp,mm}` — implements the `AppPlatform` pure-virtuals
  (`getImagePath`, `loadPNG`, `readAssetFile` bundle-relative; screen size;
  `supportsTouchscreen`; `getLoginInformation`; `showKeyboard`/`hideKeyboard`).
- `tools/gen_silent_pcm.py` writes a silent `pcm_data.c` so it links without the
  original APK sounds.

Build extras that were required (see CLAUDE.md): host `libc++-dev` headers,
`#include_next <_types.h>` on Apple, `-DSTBI_NO_THREAD_LOCALS`, and the OpenAL
typedef fix in `SoundSystemAL.hpp`.

## libc++ iostream link wall (SOLVED — don't regress)
Symptom: at 100% (linking) the build dies with `Undefined symbols for
architecture armv7` for `vtable for std::__1::basic_stringstream`, `VTT for
...`, `vtable for basic_stringbuf`, `basic_ostringstream`, and
`basic_stringbuf::str() const` — referenced from `Options.cpp`, `I18n.cpp`,
`ExternalServerFile.cpp`, `libjsoncpp.a(json_writer.cpp.o)`, etc.

Cause: the host libc++ headers (libc++-18) are a *non-vendor* build, so they
define `_LIBCPP_HAS_NO_VENDOR_AVAILABILITY_ANNOTATIONS`. That makes `<sstream>`
emit `extern template` declarations for the string-stream classes — i.e. it
expects their vtables / `str()` to come from the shared `libc++.dylib`. The iOS
8.0 SDK's `libc++.dylib` (2014) never exported those symbols, so the link fails.

Fix: `platforms/ios/libcxx/__config_site` is force-included ahead of the host
one (via `-I "$scriptroot/libcxx"` in `ios-cc`/`ios-c++`). It `#include_next`s
the real config and then `#undef`s `_LIBCPP_HAS_NO_VENDOR_AVAILABILITY_ANNOTATIONS`,
pushing `<__availability>` into the `__APPLE__` branch. There the extra iostream
instantiations are disabled for iOS < 15.0, so the stream vtables are emitted
locally (weak) in our objects and the link succeeds. Don't remove the
`-I .../libcxx` flag or that shim.

## Deployment target
Default `NBC_TARGETS="armv7-apple-ios5.0 arm64-apple-ios7.0"` (fat binary;
lipo fuses the slices). armv7 floor is iOS 5.0; arm64 floor is iOS 7.0, the
minimum the iOS 8.0 SDK supports for 64-bit. Evaluate lowering armv7 to 4.3
only after on-device testing with real assets.

## Configure/link fixes (don't regress)
Three issues blocked the iOS build; all fixed in `minecraftpe/CMakeLists.txt`,
`MarketplaceScreen.cpp`, and `CrossPlatformWeb.cpp`:

1. **`install_name_tool` configure error.** `minecraftpe/CMakeLists.txt` called
   `enable_language(OBJCXX)` under `MCPE_IOS`. On a Darwin target that triggers
   `CMakeDetermineOBJCXXCompiler` → `CMakeFindBinUtils`, which hard-requires
   `install_name_tool` (a macOS-only cctools binary we don't ship), aborting
   configure. We don't need it: `.mm`/`.m` are compiled by extension via the
   `ios-c++` wrapper without enabling the OBJCXX language. Removed the
   `enable_language(OBJCXX)` block (matches the working reference port).

2. **`curl/curl.h` not found.** The iOS 8.0 SDK has no libcurl. `MarketplaceScreen.cpp`
   `#include`d `<curl/curl.h>` but never calls `curl_*` directly (all network
   goes through the `CrossPlatform_*` wrappers), so the include was dropped.
   `CrossPlatformWeb.cpp` fell into its `#else // Linux/Mac` curl branch on iOS;
   added an `#elif defined(MCPE_IOS)` branch with pure-C++ no-op stubs (this TU
   is `.cpp`, so no Objective-C). Real iOS networking is still milestone 4.

3. **"No binary produced" after a 100% link.** jsoncpp's subproject CMakeLists
   sets `CMAKE_RUNTIME_OUTPUT_DIRECTORY` globally, relocating our executable to
   `jsoncpp/jsoncpp/bin/` (depth 4), past `build.sh`'s `-maxdepth 3` search.
   Re-pinned `set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/build)`
   at the top of `minecraftpe/CMakeLists.txt` (as in the reference port) so the
   binary lands in `build/` and lipo/sign/ipa pick it up.

Verified on Debian 12 with clang-14: both slices
(`armv7-apple-ios5.0`, `arm64-apple-ios7.0`) compile + link, `lipo` fuses a fat
Mach-O, `ldid` signs it, and `build-ipa.sh` produces `build/ModifiedEight.ipa`.
Note `minecraftpe/impl/pcm_data.c` is gitignored (proprietary sounds): drop one
in via `tools/get_sound_data.py` or `tools/gen_silent_pcm.py` before linking.
