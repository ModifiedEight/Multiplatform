# ModifiedEight
Modified MCPE Alpha 0.8.1 client with new stuff

[This repository is used as a basis](https://github.com/oldminecraftcommunity/MCPE-0.8.1)

## Building

## Cloning the repo and extracting sounds
* Clone the repo with `--recursive` flag(or use `git submodule init` and `git submodule update` after cloning if you forgot to add it)
* Run `python tools/get_sound_data.py <path/to/libminecraftpe.so>` - it should generate `pcm_data.c`
* Move `pcm_data.c` to `./modifiedeight/impl/`

### Linux
Requires SDL1(`libsdl1.2-dev` and `libsdl1.2debian`), OpenAL(`libopenal-dev` and `libopenal1`), OpenGL(`libgl1-mesa-dev`), zlib (`zlib1g` and `zlib1g-dev`), curl (`libcurl4` and `libcurl4-openssl-dev`).
You must also have original 0.8.1 apk file to extract sounds(won't compile without them) and obtain assets(the compiled file will crash or not work properly without them).
* ```
  mkdir build
  cd build
  cmake ..
  make
  ```
* the output should be `build/modifiedeight/build` and `build/modifiedeight-newadditions/build`

### Android
See [platforms/android/README.md](platforms/android/README.md)

### Windows
Windows version does not require OpenAL(it uses DirectSound instead). See [.github/workflows/main.yml](https://github.com/oldminecraftcommunity/MCPE-0.8.1/blob/master/.github/workflows/main.yml#L37) for a way to compile it for Windows. 

### Running
* extract `assets` from real MCPE 0.8.1 apk into the folder you're running the executable from
* run compiled executable

## Some additional info:
* JSON library that was probably used by Mojang: https://chromium.googlesource.com/external/jsoncpp/+/6921bf1feef6f1fb83935ae3943f07753488311d/jsoncpp
* RakNet: https://web.archive.org/web/20260101222408if_/http://www.raknet.com/raknet/downloads/RakNet_PC-4.036.zip (might be some other version, probably modifed by mojang in 0.1.x)
* GZIP stuff - zlib 1.2.3, based on https://zlib.net/zpipe.c
* https://github.com/nothings/stb/
* GLM - commit before https://github.com/g-truc/glm/commit/2b747cbbadfd3af39b443e88902f1c98bd231083 and -DGLM_FORCE_RADIANS <?>
* OpenAES - used for realms stuff