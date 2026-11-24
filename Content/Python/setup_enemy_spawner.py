"""Crea BP_EnemySpawner / BP_EnemyCube y coloca el spawner en Lvl_ThirdPerson."""
import unreal

SPAWNER_BP = "/Game/MyGame/Actors/BP_EnemySpawner"
ENEMY_BP = "/Game/MyGame/Actors/BP_EnemyCube"
MAP_PATH = "/Game/MyGame/Maps/Lvl_ThirdPerson"


def ensure_bp(asset_path: str, asset_name: str, parent_class) -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        unreal.log(f"Exists {asset_path}")
        return
    unreal.EditorAssetLibrary.make_directory("/Game/MyGame/Actors")
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", parent_class)
    bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name, "/Game/MyGame/Actors", unreal.Blueprint, factory
    )
    if bp:
        unreal.EditorAssetLibrary.save_asset(asset_path)
        unreal.log(f"Created {asset_path}")
    else:
        unreal.log_error(f"Failed {asset_path}")


def place_spawner() -> None:
    if not unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH):
        unreal.log_error(f"Failed to load {MAP_PATH}")
        return

    world = unreal.EditorLevelLibrary.get_editor_world()
    for actor in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.EnemySpawnerActor):
        unreal.log(f"Spawner already in level: {actor.get_name()}")
        return

    bp_class = unreal.EditorAssetLibrary.load_blueprint_class(SPAWNER_BP)
    if not bp_class:
        bp_class = unreal.EnemySpawnerActor
    if not bp_class:
        unreal.log_error("No spawner class")
        return

    spawn_loc = unreal.Vector(800.0, 0.0, 100.0)
    for ps in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.PlayerStart):
        spawn_loc = ps.get_actor_location() + ps.get_actor_forward_vector() * 900.0
        spawn_loc.z = ps.get_actor_location().z
        break

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_class, spawn_loc, unreal.Rotator(0, 0, 0))
    if actor:
        actor.set_actor_label("BP_EnemySpawner")
        unreal.EditorLevelLibrary.save_current_level()
        unreal.log(f"Placed spawner at {spawn_loc}")
    else:
        unreal.log_error("Spawn failed")


def main() -> None:
    ensure_bp(ENEMY_BP, "BP_EnemyCube", unreal.EnemyCubeActor)
    ensure_bp(SPAWNER_BP, "BP_EnemySpawner", unreal.EnemySpawnerActor)
    place_spawner()
    unreal.log("setup_enemy_spawner DONE")


main()
