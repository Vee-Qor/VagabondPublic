// Copyright 2025 Vee.Qor. All Rights Reserved.


#include "VbGameplayTags.h"

namespace VbGameplayTags
{
// ==== Input Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");
UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
UE_DEFINE_GAMEPLAY_TAG(Input_Jump, "Input.Jump");
UE_DEFINE_GAMEPLAY_TAG(Input_Dodge, "Input.Dodge");
UE_DEFINE_GAMEPLAY_TAG(Input_AbilityConfirm, "Input.AbilityConfirm");
UE_DEFINE_GAMEPLAY_TAG(Input_AbilityCancel, "Input.AbilityCancel");
UE_DEFINE_GAMEPLAY_TAG(Input_AbilityOne, "Input.AbilityOne");
UE_DEFINE_GAMEPLAY_TAG(Input_AbilityTwo, "Input.AbilityTwo");
UE_DEFINE_GAMEPLAY_TAG(Input_AbilityThree, "Input.AbilityThree");
UE_DEFINE_GAMEPLAY_TAG(Input_AbilityFour, "Input.AbilityFour");
UE_DEFINE_GAMEPLAY_TAG(Input_Interaction, "Input.Interaction");
UE_DEFINE_GAMEPLAY_TAG(Input_Weapon_Equip, "Input.Weapon.Equip");
UE_DEFINE_GAMEPLAY_TAG(Input_Weapon_Unequip, "Input.Weapon.Unequip");
UE_DEFINE_GAMEPLAY_TAG(Input_Weapon_Switch, "Input.Weapon.Switch");
UE_DEFINE_GAMEPLAY_TAG(Input_Attack_Primary, "Input.Attack.Primary");
UE_DEFINE_GAMEPLAY_TAG(Input_Inventory_Open, "Input.Inventory.Open");
UE_DEFINE_GAMEPLAY_TAG(Input_Equipment_Open, "Input.Equipment.Open");
UE_DEFINE_GAMEPLAY_TAG(Input_GameMenu, "Input.GameMenu");

// ==== Ability Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Ability, "Ability");
UE_DEFINE_GAMEPLAY_TAG(Ability_System, "Ability.System");
UE_DEFINE_GAMEPLAY_TAG(Ability_System_Death, "Ability.System.Death");
UE_DEFINE_GAMEPLAY_TAG(Ability_System_Respawn, "Ability.System.Respawn");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay, "Ability.Gameplay");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Cooldown, "Ability.Gameplay.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Defend_Block, "Ability.Gameplay.Defend.Block");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Control_HitReact, "Ability.Gameplay.Control.HitReact");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Control_Knockdown, "Ability.Gameplay.Control.Knockdown");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Movement_Jump, "Ability.Gameplay.Movement.Jump");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Movement_Dodge, "Ability.Gameplay.Movement.Dodge");

UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack, "Ability.Gameplay.Attack");

UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Grab, "Ability.Gameplay.Attack.Grab");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Stinger, "Ability.Gameplay.Attack.Stinger");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Rage, "Ability.Gameplay.Attack.Rage");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_GroundSlam, "Ability.Gameplay.Attack.GroundSlam");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_RainOfArrows, "Ability.Gameplay.Attack.RainOfArrows");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_RainOfArrows_Cooldown, "Ability.Gameplay.Attack.RainOfArrows.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_MillionStrikes, "Ability.Gameplay.Attack.MillionStrikes");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_MillionStrikes_Cooldown, "Ability.Gameplay.Attack.MillionStrikes.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_FinalCut, "Ability.Gameplay.Attack.FinalCut");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_FinalCut_Cooldown, "Ability.Gameplay.Attack.FinalCut.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_TripleShot, "Ability.Gameplay.Attack.TripleShot");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_TripleShot_Cooldown, "Ability.Gameplay.Attack.TripleShot.Cooldown");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_SpinningBackfist, "Ability.Gameplay.Attack.SpinningBackfist");

UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Melee, "Ability.Gameplay.Attack.Melee");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Range, "Ability.Gameplay.Attack.Range");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Melee_Light, "Ability.Gameplay.Attack.Melee.Light");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Melee_Heavy, "Ability.Gameplay.Attack.Melee.Heavy");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Range_Light, "Ability.Gameplay.Attack.Range.Light");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Attack_Range_Heavy, "Ability.Gameplay.Attack.Range.Heavy");

UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Weapon_Equip, "Ability.Gameplay.Weapon.Equip");
UE_DEFINE_GAMEPLAY_TAG(Ability_Gameplay_Weapon_Unequip, "Ability.Gameplay.Weapon.Unequip");

