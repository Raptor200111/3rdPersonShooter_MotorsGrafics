import unreal

def fix_belica():
    recovery = "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jump_Recovery"
    additive = "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jump_Recovery_Additive"

    # Rebuild Additive asset as a clean duplicate of full-body Recovery
    if unreal.EditorAssetLibrary.does_asset_exist(additive):
        unreal.EditorAssetLibrary.delete_asset(additive)
    ok = unreal.EditorAssetLibrary.duplicate_asset(recovery, additive)
    unreal.log("[MG_R1] duplicate Additive from Recovery: " + str(ok))

    for path in [
        additive, recovery,
        "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jump_Start",
        "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jump_PreLand",
        "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jump_Apex",
        "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Idle_Relaxed",
        "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jog_Fwd",
        "/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations/Jog_Fwd_Uphill",
    ]:
        a = unreal.EditorAssetLibrary.load_asset(path)
        if not a:
            continue
        a.set_editor_property("additive_anim_type", unreal.AdditiveAnimationType.AAT_NONE)
        unreal.EditorAssetLibrary.save_loaded_asset(a)
        unreal.log("[MG_R1] NoAdditive " + path)

    # Prefer ABP Land uses Jump_Recovery (non-additive name) if we can find sequence players
    abp = unreal.EditorAssetLibrary.load_asset("/Game/MyGame/Animations/ABP_BelicaMyNewCharacter")
    recovery_asset = unreal.EditorAssetLibrary.load_asset(recovery)
    unreal.log("[MG_R1] ABP loaded=" + str(abp is not None) + " recovery=" + str(recovery_asset is not None))

    bp = unreal.EditorAssetLibrary.load_asset("/Game/MyGame/Characters/BP_PlayerCharacter")
    if bp:
        cdo = unreal.get_default_object(bp.generated_class())
        cdo.set_editor_property("use_controller_rotation_yaw", True)
        move = cdo.get_editor_property("character_movement")
        move.set_editor_property("orient_rotation_to_movement", False)
        for arm in cdo.get_components_by_class(unreal.SpringArmComponent):
            arm.set_editor_property("do_collision_test", False)
            arm.set_editor_property("target_arm_length", 500.0)
            arm.set_editor_property("use_pawn_control_rotation", True)
        unreal.EditorAssetLibrary.save_loaded_asset(bp)
        unreal.log("[MG_R1] BP_PlayerCharacter fixed")

    unreal.EditorAssetLibrary.save_directory("/Game/ParagonLtBelica/Characters/Heroes/Belica/Animations")
    unreal.log("[MG_R1] Belica landing/orient fix DONE")

fix_belica()
