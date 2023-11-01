// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseCharacter.h"
#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	UPROPERTY(EditAnywhere, Category = Combat)
		float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
		float AttackMax = 1.f;

	/** AActor */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	/** IHitInterface */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	/** AI Behaviour */
	void HideHealthBar();
	void ShowHealthBar();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	void LooseInterest();
	void StartPatrolling();
	void StartChasing();
	bool IsPatrolling();
	bool IsDead();
	bool IsInsideAttackRadius();
	bool IsAttacking();
	bool IsChasing();
	bool IsEngaged();

	/** Combat */
	void StartAttackTimer();
	void ClearAttackTimer();
	void SetTargetSystem(class UTargetSystemComponent* LockedOnByTargetSystem);

	FTimerHandle AttackTimer;

protected:
	UPROPERTY(VisibleAnywhere)
		EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere, Category = Combat)
		float DeathLifeSpan = 8.f;

	/** AActor */
	virtual void BeginPlay() override;

	/** BaseCharacter */
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void Die_Implementation() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;

	UFUNCTION()
		void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

private:
	void InitializeEnemy();
	void SpawnDefaultWeapon();
	/** AI Behaviour */
	void CheckPatrolTarget();
	void CheckCombatTarget();

	/** Combat */
	UPROPERTY(VisibleAnywhere)
		class UHealthBarComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(VisibleAnywhere)
		class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		double CombatRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		double AttackRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		double AcceptanceRadius = 50.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class ASoul> SoulClass;

	UPROPERTY(VisibleAnywhere)
		class UTargetSystemComponent* TargetSystem;

	void SpawnSoulsOnDeath();

	/**
	* Navigation
	*/
	UPROPERTY()
		class AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float MaxWalkSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float MaxRunSpeed = 300.f;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* PickPatrolTarget();

	void ClearPatrolTimer();
	
	
};
