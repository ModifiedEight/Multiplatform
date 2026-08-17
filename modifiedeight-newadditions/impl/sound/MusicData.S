#if defined(__APPLE__)
#define SYM(x) _##x
.section __TEXT,__const
#else
#define SYM(x) x
.section .rodata
#endif

.global SYM(music_danny_start)
.global SYM(music_danny_end)
.global SYM(music_door_start)
.global SYM(music_door_end)
.global SYM(music_haggstrom_start)
.global SYM(music_haggstrom_end)
.global SYM(music_key_start)
.global SYM(music_key_end)
.global SYM(music_living_mice_start)
.global SYM(music_living_mice_end)
.global SYM(music_mice_on_venus_start)
.global SYM(music_mice_on_venus_end)
.global SYM(music_minecraft_start)
.global SYM(music_minecraft_end)
.global SYM(music_moog_city_2_start)
.global SYM(music_moog_city_2_end)
.global SYM(music_oxygene_start)
.global SYM(music_oxygene_end)
.global SYM(music_subwoofer_lullaby_start)
.global SYM(music_subwoofer_lullaby_end)
.global SYM(music_equinoxe_start)
.global SYM(music_equinoxe_end)

SYM(music_danny_start):
.incbin "assets/music/danny.aac"
SYM(music_danny_end):

SYM(music_door_start):
.incbin "assets/music/door.aac"
SYM(music_door_end):

SYM(music_haggstrom_start):
.incbin "assets/music/haggstrom.aac"
SYM(music_haggstrom_end):

SYM(music_key_start):
.incbin "assets/music/key.aac"
SYM(music_key_end):

SYM(music_living_mice_start):
.incbin "assets/music/living_mice.aac"
SYM(music_living_mice_end):

SYM(music_mice_on_venus_start):
.incbin "assets/music/mice_on_venus.aac"
SYM(music_mice_on_venus_end):

SYM(music_minecraft_start):
.incbin "assets/music/minecraft.aac"
SYM(music_minecraft_end):

SYM(music_moog_city_2_start):
.incbin "assets/music/moog_city_2.aac"
SYM(music_moog_city_2_end):

SYM(music_oxygene_start):
.incbin "assets/music/oxygene.aac"
SYM(music_oxygene_end):

SYM(music_subwoofer_lullaby_start):
.incbin "assets/music/subwoofer_lullaby.aac"
SYM(music_subwoofer_lullaby_end):

SYM(music_equinoxe_start):
.incbin "assets/music/equinoxe.aac"
SYM(music_equinoxe_end):
