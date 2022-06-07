// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE2_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere)
	float Reach = 300.f;

	UPROPERTY(EditAnywhere)
	float HoldDistance = 200.f;

	UPROPERTY(EditAnywhere)
	float HoldHeightAdjustment = 80.f;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	UPROPERTY()
	UInputComponent* InputComponent = nullptr;

	void Grab();
	void Release();
	void FindPhysicsHandle();
	void SetupInputComponent();

	// Return first actor within reach with a physics body
	FHitResult GetFirstPhysicsBodyInReach() const;

	// Return the reaching Line Trace End
	FVector GetPlayersReach() const;

	// Return the holding Line Trace End
	FVector GetPlayersHoldingDistance() const;

	// Get player's position in the world
	FVector GetPlayersWorldPosition() const;		
};
