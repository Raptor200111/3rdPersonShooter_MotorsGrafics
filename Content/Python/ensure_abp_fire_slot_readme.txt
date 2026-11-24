Animación de disparo (ABP_BelicaMyNewCharacter)
================================================

El C++ ya llama PlayAnimMontage y el log muestra result=1.000.
Si no se ve la animación: falta el nodo Slot en el Anim Graph.

Pasos (2 minutos):
1. Abre Content/MyGame/Animations/ABP_BelicaMyNewCharacter
2. Pestaña AnimGraph
3. Clic derecho → Slot → Animation Slot
4. Cadena:

   MainState → Slot (DefaultSlot) → Modify Bone (AimPitch) → Output Pose

5. Compile + Save
6. BP_PlayerCharacter → Class Defaults → Combat → Fire Montage =
   Primary_Fire_Med_Montage
7. Abre el Montage y confirma Slot = DefaultSlot

Sin el Slot, el montaje “suena” en el AnimInstance pero el grafo lo ignora.
