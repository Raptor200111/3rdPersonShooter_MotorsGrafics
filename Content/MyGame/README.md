# MyGame — todo el contenido del proyecto R1

## Estructura real (aquí están los ficheros)

```
MyGame/
├── Actors/          ← BP_Projectile (tú lo creas)
├── Animations/      ← ABP_Player, BlendSpaces, Montages (tú)
├── Characters/      ← BP_PlayerCharacter (tú)  [vacía al inicio]
├── Core/            ← BP_GameMode_R1 (tú)
├── Input/           ← IMC_Default, IA_* (tú)
├── Maps/            ← (reserva; el nivel jugable está en Resources/ThirdPerson)
├── Meshes/Shapes/   ← Shape_Sphere / Cube para el proyectil
└── Resources/       ← recursos Epic necesarios (NO borrar)
    ├── Mannequins/          mesh Quinn + animaciones MM_*
    ├── LevelPrototyping/    geometría del nivel
    └── ThirdPerson/
        ├── Lvl_ThirdPerson.umap     ← TU ESCENARIO
        └── MI_ThirdPersonColWay
```

## Por qué ves también `Content/Characters`, `LevelPrototyping`, `ThirdPerson`

Son **junctions** (accesos directos de carpeta de Windows) que apuntan a `MyGame/Resources/...`.

Unreal guardó el nivel con rutas tipo `/Game/Characters/Mannequins/...` y `/Game/ThirdPerson/Lvl_ThirdPerson`.  
Si solo moviéramos los archivos sin esas rutas, el mapa se rompería. Los junctions hacen que:

- En disco, **todo viva dentro de MyGame/Resources**
- Unreal **siga encontrando** las rutas antiguas

**No borres** esas carpetas-junction del Content root ni `MyGame/Resources`.

## Carpetas del Content root que deben existir

| Carpeta | Rol |
|---------|-----|
| `MyGame/` | Todo tu proyecto |
| `Developers/` | Datos locales del editor (déjala) |
| `__ExternalActors__/` / `__ExternalObjects__/` | Piezas del nivel (World Partition) — necesarias |
| `Characters/`, `LevelPrototyping/`, `ThirdPerson/` | Junctions → `MyGame/Resources/...` |

## C++

Los `.h` / `.cpp` **no** van aquí. Están en `Source/MG_R1/`.

Tutorial: `TUTORIAL_R1_v3.md` en la raíz del repo.
