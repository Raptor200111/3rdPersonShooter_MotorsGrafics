"""Crea IA_ToggleWeaponDebug (duplicando IA_Fire) y lo asigna al BP.
El bind de tecla T está en C++ (EKeys::T); el IMC es opcional.
"""
import unreal

IA_PATH = "/Game/MyGame/Input/IA_ToggleWeaponDebug"
IA_FIRE = "/Game/MyGame/Input/IA_Fire"
BP_CANDIDATES = (
    "/Game/MyGame/Characters/BP_PlayerCharacter",
    "/Game/MyGame/BP_PlayerCharacter",
)


def ensure_input_action():
    if unreal.EditorAssetLibrary.does_asset_exist(IA_PATH):
        unreal.log(f"IA exists: {IA_PATH}")
        return unreal.EditorAssetLibrary.load_asset(IA_PATH)

    if not unreal.EditorAssetLibrary.does_asset_exist(IA_FIRE):
        unreal.log_error(f"Template missing: {IA_FIRE}")
        return None

    source = unreal.EditorAssetLibrary.load_asset(IA_FIRE)
    dup = unreal.AssetToolsHelpers.get_asset_tools().duplicate_asset(
        "IA_ToggleWeaponDebug", "/Game/MyGame/Input", source
    )
    if not dup:
        # Fallback API
        if unreal.EditorAssetLibrary.duplicate_asset(IA_FIRE, IA_PATH):
            dup = unreal.EditorAssetLibrary.load_asset(IA_PATH)
    if not dup:
        unreal.log_error("Failed to create IA_ToggleWeaponDebug")
        return None

    try:
        dup.set_editor_property("value_type", unreal.InputActionValueType.BOOLEAN)
    except Exception:
        pass
    unreal.EditorAssetLibrary.save_asset(IA_PATH)
    unreal.log(f"Created {IA_PATH}")
    return dup


def assign_action_to_player_bp(action) -> None:
    bp_path = None
    for candidate in BP_CANDIDATES:
        if unreal.EditorAssetLibrary.does_asset_exist(candidate):
            bp_path = candidate
            break
    if not bp_path:
        unreal.log_warning("BP_PlayerCharacter not found")
        return

    bp_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if not bp_class:
        unreal.log_warning(f"Could not load class for {bp_path}")
        return

    cdo = unreal.get_default_object(bp_class)
    for prop in ("toggle_weapon_debug_action", "ToggleWeaponDebugAction"):
        try:
            cdo.set_editor_property(prop, action)
            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log(f"Set {prop} on {bp_path}")
            return
        except Exception as exc:
            unreal.log(f"set {prop}: {exc}")

    unreal.log("ToggleWeaponDebugAction optional; C++ BindKey(T) is enough")


def main() -> None:
    action = ensure_input_action()
    if action:
        assign_action_to_player_bp(action)
    unreal.log("setup_weapon_debug_input DONE (T bound in C++)")


main()