// ==== Event Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Death, "Event.Ability.Death");
UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Respawn, "Event.Ability.Respawn");
UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Dodge, "Event.Ability.Dodge");
UE_DEFINE_GAMEPLAY_TAG(Event_Ability_HitReact, "Event.Ability.HitReact");
UE_DEFINE_GAMEPLAY_TAG(Event_Ability_Knockdown, "Event.Ability.Knockdown");
UE_DEFINE_GAMEPLAY_TAG(Event_Knockdown_End, "Event.Knockdown.End");
UE_DEFINE_GAMEPLAY_TAG(Event_Stun, "Event.Stun");

UE_DEFINE_GAMEPLAY_TAG(Event_Attack_End, "Event.Attack.End");
UE_DEFINE_GAMEPLAY_TAG(Event_Attack_TraceStart, "Event.Attack.TraceStart");
UE_DEFINE_GAMEPLAY_TAG(Event_Attack_TraceEnd, "Event.Attack.TraceEnd");
UE_DEFINE_GAMEPLAY_TAG(Event_Attack_ProjectileSpawn, "Event.Attack.ProjectileSpawn");
UE_DEFINE_GAMEPLAY_TAG(Event_Attack_ProjectileStart, "Event.Attack.ProjectileStart");

UE_DEFINE_GAMEPLAY_TAG(Event_Grab_Start, "Event.Grab.Start");
UE_DEFINE_GAMEPLAY_TAG(Event_Grab_End, "Event.Grab.End");

UE_DEFINE_GAMEPLAY_TAG(Event_Weapon_Equip, "Event.Weapon.Equip");
UE_DEFINE_GAMEPLAY_TAG(Event_Weapon_Unequip, "Event.Weapon.Unequip");

UE_DEFINE_GAMEPLAY_TAG(Event_Parrying_Start, "Event.Parrying.Start");
UE_DEFINE_GAMEPLAY_TAG(Event_Parrying_End, "Event.Parrying.End");
UE_DEFINE_GAMEPLAY_TAG(Event_Blocked_Hit, "Event.Blocked.Hit");

UE_DEFINE_GAMEPLAY_TAG(Event_Evading_Start, "Event.Evading.Start");
UE_DEFINE_GAMEPLAY_TAG(Event_Evading_End, "Event.Evading.End");

UE_DEFINE_GAMEPLAY_TAG(Event_Ragdoll, "Event.Ragdoll");
UE_DEFINE_GAMEPLAY_TAG(Event_Respawn_End, "Event.Respawn.End");

// ==== State Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
UE_DEFINE_GAMEPLAY_TAG(State_Respawning, "State.Respawning");
UE_DEFINE_GAMEPLAY_TAG(State_Dying, "State.Dying");
UE_DEFINE_GAMEPLAY_TAG(State_Combat_Armed, "State.Combat.Armed");
UE_DEFINE_GAMEPLAY_TAG(State_Combat_Unarmed, "State.Combat.Unarmed");
UE_DEFINE_GAMEPLAY_TAG(State_Damage_Immune, "State.Damage.Immune");
UE_DEFINE_GAMEPLAY_TAG(State_HitReact_Immune, "State.HitReact.Immune");
UE_DEFINE_GAMEPLAY_TAG(State_Health_Full, "State.Health.Full");
UE_DEFINE_GAMEPLAY_TAG(State_Health_Empty, "State.Health.Empty");
UE_DEFINE_GAMEPLAY_TAG(State_Resource_Full, "State.Resource.Full");
UE_DEFINE_GAMEPLAY_TAG(State_Resource_Empty, "State.Resource.Empty");
UE_DEFINE_GAMEPLAY_TAG(State_Poise_Full, "State.Poise.Full");
UE_DEFINE_GAMEPLAY_TAG(State_Poise_Empty, "State.Poise.Empty");

// ==== AI Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(AI_State_Idle, "AI.State.Idle");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Patrol, "AI.State.Patrol");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Combat, "AI.State.Combat");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Search, "AI.State.Search");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Roaming, "AI.State.Roaming");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Chasing, "AI.State.Chasing");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Strafing, "AI.State.Strafing");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Investigate, "AI.State.Investigate");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Phase_One, "AI.State.Phase.One");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Phase_Two, "AI.State.Phase.Two");
UE_DEFINE_GAMEPLAY_TAG(AI_State_Phase_Three, "AI.State.Phase.Three");

// ==== Action Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Action, "Action");
UE_DEFINE_GAMEPLAY_TAG(Action_Jumping, "Action.Jumping");
UE_DEFINE_GAMEPLAY_TAG(Action_Dodging, "Action.Dodging");
UE_DEFINE_GAMEPLAY_TAG(Action_Blocking, "Action.Blocking");
UE_DEFINE_GAMEPLAY_TAG(Action_Attacking, "Action.Attacking");
UE_DEFINE_GAMEPLAY_TAG(Action_HitReacting, "Action.HitReacting");
UE_DEFINE_GAMEPLAY_TAG(Action_Weapon, "Action.Weapon");
UE_DEFINE_GAMEPLAY_TAG(Action_Weapon_Equipping, "Action.Weapon.Equipping");
UE_DEFINE_GAMEPLAY_TAG(Action_Weapon_Unequipping, "Action.Weapon.Unequipping");

