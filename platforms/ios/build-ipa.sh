#!/bin/sh
# shellcheck disable=2016
set -e

ipaname="${IPA_NAME:-ModifiedEight.ipa}"
bin="${1:-build/modifiedeight}"
execname="$(basename "$bin")"

# Which client's bundle metadata to stamp: "classic" or "newadditions".
# Derived from NBC_CLIENT (exported by build.sh) or the ipa name, so the two
# variants get distinct bundle identifiers/names and can coexist on a device.
nbcclient="${NBC_CLIENT:-classic}"
case "$ipaname" in *NewAdditions*|*newadditions*) nbcclient=newadditions ;; esac

platformdir='platforms/ios'
resdir="$platformdir/resources"
builddir="$platformdir/build"

if [ "$nbcclient" = "newadditions" ]; then
    target_sub="new-additions/assets"
else
    target_sub="classic/assets"
fi

if [ -n "$ASSET_DIR" ]; then
    assetdir="$ASSET_DIR"
elif [ -d "assets_repo/$target_sub" ]; then
    assetdir="assets_repo/$target_sub"
elif [ -d "$platformdir/build/assets_repo/$target_sub" ]; then
    assetdir="$platformdir/build/assets_repo/$target_sub"
elif [ -d "$resdir/assets" ] && [ -n "$(ls -A "$resdir/assets" 2>/dev/null | grep -v '^\.gitkeep$')" ]; then
    assetdir="$resdir/assets"
else
    printf 'Cloning assets from https://github.com/ModifiedEight/assets.git...\n'
    git clone --depth 1 https://github.com/ModifiedEight/assets.git "$platformdir/build/assets_repo" || true
    if [ -d "$platformdir/build/assets_repo/$target_sub" ]; then
        assetdir="$platformdir/build/assets_repo/$target_sub"
    else
        assetdir="$resdir/assets"
    fi
fi

ipadir="$builddir/ipa"
apppath="$ipadir/Payload/$execname.app"

[ "${0%/*}" = "$0" ] && scriptroot="." || scriptroot="${0%/*}"
cd "$scriptroot/../.."

if ! [ -f "$bin" ]; then
    printf 'Expected working binary at %s.\n' "$bin"
    printf 'Please do a cmake build before running this script.\n'
    exit 1
fi

if ! command -v plistutil >/dev/null; then
    printf 'plistutil not found!\n'
    exit 1
fi

rm -rf "$ipadir"
mkdir -p "$apppath"
cp "$bin" "$apppath/$execname"

# Info.plist: prefer the editable one in resources/, fall back to a minimal
# inline default so the build still works on a bare checkout.
if [ -f "$resdir/Info.plist" ]; then
    cp "$resdir/Info.plist" "$apppath/Info.plist"
else
    cat > "$apppath/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleExecutable</key>
	<string>$execname</string>
	<key>CFBundleIdentifier</key>
	<string>com.mojang.minecraftpe081</string>
	<key>CFBundleName</key>
	<string>$execname</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>0.8.1</string>
	<key>CFBundleVersion</key>
	<string>1</string>
	<key>LSRequiresIPhoneOS</key>
	<true/>
</dict>
</plist>
EOF
fi

# Ensure CFBundleExecutable matches execname
sed -i "/<key>CFBundleExecutable<\/key>/{n;s#<string>.*</string>#<string>$execname</string>#;}" "$apppath/Info.plist"

# For the New Additions variant, give the bundle a distinct identifier and
# display name so it installs alongside the classic build instead of replacing
# it.
if [ "$nbcclient" = "newadditions" ]; then
    plist="$apppath/Info.plist"
    # Append .newadditions to the bundle id (only the first CFBundleIdentifier).
    sed -i '/<key>CFBundleIdentifier<\/key>/{n;s#<string>\(.*\)</string>#<string>\1.newadditions</string>#;}' "$plist"
    # Rename the visible app name.
    sed -i '/<key>CFBundleName<\/key>/{n;s#<string>.*</string>#<string>ModifiedEight New Additions</string>#;}' "$plist"
    sed -i '/<key>CFBundleDisplayName<\/key>/{n;s#<string>.*</string>#<string>ModifiedEight New Additions</string>#;}' "$plist"
fi

# Launch images + icons live at the .app root (legacy naming).
if [ -d "$resdir" ]; then
    for f in "$resdir"/Default*.png "$resdir"/Icon*.png; do
        [ -f "$f" ] && cp "$f" "$apppath/"
    done
fi

# Game assets -> <app>/assets (AppPlatform_iOS resolves bundle + "assets/...").
if [ -d "$assetdir" ]; then
    rm -rf "$apppath/assets"
    cp -a "$assetdir" "$apppath/assets"
    if [ -z "$(ls -A "$assetdir" 2>/dev/null | grep -v '^\.gitkeep$')" ]; then
        printf 'WARNING: %s is empty. Unpack a real MCPE 0.8.1 APK assets/ there;\n' "$assetdir"
        printf '         the app will launch but cannot load textures/UI.\n'
    fi
else
    printf 'WARNING: no asset dir at %s (set ASSET_DIR). App will lack assets.\n' "$assetdir"
fi

cd "$ipadir"
rm -f "../$ipaname"
zip -r "../$ipaname" Payload >/dev/null

printf '\nDone! Your IPA is at %s/%s\n' "$builddir" "$ipaname"
