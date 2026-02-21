// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom);
	PlayerCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	InteractObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractObjectMesh"));
	InteractObjectMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetWorldTimerManager().SetTimer(LandMovementModeTimerHandle, this, &APlayerCharacter::SetLandMovementMode, 1.f, false);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FindInteractionObject();

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, IsSprinting);
	DOREPLIFETIME(APlayerCharacter, IsMoveInputPressed);
}

void APlayerCharacter::SetLandMovementMode()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void APlayerCharacter::SetSprint_Implementation(bool bIsSprinting)
{
	if (HasAuthority())
	{
		IsSprinting = bIsSprinting;
		//OnRep_SetSprint();
		MulticastSetMaxWalkSpeed(IsSprinting ? 600.f : 300.f);
	}
}

void APlayerCharacter::MulticastSetMaxWalkSpeed_Implementation(float NewMaxWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;
}

void APlayerCharacter::OnRep_SetSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = IsSprinting ? 600.f : 300.f;
}

void APlayerCharacter::Jump()
{
	Super::Jump();
	if (JumpMontage)
	{
		PlayAnimMontage(JumpMontage);
	}
}

void APlayerCharacter::HitCheck()
{
	if (HasAuthority())
	{
		FHitResult HitResult;
		bool isHit = UKismetSystemLibrary::SphereTraceSingle
		(this,
			GetActorLocation(),
			GetActorLocation() + GetActorForwardVector() * 150.f,
			50.f,
			UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForDuration, HitResult, true);
		if (isHit)
		{
			APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(HitResult.GetActor());
			if(HitPlayer)
				HitPlayer->Server_Hit();
		}
	}
}

void APlayerCharacter::FindInteractionObject()
{
	if(HasAuthority())
	{
		FHitResult HitResult;
		bool isHit = UKismetSystemLibrary::SphereTraceSingle
		(this,
			GetActorLocation(),
			GetActorLocation() + GetActorForwardVector() * 150.f,
			50.f,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			TArray<AActor*>(),
			EDrawDebugTrace::ForOneFrame, HitResult, true);
		if (isHit)
		{
			InteractObject = HitResult.GetActor();
		}
		else
		{
			InteractObject = nullptr;
		}
		FString Message = InteractObject ? FString::Printf(TEXT("Hit Object: %s"), *InteractObject->GetName()) : TEXT("No Object");
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, Message);
	}
}

void APlayerCharacter::Multicast_Interact_Implementation(UStaticMesh* MeshAsset, const TArray<UMaterialInterface*>& Materials)
{
	if (MeshAsset)
	{
		GetMesh()->SetHiddenInGame(true);
		InteractObjectMesh->SetStaticMesh(MeshAsset);

		for (int32 i = 0; i < Materials.Num(); ++i)
		{
			if (Materials[i])
			{
				InteractObjectMesh->SetMaterial(i, Materials[i]);
			}
		}
	}
	else
	{
		GetMesh()->SetHiddenInGame(false);
		InteractObjectMesh->SetStaticMesh(nullptr);
	}
}

void APlayerCharacter::Server_Interact_Implementation()
{
	if (HasAuthority())
	{
		if(InteractObject)
		{
			UStaticMeshComponent* StaticMeshComp = InteractObject->FindComponentByClass<UStaticMeshComponent>();

			if (StaticMeshComp)
			{
				UStaticMesh* MeshAsset = StaticMeshComp->GetStaticMesh();
				
				// [추가] 컴포넌트에 설정된 머티리얼 목록 가져오기 (오버라이드된 머티리얼 포함)
				TArray<UMaterialInterface*> MaterialList = StaticMeshComp->GetMaterials();

				if (MeshAsset)
				{
					// [수정] 메쉬와 머티리얼 목록을 함께 전송
					Multicast_Interact(MeshAsset, MaterialList);
				}
			}
		}
		else
		{
			// 해제 시에는 빈 배열 전달
			Multicast_Interact(nullptr, TArray<UMaterialInterface*>());
		}
	}
}

void APlayerCharacter::Multicast_PlayHitEffect_Implementation()
{
	if (HitbyMontage)
	{
		PlayAnimMontage(HitbyMontage);
	}
}

void APlayerCharacter::Server_Hit_Implementation()
{
	if (HasAuthority())
	{
		Multicast_PlayHitEffect();
	}
}

void APlayerCharacter::Multicast_PlayAttackMontage_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
}

void APlayerCharacter::Server_Attack_Implementation()
{
	if (HasAuthority())
	{
		Multicast_PlayAttackMontage();
	}
}

void APlayerCharacter::SetIsMoveInputPressed_Implementation(bool bIsPressed)
{
	if (HasAuthority())
	{
		IsMoveInputPressed = bIsPressed;
	}
}

