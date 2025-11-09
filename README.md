# Billiard Game — Skeleton (C++ / SFML-ready)

Ini adalah setup awal "kosongan" dengan struktur profesional. Belum ada gameplay/engine; hanya rangka folder dan target build minimal yang bisa dikompilasi tanpa SFML. Adapter SFML disiapkan sebagai placeholder.

## Struktur Direktori

```
include/
  billiard/
    core/      # Konfigurasi inti, versi, logger (nanti)
    math/      # Util matematika (nanti)
    physics/   # Fisika & collision (nanti)
    gameplay/  # Entitas game (nanti)
    graphics/  # Interface renderer (nanti)
    input/     # Interface input (nanti)
    platform/
      sfml/    # Adapter SFML (nanti)
src/
  core/
  math/
  physics/
  gameplay/
  platform/sfml/
  game/
assets/
  fonts/
  textures/
  sounds/
tests/
docs/
.github/workflows/
```

## Build Cepat

```bash
cmake --preset dev
cmake --build build/dev -j
./build/dev/billiard_game
```

## Aktifkan SFML (nanti)

- Ubah preset atau tambahkan flag `-DBILLIARD_WITH_SFML=ON` lalu implementasikan adapter di `platform/sfml/`.
- CI default mem-build tanpa SFML agar skeleton bisa jalan di semua runner.
