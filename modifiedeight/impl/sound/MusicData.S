#if defined(MCPE_IOS) || defined(__APPLE__)
.section __TEXT,__const
#else
.section .rodata
#endif

.globl music_danny_start
.globl _music_danny_start
.globl music_danny_end
.globl _music_danny_end

.globl music_door_start
.globl _music_door_start
.globl music_door_end
.globl _music_door_end

.globl music_haggstrom_start
.globl _music_haggstrom_start
.globl music_haggstrom_end
.globl _music_haggstrom_end

.globl music_key_start
.globl _music_key_start
.globl music_key_end
.globl _music_key_end

.globl music_living_mice_start
.globl _music_living_mice_start
.globl music_living_mice_end
.globl _music_living_mice_end

.globl music_mice_on_venus_start
.globl _music_mice_on_venus_start
.globl music_mice_on_venus_end
.globl _music_mice_on_venus_end

.globl music_minecraft_start
.globl _music_minecraft_start
.globl music_minecraft_end
.globl _music_minecraft_end

.globl music_moog_city_2_start
.globl _music_moog_city_2_start
.globl music_moog_city_2_end
.globl _music_moog_city_2_end

.globl music_oxygene_start
.globl _music_oxygene_start
.globl music_oxygene_end
.globl _music_oxygene_end

.globl music_subwoofer_lullaby_start
.globl _music_subwoofer_lullaby_start
.globl music_subwoofer_lullaby_end
.globl _music_subwoofer_lullaby_end

.globl music_equinoxe_start
.globl _music_equinoxe_start
.globl music_equinoxe_end
.globl _music_equinoxe_end

music_danny_start:
_music_danny_start:
.incbin "assets/music/danny.aac"
music_danny_end:
_music_danny_end:

music_door_start:
_music_door_start:
.incbin "assets/music/door.aac"
music_door_end:
_music_door_end:

music_haggstrom_start:
_music_haggstrom_start:
.incbin "assets/music/haggstrom.aac"
music_haggstrom_end:
_music_haggstrom_end:

music_key_start:
_music_key_start:
.incbin "assets/music/key.aac"
music_key_end:
_music_key_end:

music_living_mice_start:
_music_living_mice_start:
.incbin "assets/music/living_mice.aac"
music_living_mice_end:
_music_living_mice_end:

music_mice_on_venus_start:
_music_mice_on_venus_start:
.incbin "assets/music/mice_on_venus.aac"
music_mice_on_venus_end:
_music_mice_on_venus_end:

music_minecraft_start:
_music_minecraft_start:
.incbin "assets/music/minecraft.aac"
music_minecraft_end:
_music_minecraft_end:

music_moog_city_2_start:
_music_moog_city_2_start:
.incbin "assets/music/moog_city_2.aac"
music_moog_city_2_end:
_music_moog_city_2_end:

music_oxygene_start:
_music_oxygene_start:
.incbin "assets/music/oxygene.aac"
music_oxygene_end:
_music_oxygene_end:

music_subwoofer_lullaby_start:
_music_subwoofer_lullaby_start:
.incbin "assets/music/subwoofer_lullaby.aac"
music_subwoofer_lullaby_end:
_music_subwoofer_lullaby_end:

music_equinoxe_start:
_music_equinoxe_start:
.incbin "assets/music/equinoxe.aac"
music_equinoxe_end:
_music_equinoxe_end:
