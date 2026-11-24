"""Coloca una instancia de BP_Door en Lvl_ThirdPerson si aún no hay ninguna."""
import unreal

MAP_PATH = "/Game/MyGame/Maps/Lvl_ThirdPerson"
BP_PATH = "/Game/MyGame/Actors/BP_Door"


def find_existing_door(editor_world) -> bool:
    for actor in unreal.GameplayStatics.get_all_actors_of_class(editor_world, unreal.DoorActor):
        unreal.log(f"Door already in level: {actor.get_name()}")
        return True
    return False


def place_door() -> None:
    if not unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH):
        unreal.log_error(f"Failed to load map {MAP_PATH}")
        return

    editor_world = unreal.EditorLevelLibrary.get_editor_world()
    if find_existing_door(editor_world):
        return

    bp_class = unreal.EditorAssetLibrary.load_blueprint_class(BP_PATH)
    if not bp_class:
        unreal.log_error(f"Failed to load class {BP_PATH}")
        return

    # Delante del PlayerStart si existe; si no, origen + offset.
    spawn_loc = unreal.Vector(400.0, 0.0, 100.0)
    spawn_rot = unreal.Rotator(0.0, 0.0, 0.0)
    for ps in unreal.GameplayStatics.get_all_actors_of_class(editor_world, unreal.PlayerStart):
        spawn_loc = ps.get_actor_location() + ps.get_actor_forward_vector() * 500.0
        spawn_loc.z = ps.get_actor_location().z + 100.0
        spawn_rot = ps.get_actor_rotation()
        break

    door = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_class, spawn_loc, spawn_rot)
    if not door:
        unreal.log_error("Failed to spawn BP_Door")
        return

    door.set_actor_label("BP_Door")
    unreal.EditorLevelLibrary.save_current_level()
    unreal.log(f"Placed BP_Door at {spawn_loc}")


place_door()
