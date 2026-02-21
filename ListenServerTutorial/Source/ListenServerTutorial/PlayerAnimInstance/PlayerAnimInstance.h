// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LISTENSERVERTUTORIAL_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPlayerAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	TObjectPtr<class APlayerCharacter> PlayerCharacter;
	TObjectPtr<class UCharacterMovementComponent> PlayerCharacterMovement;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool IsSprinting;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool IsFalling;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bHasInput;
};
