

#include "Character/OpenWorldCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "HUD/OpenWorldHUD.h"
#include "HUD/EcoOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

//Enhance Input heder files.
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AOpenWorldCharacter::AOpenWorldCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 350.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrow = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrow"));
	Eyebrow->SetupAttachment(GetMesh());
	Eyebrow->AttachmentName = FString("head");

}

void AOpenWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Move Action.
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::MoveForward);

		//Look Action.
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::LookInput);

		//Jump Action.
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Jump);

		//EKeyPressed Action.
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Started, this, &AOpenWorldCharacter::EKeyPressed);

		//Attack Action.
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Attack);

		//Dodge Action.
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AOpenWorldCharacter::Dodge);

	}

}

void AOpenWorldCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void AOpenWorldCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnable(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

float AOpenWorldCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void AOpenWorldCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AOpenWorldCharacter::AddSoul(ASoul* Soul)
{
	if (Attributes && EcoOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		EcoOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AOpenWorldCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && EcoOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		EcoOverlay->SetGold(Attributes->GetGold());
	}
}

void AOpenWorldCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Player"));

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	InitializePlayerController(PlayerController);
	InitializeEcoOverlay(PlayerController);

}

void AOpenWorldCharacter::Tick(float DeltaTime)
{
	if (IsDead()) return;

	if (Attributes && EcoOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		Attributes->RegenHealth(DeltaTime);
		EcoOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		EcoOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AOpenWorldCharacter::MoveForward(const FInputActionValue& Value)
{

	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (GetController())
	{

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

}

void AOpenWorldCharacter::LookInput(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}

}

void AOpenWorldCharacter::EKeyPressed()
{
	AWeapon* OverlappingWepon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWepon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWepon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void AOpenWorldCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AOpenWorldCharacter::Dodge()
{
	if (!IsUnoccupied() || !HasEnoughStamina()) return;
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && EcoOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		EcoOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AOpenWorldCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandWepon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void AOpenWorldCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AOpenWorldCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

bool AOpenWorldCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AOpenWorldCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AOpenWorldCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

bool AOpenWorldCharacter::IsDead()
{
	return ActionState == EActionState::EAS_Dead;
}

void AOpenWorldCharacter::Disarm()
{
	PlayEquipeMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AOpenWorldCharacter::Arm()
{
	PlayEquipeMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandWepon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AOpenWorldCharacter::PlayEquipeMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AOpenWorldCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void AOpenWorldCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpinSocket"));
	}
}

void AOpenWorldCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AOpenWorldCharacter::FinshEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AOpenWorldCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AOpenWorldCharacter::InitializePlayerController(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}
	}
}

void AOpenWorldCharacter::InitializeEcoOverlay(APlayerController* PlayerController)
{
	if (PlayerController)
	{
		AOpenWorldHUD* OpenWorldHUD = Cast<AOpenWorldHUD>(PlayerController->GetHUD());
		if (OpenWorldHUD)
		{
			EcoOverlay = OpenWorldHUD->GetEcoOverlay();
			if (EcoOverlay && Attributes)
			{
				EcoOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				EcoOverlay->SetStaminaBarPercent(1.f);
				EcoOverlay->SetGold(0);
				EcoOverlay->SetSouls(0);
			}
		}
	}
}

bool AOpenWorldCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool AOpenWorldCharacter::HasEnoughStamina()
{
	return Attributes &&
		Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void AOpenWorldCharacter::SetHUDHealth()
{
	if (EcoOverlay && Attributes)
	{
		EcoOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}
