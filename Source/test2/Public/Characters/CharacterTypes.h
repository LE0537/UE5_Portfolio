#pragma once

UENUM(BlueprintType)	// BlueprintType : 블루프린트에 enum을 보이게 하는 매크로, BlueprintType
enum class ECharacterState : uint8	// enum 옆에 uint8을 붙여서 사이즈를 절약할 수 있음
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),	// UMETA(DisplayName = ""): 블루프린트에 enum 항목으로 표시할 이름을 적을 수 있음
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon")
};