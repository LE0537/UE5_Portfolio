#pragma once

UENUM(BlueprintType)	// BlueprintType : �������Ʈ�� enum�� ���̰� �ϴ� ��ũ��, BlueprintType
enum class ECharacterState : uint8	// enum ���� uint8�� �ٿ��� ����� ������ �� ����
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),	// UMETA(DisplayName = ""): �������Ʈ�� enum �׸����� ǥ���� �̸��� ���� �� ����
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