// ==== Effect Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Effect_Knockdown, "Effect.Knockdown");
UE_DEFINE_GAMEPLAY_TAG(Effect_Stun, "Effect.Stun");
UE_DEFINE_GAMEPLAY_TAG(Effect_Grabbed, "Effect.Grabbed");
UE_DEFINE_GAMEPLAY_TAG(Effect_Blocking, "Effect.Blocking");
UE_DEFINE_GAMEPLAY_TAG(Effect_Parrying, "Effect.Parrying");
UE_DEFINE_GAMEPLAY_TAG(Effect_Evading, "Effect.Evading");
UE_DEFINE_GAMEPLAY_TAG(Effect_Regen_Health, "Effect.Regen.Health");
UE_DEFINE_GAMEPLAY_TAG(Effect_Regen_Resource, "Effect.Regen.Resource");
UE_DEFINE_GAMEPLAY_TAG(Effect_Regen_Poise, "Effect.Regen.Poise");

// ==== Combat Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Combat_Style_Swordsman, "Combat.Style.Swordsman");
UE_DEFINE_GAMEPLAY_TAG(Combat_Style_Fighter, "Combat.Style.Fighter");
UE_DEFINE_GAMEPLAY_TAG(Combat_Style_Ranger, "Combat.Style.Ranger");
UE_DEFINE_GAMEPLAY_TAG(Combat_Style_Enemy, "Combat.Style.Enemy");

UE_DEFINE_GAMEPLAY_TAG(Combat_Attack_Type_Slash, "Combat.Attack.Type.Slash");
UE_DEFINE_GAMEPLAY_TAG(Combat_Attack_Type_Stab, "Combat.Attack.Type.Stab");
UE_DEFINE_GAMEPLAY_TAG(Combat_Attack_Type_Strike, "Combat.Attack.Type.Strike");

UE_DEFINE_GAMEPLAY_TAG(Combat_Attack_Property_Unblockable, "Combat.Attack.Property.Unblockable");
UE_DEFINE_GAMEPLAY_TAG(Combat_Attack_Property_Unavoidable, "Combat.Attack.Property.Unavoidable");

// ==== Data Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
UE_DEFINE_GAMEPLAY_TAG(Data_Experience, "Data.Experience");

// ==== GameplayCue Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_TakenDamage, "GameplayCue.Hit.React");
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_RainOfArrows, "GameplayCue.RainOfArrows");
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_FinalCut, "GameplayCue.FinalCut");
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_FinalCut_Charge, "GameplayCue.FinalCut.Charge");
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_MillionStrikes, "GameplayCue.MillionStrikes");
UE_DEFINE_GAMEPLAY_TAG(GameplayCue_TripleShot_Charge, "GameplayCue.TripleShot.Charge");

// ==== ItemType Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment, "Item.Equipment");

UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Armor_Helmet, "Item.Equipment.Armor.Helmet");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Armor_Chest, "Item.Equipment.Armor.Chest");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Armor_Boots, "Item.Equipment.Armor.Boots");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Armor_Gloves, "Item.Equipment.Armor.Gloves");

UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Accessory_Necklace, "Item.Equipment.Accessory.Necklace");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Accessory_Belt, "Item.Equipment.Accessory.Belt");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Accessory_Ring, "Item.Equipment.Accessory.Ring");

UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Weapon_Sword, "Item.Equipment.Weapon.Sword");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Weapon_Bow, "Item.Equipment.Weapon.Bow");
UE_DEFINE_GAMEPLAY_TAG(Item_Equipment_Weapon_Gauntlets, "Item.Equipment.Weapon.Gauntlets");

UE_DEFINE_GAMEPLAY_TAG(Item_Consumable_HealthPotion, "Item.Consumable.HealthPotion");

// ==== Equipment Tags ==== //
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Helmet, "Equipment.Slot.Helmet");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Chest, "Equipment.Slot.Chest");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Belt, "Equipment.Slot.Belt");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Boots, "Equipment.Slot.Boots");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Gloves, "Equipment.Slot.Gloves");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Necklace, "Equipment.Slot.Necklace");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_RingOne, "Equipment.Slot.RingOne");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_RingTwo, "Equipment.Slot.RingTwo");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_FirstWeapon, "Equipment.Slot.FirstWeapon");
UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_SecondWeapon, "Equipment.Slot.SecondWeapon");
}