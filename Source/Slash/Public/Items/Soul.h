// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AItem
{
	GENERATED_BODY()

public:
	ASoul();
	virtual void Tick(float DeltaTime) override;

	void MoveToDesiredZ(float DeltaTime);

protected:
	virtual void BeginPlay() override;
	void BoxTrace(FHitResult& BoxHit);
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	

private:
	
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls = 1;	
	
	float DesiredZ = 50.f;

public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
};
