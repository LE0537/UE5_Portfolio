#pragma once

UENUM(BlueprintType)	// BlueprintType : �������Ʈ�� enum�� ���̰� �ϴ� ��ũ��, BlueprintType
enum class ECharacterState : uint8	// enum ���� uint8�� �ٿ��� ����� ������ �� ����
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),	// UMETA(DisplayName = ""): �������Ʈ�� enum �׸����� ǥ���� �̸��� ���� �� ����
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};