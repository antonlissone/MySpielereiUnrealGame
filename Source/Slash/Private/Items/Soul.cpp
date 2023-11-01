// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NiagaraFunctionLibrary.h"

ASoul::ASoul()
{	
	PrimaryActorTick.bCanEverTick = true;
}

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveToDesiredZ(DeltaTime);
}

void ASoul::MoveToDesiredZ(float DeltaTime)
{
	if (GetActorLocation().Z > DesiredZ)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, -15.f * DeltaTime));
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();
	FHitResult BoxHit;
	BoxTrace(BoxHit);
	DesiredZ = BoxHit.ImpactPoint.Z + 50.f;

}

void ASoul::BoxTrace(FHitResult& BoxHit)
{
	const FVector StartLocation = GetActorLocation();
	const FVector EndLocation = StartLocation - FVector(0.f, 0.f, 2000.f);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(TEnumAsByte<EObjectTypeQuery>::EnumType::ObjectTypeQuery1); // World Static

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		StartLocation,
		EndLocation,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
	);
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->AddSouls(this);
	}
	Destroy();
}
