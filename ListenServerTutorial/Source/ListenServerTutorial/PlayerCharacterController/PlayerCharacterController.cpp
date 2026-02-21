// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterController/PlayerCharacterController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "PlayerCharacter/PlayerCharacter.h"

APlayerCharacterController::APlayerCharacterController()
{
}

void APlayerCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacterController::MoveEnd);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacterController::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacterController::SprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacterController::SprintEnd);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacterController::JumpStart);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacterController::AttackStart);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacterController::InteractStart);
	}
}
void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
}

void APlayerCharacterController::Move(const FInputActionValue& Value)
{
	FVector2D MoveAxisVector = Value.Get<FVector2D>();
	if (PlayerCharacter)
	{
		const FRotator YawRotation(0, GetControlRotation().Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		PlayerCharacter->AddMovementInput(ForwardDirection, MoveAxisVector.X);
		PlayerCharacter->AddMovementInput(RightDirection, MoveAxisVector.Y);
		PlayerCharacter->SetIsMoveInputPressed(true);
	}
}

void APlayerCharacterController::MoveEnd(const FInputActionValue& Value)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->SetIsMoveInputPressed(false);
	}
}

void APlayerCharacterController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void APlayerCharacterController::SprintStart(const FInputActionValue& Value)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->SetSprint(true);
	}
}

void APlayerCharacterController::SprintEnd(const FInputActionValue& Value)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->SetSprint(false);
	}
}

void APlayerCharacterController::JumpStart(const FInputActionValue& Value)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->Jump();
	}
}

void APlayerCharacterController::AttackStart(const FInputActionValue& Value)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->Server_Attack();
	}
}

void APlayerCharacterController::InteractStart(const FInputActionValue& Value)
{
	if(PlayerCharacter)
	{
		PlayerCharacter->Server_Interact();
	}
}
