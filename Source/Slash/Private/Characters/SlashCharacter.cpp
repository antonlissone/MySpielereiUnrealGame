// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Enemy/Enemy.h"
#include "TargetSystemComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "Items/Item.h"
#include "Items/Treasure.h"
#include "Items/Weapon.h"
#include "Items/Soul.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(CameraBoom);

	TargetSystem = CreateDefaultSubobject<UTargetSystemComponent>(TEXT("TargetSystem"));
	TargetSystem->bShouldControlRotation = false;
	TargetSystem->bAdjustPitchBasedOnDistanceToTarget = true;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);


	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes)
	{
		Attributes->RegenStamina(DeltaTime);
	}
	if (SlashOverlay)
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;

}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CharacterMappingContext, 0);
		}

		
	}
	InitializeOverlay();
	Tags.Add(FName("EngageableTarget"));
}

void ASlashCharacter::InitializeOverlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetCoinCount(0);
				SlashOverlay->SetSoulsCount(0);
			}
		}
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (ActionState != EActionState::EAS_Unoccupied) return;

	// Foward
	if (GetController() && (MovementVector.Y != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), MovementVector.Y);
	}

	// Turn Right
	if (GetController() && (MovementVector.X != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), MovementVector.X);
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookValue = Value.Get<FVector2D>();

	if (GetController() && (LookValue.X + LookValue.Y != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddControllerPitchInput(LookValue.Y);
		AddControllerYawInput(LookValue.X);
	}
}

void ASlashCharacter::Equip(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			CharacterState = ECharacterState::ECS_Unequipped;
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
}

void ASlashCharacter::Dodge(const FInputActionValue& Value)
{
	const float StaminaCost = 10;
	if (!IsUnoccupied() || !Attributes->HasSufficientStamina(StaminaCost)) return;
	
	Attributes->UseStamina(StaminaCost);
	if (SlashOverlay)
	{
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodging;	
}

void ASlashCharacter::Target(const FInputActionValue& Value)
{
	if (TargetSystem)
	{
		if (TargetSystem->IsLocked())
		{
			if (AEnemy* CurrentTarget = CastChecked<AEnemy>(TargetSystem->GetLockedOnTargetActor()))
			{
				CurrentTarget->SetTargetSystem(nullptr);
			}
		}

		TargetSystem->TargetActor();
		
		if (TargetSystem->IsLocked())
		{
			CombatTarget = TargetSystem->GetLockedOnTargetActor();
			if (AEnemy* Enemy = CastChecked<AEnemy>(CombatTarget))
			{
				Enemy->SetTargetSystem(TargetSystem);
			}	
		}
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = OverlappingWeapon->GetEquippedCharacterState();
	OverlappingItem = nullptr;
	EquippedWeapon = OverlappingWeapon;
}

void ASlashCharacter::DoAttack(const FInputActionValue& Value)
{
	Attack();
}

void ASlashCharacter::Attack()
{
	if (CanAttack())
	{
		if (TargetSystem && TargetSystem->IsLocked())
		{
			FRotator LookAtEnemy = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetSystem->GetLockedOnTargetActor()->GetActorLocation());
			SetActorRotation(LookAtEnemy, ETeleportType::None);
		}
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::SwitchEquip(const FInputActionValue& Value)
{
	if (CanArm())
	{
		Arm();
	}
	else if (CanDisarm())
	{
		Disarm();
	}
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon;
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState != ECharacterState::ECS_Unequipped
		&& EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	PlaySwitchEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_Equipping;
}

void ASlashCharacter::Arm()
{
	PlaySwitchEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_Equipping;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
		CharacterState = ECharacterState::ECS_Unequipped;
	}
}

void ASlashCharacter::AttachWeaponToArm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
		CharacterState = EquippedWeapon->GetEquippedCharacterState();
	}
}

void ASlashCharacter::PlaySwitchEquipMontage(FName SectionName)
{
	PlayMontageSection(SwitchEquipMontage, SectionName);
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SwitchEquipEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::DoAttack);
		EnhancedInputComponent->BindAction(SwitchEquipAction, ETriggerEvent::Started, this, &ASlashCharacter::SwitchEquip);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ASlashCharacter::Target);
	}
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return IsAlive() && ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSoulsCount(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetCoinCount(Attributes->GetGold());
	}
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}
