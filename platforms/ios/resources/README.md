# platforms/ios/resources

Everything that gets bundled into the `.app` lives here. `build-ipa.sh` copies
this directory's contents into `Payload/modifiedeight.app/`.

## Files

- `Info.plist` — the app's bundle metadata. **Edit this directly** (it is no
  longer generated inline by `build-ipa.sh`). Landscape-only, status bar hidden,
  launch-image + icon keys already wired.
- `Default.png` / `Default@2x.png` / `Default-568h@2x.png` — launch images.
  `Default-568h@2x.png` (640x1136) is the iPhone 5 / 5s tall-screen launch image;
  its presence is also how iOS knows the app supports the 4-inch screen. Replace
  these placeholders with real splash art (keep the exact filenames + sizes).
- `Icon.png` (57x57), `Icon@2x.png` (114x114), `Icon-72.png` (72x72),
  `Icon-72@2x.png` (144x144) — home-screen icons (iPhone + iPad, non-retina +
  retina). Replace with real art at the same sizes.
- `assets/` — **unpack the game's resources here.** Extract `assets/` from a real
  MCPE 0.8.1 APK into this folder. At runtime `AppPlatform_iOS` resolves files
  against `<bundle>/assets/...` (textures, gui, lang, sounds metadata, etc.), so
  the layout must match the APK's `assets/` tree:

  ```
  resources/assets/
  ├── images/      (textures: terrain.png, gui/, item/, ...)
  ├── gui/
  ├── lang/
  ├── font/
  └── ...
  ```

  Only `.gitkeep` is committed — the real assets are copyrighted and must be
  supplied by the builder.

## Launch image sizes (reference)

| File                    | Size      | Device                         |
|-------------------------|-----------|--------------------------------|
| `Default.png`           | 320x480   | iPhone / iPod (non-retina)     |
| `Default@2x.png`        | 640x960   | iPhone 4 / 4s (retina 3.5")    |
| `Default-568h@2x.png`   | 640x1136  | iPhone 5 / 5s / 5c (retina 4") |

> Note: pre-iOS 7 expects **portrait** launch images even for a landscape app;
> iOS rotates them. That's why these are portrait-oriented.
