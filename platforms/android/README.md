# Android Platform Integration

This directory provides the Android project wrapper for building ModifiedEight directly from the root C++ codebase.

## Building APKs

Run Gradle from this directory:

- Build Classic APK:
  ```bash
  ./gradlew assembleClassicRelease
  ```

- Build New Additions APK:
  ```bash
  ./gradlew assembleNewadditionsRelease
  ```

- Build both APKs:
  ```bash
  ./gradlew assembleClassicRelease assembleNewadditionsRelease
  ```

The generated APKs will be in `app/build/outputs/apk/`.
