// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		AActor* CombatTarget;

	UPROPERTY(VisibleAnywhere)
		class UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
		class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EDeathPose> DeathPose;

	UFUNCTION(BlueprintCallable)
		FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
		FVector GetRotationWarpTarget();

	virtual void BeginPlay() override;

	void DisableCapsule();
	bool IsAlive();

	/** Combat */
	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;
	virtual void Attack();
	
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	
	virtual bool CanAttack();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);
	virtual void HandleDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
		void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollissionEnabled);

	UFUNCTION(BlueprintCallable)
		void SetCombatTarget(AActor* Target);

	UFUNCTION(BlueprintCallable)
		virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
		virtual void DodgeEnd();

	/** Play Montages*/
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayDodgeMontage();
	void PlayHitReactMontage(const FName& SectionName);
	void StopAttackMontage();

	void DisableMeshCollision();

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
		class USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
		class UParticleSystem* HitParticles;

	/** Animation Montages */

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* DodgeMontage;

	int32 PlayRandomMontageSection(UAnimMontage* Montage);

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
