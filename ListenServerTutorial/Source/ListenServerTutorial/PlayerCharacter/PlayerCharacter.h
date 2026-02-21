// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class LISTENSERVERTUTORIAL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class UCameraComponent> PlayerCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;
	FTimerHandle LandMovementModeTimerHandle;
	UPROPERTY(ReplicatedUsing = OnRep_SetSprint)
	bool IsSprinting = false;
	UPROPERTY(Replicated)
	bool IsMoveInputPressed = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> JumpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> HitbyMontage;
	TObjectPtr<AActor> InteractObject;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> InteractObjectMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void SetLandMovementMode();
	UFUNCTION(Server,Reliable)
	void SetSprint(bool bIsSprinting);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMaxWalkSpeed(float NewMaxWalkSpeed);
	UFUNCTION()
	void OnRep_SetSprint();
	bool GetIsSprinting() const { return IsSprinting; }
	UFUNCTION(Server, Reliable)
	void SetIsMoveInputPressed(bool bIsPressed);
	bool GetIsMoveInputPressed() const { return IsMoveInputPressed; }
	virtual void Jump() override;
	UFUNCTION(Server, Reliable)
	void Server_Attack();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAttackMontage();
	void HitCheck();
	UFUNCTION(Server, Reliable)
	void Server_Hit();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayHitEffect();

	void FindInteractionObject();
	UFUNCTION(Server, Reliable)
	void Server_Interact();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Interact(UStaticMesh* MeshAsset, const TArray<UMaterialInterface*>& Materials);

};
