// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

/**
 *
 */
UCLASS()
class OPENLAND_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;
		
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:

	void DisableSphereCollision();
	void PlayEquipSound();
	void DeactivateEmbers();
	void BoxTrace(FHitResult& BoxHit);
	void ExecuteGetHit(FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor);

	UPROPERTY(EditAnywhere, Category = "Weapon Propertys")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Propertys")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Propertys")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Propertys")
	class UBoxComponent* WeaponBox;

	UPROPERTY(EditAnywhere, Category = "Weapon Propertys")
	float Damage = 20.f;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

public:

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

};
