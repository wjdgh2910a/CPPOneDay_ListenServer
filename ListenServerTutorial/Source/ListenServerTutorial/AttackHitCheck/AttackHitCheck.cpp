// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHitCheck/AttackHitCheck.h"
#include "PlayerCharacter/PlayerCharacter.h"

void UAttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			Player->HitCheck();
		}
	}
}
