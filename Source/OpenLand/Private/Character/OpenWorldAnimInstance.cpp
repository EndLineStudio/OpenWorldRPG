// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OpenWorldAnimInstance.h"
#include "Character/OpenWorldCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UOpenWorldAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OpenWorldCharacter = Cast<AOpenWorldCharacter>(TryGetPawnOwner());

	if (OpenWorldCharacter)
	{
		CharacterMovement = OpenWorldCharacter->GetCharacterMovement();
	}

}

void UOpenWorldAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		IsFalling = CharacterMovement->IsFalling();
		CharacterState = OpenWorldCharacter->GetCharacterState();
		ActionState = OpenWorldCharacter->GetActionState();
		DeathPose = OpenWorldCharacter->GetDeathPos();
	}
}
