"""Crea /Game/MyGame/Actors/BP_Door basado en ADoorActor (mesh cubo ya en C++)."""
import unreal

ASSET_PATH = "/Game/MyGame/Actors/BP_Door"
PARENT_CLASS = unreal.DoorActor


def ensure_directory(package_path: str) -> None:
    if not unreal.EditorAssetLibrary.does_directory_exist(package_path):
        unreal.EditorAssetLibrary.make_directory(package_path)


def create_bp_door() -> None:
    ensure_directory("/Game/MyGame/Actors")

    if unreal.EditorAssetLibrary.does_asset_exist(ASSET_PATH):
        unreal.log(f"BP_Door already exists: {ASSET_PATH}")
        return

    factory = unreal.BlueprintFactory()
    factory.set_editor_property("parent_class", PARENT_CLASS)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    bp = asset_tools.create_asset("BP_Door", "/Game/MyGame/Actors", unreal.Blueprint, factory)
    if not bp:
        unreal.log_error("Failed to create BP_Door")
        return

    unreal.EditorAssetLibrary.save_asset(ASSET_PATH)
    unreal.log(f"Created {ASSET_PATH}")


create_bp_door()
