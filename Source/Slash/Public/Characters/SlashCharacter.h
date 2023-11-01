// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Characters/BaseCharacter.h"
#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	
	/** IPickupInterface */
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	
	/** IHitInterface */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	/** AActor */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		class UTargetSystemComponent* TargetSystem;

	/** Movement Stuff */
	UPROPERTY(EditAnywhere, Category = Input)
		class UInputMappingContext* CharacterMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
		class UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* TargetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* SwitchEquipAction;

	/** Callbacks for Inputs */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void Target(const FInputActionValue& Value);
	void DoAttack(const FInputActionValue& Value);
	virtual void Attack() override;

	void EquipWeapon(AWeapon* OverlappingWeapon);
	void SwitchEquip(const FInputActionValue& Value);

	/** Combat */
	void PlaySwitchEquipMontage(FName SectionName);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;

	UFUNCTION(BlueprintCallable)
	void SwitchEquipEnd();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		UAnimMontage* SwitchEquipMontage;
	
	virtual bool CanAttack() override;
	virtual void Die_Implementation() override;

	bool CanArm();
	bool CanDisarm();
	void Disarm();
	void Arm();

private:

	/** Character components */
	UPROPERTY(VisibleAnywhere)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
		class UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere)
		UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
		class AItem* OverlappingItem;

	

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BluePrintReadWrite, meta = (AllowPrivateAccess = "true"))
		EActionState ActionState = EActionState::EAS_Unoccupied;

	UFUNCTION(BlueprintCallable)
		void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
		void AttachWeaponToArm();

	UPROPERTY()
	class USlashOverlay* SlashOverlay;

	bool IsUnoccupied();
	
	void InitializeOverlay();
	void SetHUDHealth();
	

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
