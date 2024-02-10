
#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterType.h"
#include "Interfaces/PickupInterface.h"
#include "OpenWorldCharacter.generated.h"

//Forward declaration classes.
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class UAnimMontage;
class AItem;
class ASoul;
class ATreasure;
class UEcoOverlay;

UCLASS()
class OPENLAND_API AOpenWorldCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AOpenWorldCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	/* <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/* Pickup Interface */
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSoul(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:

	/* <AActor> */
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	/* Callbacks for Input */
	void MoveForward(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void EKeyPressed();
	virtual void Attack() override;
	void Dodge();

	/* Combat */
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	bool IsDead();
	void Disarm();
	void Arm();
	void PlayEquipeMontage(const FName& SectionName);
	virtual void Die_Implementation() override;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinshEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();


	/* Input Component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EKeyAction;

	/* Combat */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DodgeAction;

private:

	/* Combat */
	void InitializePlayerController(APlayerController* PlayerController);
	void InitializeEcoOverlay(APlayerController* PlayerController);
	bool IsUnoccupied();
	bool HasEnoughStamina();
	void SetHUDHealth();

	/* Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrow;

	/* Input Component */
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	/* Combat */
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	UEcoOverlay* EcoOverlay;


public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
