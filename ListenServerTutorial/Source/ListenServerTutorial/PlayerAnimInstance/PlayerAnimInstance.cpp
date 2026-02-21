// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance/PlayerAnimInstance.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerAnimInstance::UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer)
{
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Speed = 0.f;
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(!PlayerCharacter || !PlayerCharacterMovement)
	{
		return;
	}
	Speed = PlayerCharacterMovement->Velocity.Size();
	IsSprinting = PlayerCharacter->GetIsSprinting();
	IsFalling = PlayerCharacterMovement->IsFalling();
	bHasInput = PlayerCharacter->GetIsMoveInputPressed();
}
