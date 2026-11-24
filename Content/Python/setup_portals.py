"""Crea BP_Portal y coloca dos instancias enlazadas en Lvl_ThirdPerson."""
import unreal

BP_PATH = "/Game/MyGame/Actors/BP_Portal"
MAP_PATH = "/Game/MyGame/Maps/Lvl_ThirdPerson"


def ensure_bp():
    if unreal.EditorAssetLibrary.does_asset_exist(BP_PATH):
        unreal.log(f"Exists {BP_PATH}")
        return unreal.EditorAssetLibrary.load_blueprint_class(BP_PATH)

    unreal.EditorAssetLibrary.make_directory("/Game/MyGame/Actors")
    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", unreal.PortalActor)
    bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        "BP_Portal", "/Game/MyGame/Actors", unreal.Blueprint, factory
    )
    if not bp:
        unreal.log_error("Failed BP_Portal")
        return None
    unreal.EditorAssetLibrary.save_asset(BP_PATH)
    unreal.log(f"Created {BP_PATH}")
    return unreal.EditorAssetLibrary.load_blueprint_class(BP_PATH)


def place_pair(bp_class):
    if not unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH):
        unreal.log_error("map load failed")
        return

    world = unreal.EditorLevelLibrary.get_editor_world()
    existing = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.PortalActor)
    if len(existing) >= 2:
        unreal.log("Portals already in level")
        return

    base = unreal.Vector(200.0, 400.0, 100.0)
    for ps in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.PlayerStart):
        base = ps.get_actor_location() + unreal.Vector(300.0, 400.0, 0.0)
        base.z = ps.get_actor_location().z
        break

    a = unreal.EditorLevelLibrary.spawn_actor_from_class(
        bp_class, base, unreal.Rotator(0, 0, 0)
    )
    b = unreal.EditorLevelLibrary.spawn_actor_from_class(
        bp_class, base + unreal.Vector(1200.0, 0.0, 0.0), unreal.Rotator(0, 180, 0)
    )
    if not a or not b:
        unreal.log_error("spawn failed")
        return

    a.set_actor_label("BP_Portal_A")
    b.set_actor_label("BP_Portal_B")

    try:
        a.set_editor_property("destination", b)
        b.set_editor_property("destination", a)
    except Exception as exc:
        unreal.log_warning(f"Set Destination manually A↔B: {exc}")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("Placed Portal A/B linked")


def main():
    cls = ensure_bp()
    if cls:
        place_pair(cls)
    unreal.log("setup_portals DONE — assign MI_Portal material in editor (see TUTORIAL_PORTAL.md)")


main()
