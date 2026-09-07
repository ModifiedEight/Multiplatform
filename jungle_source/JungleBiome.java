package net.minecraft.world.level.biome;

import net.minecraft.world.entity.EntityType;
import net.minecraft.world.entity.MobCategory;
import net.minecraft.world.level.biome.Biome.BiomeBuilder;
import net.minecraft.world.level.biome.Biome.BiomeCategory;
import net.minecraft.world.level.biome.Biome.Precipitation;
import net.minecraft.world.level.biome.Biome.SpawnerData;
import net.minecraft.world.level.levelgen.feature.Feature;
import net.minecraft.world.level.levelgen.feature.FeatureConfiguration;
import net.minecraft.world.level.levelgen.feature.MineshaftConfiguration;
import net.minecraft.world.level.levelgen.feature.MineshaftFeature.Type;
import net.minecraft.world.level.levelgen.surfacebuilders.SurfaceBuilder;

public final class JungleBiome extends Biome {
   public JungleBiome() {
      super(
         new BiomeBuilder()
            .surfaceBuilder(SurfaceBuilder.DEFAULT, SurfaceBuilder.CONFIG_GRASS)
            .precipitation(Precipitation.RAIN)
            .biomeCategory(BiomeCategory.JUNGLE)
            .depth(0.1F)
            .scale(0.2F)
            .temperature(0.95F)
            .downfall(0.9F)
            .waterColor(4159204)
            .waterFogColor(329011)
            .parent(null)
      );
      this.addStructureStart(Feature.JUNGLE_TEMPLE, FeatureConfiguration.NONE);
      this.addStructureStart(Feature.MINESHAFT, new MineshaftConfiguration(0.004, Type.NORMAL));
      this.addStructureStart(Feature.STRONGHOLD, FeatureConfiguration.NONE);
      BiomeDefaultFeatures.addDefaultCarvers(this);
      BiomeDefaultFeatures.addStructureFeaturePlacement(this);
      BiomeDefaultFeatures.addDefaultLakes(this);
      BiomeDefaultFeatures.addDefaultMonsterRoom(this);
      BiomeDefaultFeatures.addDefaultUndergroundVariety(this);
      BiomeDefaultFeatures.addDefaultOres(this);
      BiomeDefaultFeatures.addDefaultSoftDisks(this);
      BiomeDefaultFeatures.addLightBambooVegetation(this);
      BiomeDefaultFeatures.addJungleTrees(this);
      BiomeDefaultFeatures.addWarmFlowers(this);
      BiomeDefaultFeatures.addJungleGrass(this);
      BiomeDefaultFeatures.addDefaultMushrooms(this);
      BiomeDefaultFeatures.addDefaultExtraVegetation(this);
      BiomeDefaultFeatures.addDefaultSprings(this);
      BiomeDefaultFeatures.addJungleExtraVegetation(this);
      BiomeDefaultFeatures.addSurfaceFreezing(this);
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.SHEEP, 12, 4, 4));
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.PIG, 10, 4, 4));
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.CHICKEN, 10, 4, 4));
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.COW, 8, 4, 4));
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.PARROT, 40, 1, 2));
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.PANDA, 1, 1, 2));
      this.addSpawn(MobCategory.CREATURE, new SpawnerData(EntityType.CHICKEN, 10, 4, 4));
      this.addSpawn(MobCategory.AMBIENT, new SpawnerData(EntityType.BAT, 10, 8, 8));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.SPIDER, 100, 4, 4));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.ZOMBIE, 95, 4, 4));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.ZOMBIE_VILLAGER, 5, 1, 1));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.SKELETON, 100, 4, 4));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.CREEPER, 100, 4, 4));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.SLIME, 100, 4, 4));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.ENDERMAN, 10, 1, 4));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.WITCH, 5, 1, 1));
      this.addSpawn(MobCategory.MONSTER, new SpawnerData(EntityType.OCELOT, 2, 1, 1));
   }
}
