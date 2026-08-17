# ModifiedEight — AI Rules & Gotchas

> **Для ИИ-ассистентов.** Этот файл описывает критические инварианты проекта, которые **нельзя нарушать**. Перед правкой любого из перечисленных файлов — прочти раздел целиком.

---

## 1. Генерация пещер (LargeCaveFeature)

**Файлы:**
- `modifiedeight-newadditions/impl/level/gen/feature/LargeCaveFeature.cpp`
- `modifiedeight/impl/level/gen/feature/LargeCaveFeature.cpp` (всегда синхронизируй с newadditions)

### Правильная логика addFeature

```cpp
uint32_t v12 = p_random->genrand_int32();
uint32_t v13 = p_random->genrand_int32();
uint32_t v14 = p_random->genrand_int32();
uint32_t caveCount;
if (p_random->genrand_int32() % 0xF) {   // ОБЯЗАТЕЛЬНО % 0xF (15), не % 7
    caveCount = 0;
} else {
    caveCount = v14 % (v13 % (v12 % 0x28u + 1) + 1) + 1;  // из оригинального декомпайла
}
```

### Что ломает пещеры

| Ошибка | Симптом |
|--------|---------|
| % 7 вместо % 0xF | Другая плотность пещер |
| Своя формула подсчёта | Пещеры только у спавна |
| Изменение totalSteps | Туннели не достигают дальних чанков |
| Изменение range (по умолчанию 8) | Perf-удар или пещеры выпадают |

Сигнатура addTunnel — с доп. параметром seed (из .hpp):
```cpp
void addTunnel(int32_t, int32_t, uint8_t*, float, float, float,
               float, float, float, int32_t, int32_t, float, uint32_t = 0);
```
Не удаляй `uint32_t = 0` из заголовка.

---

## 2. Жидкости (MultiPlayerLevel)

**Файлы:**
- `modifiedeight-newadditions/impl/level/MultiPlayerLevel.cpp`
- `modifiedeight-newadditions/impl/tile/LiquidTileDynamic.cpp`

### Правильный MultiPlayerLevel

```cpp
// tick() — серверная ветка НЕ должна вызывать this->tick() (рекурсия!)
void MultiPlayerLevel::tick() {
    if(this->isClientMaybe) {
        // ... клиентский код ...
    } else {
        Level::tick();   // ОБЯЗАТЕЛЬНО Level::tick(), НЕ this->tick()
    }
}

// tickPendingTicks — на сервере ОБЯЗАН делегировать в Level
bool_t MultiPlayerLevel::tickPendingTicks(bool_t a2) {
    if(!this->isClientMaybe) {
        return Level::tickPendingTicks(a2);  // иначе очередь никогда не разгребается
    }
    return 0;
}
```

### Что ломает жидкости

| Ошибка | Симптом |
|--------|---------|
| `this->tick()` в else-ветке | Бесконечная рекурсия, краш |
| `tickPendingTicks` возвращает 0 | Жидкости вообще не разливаются |
| Удаление `addToTickNextTick` override | Жидкости не регистрируются |

---

## 3. Чанки и генератор

**Файл:** `modifiedeight-newadditions/impl/level/dimension/Dimension.cpp`

```cpp
if (genVer >= 1) {
    return new NewRandomLevelSource(...);   // для всех новых миров
}
return new RandomLevelSource(...);          // только genVer == 0
```

Не меняй порог `>= 1` — иначе чанки будут резаться на границах.

### LargeFeature::apply

- `range = 8` — стандарт из оригинала. Не увеличивай.
- `totalSteps ≈ range * 16 = 128` шагов = 128 блоков достижимости.
- Пещеры генерируются только при ПЕРВОМ создании чанка.

---

## 4. TickNextTickData — сортировка

**Файл:** `modifiedeight-newadditions/impl/level/TickNextTickData.cpp`

```cpp
bool_t TickNextTickData::operator<(const TickNextTickData& a2) const {
    if(this->delay < a2.delay) return 1;
    if(this->delay > a2.delay) return 0;
    if(this->id < a2.id) return 1;
    if(this->id > a2.id) return 0;
    return 0;
}
```

Не сравнивай `id` с `delay` — частая ошибка декомпайла.

---

## 5. Общие правила

1. **Всегда собирай:** `cmake --build build -j$(nproc)`
2. **Reference:** `/home/eqozqq/Downloads/текстуры/mcpe-eqozqq-.../minecraftpe-newadditions/`
3. **Синхронизируй:** после правки `modifiedeight-newadditions/impl/...` копируй в `modifiedeight/impl/...`
4. **isClientMaybe:** `true` = клиент (нет физики), `false` = сервер (полная симуляция)
