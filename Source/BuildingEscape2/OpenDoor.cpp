// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;
	TargetClosedYaw += InitialYaw;

	TimeLastOpened = GetWorld()->GetTimeSeconds();

	CheckPressurePlateExists();
	FindAudioComponent();
}

void UOpenDoor::CheckPressurePlateExists() const
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("OpenDoor assigned but no pressure plate detected for %s"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no Audio Component attached"), *GetOwner()->GetName());
	}
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float TotalMassOfActors = GetTotalMassOfActors();

	// UE_LOG(LogTemp, Error, TEXT("Opening Door with mass %i"), TotalMassOfActors);
	// UE_LOG(LogTemp, Error, TEXT("Total Opening Door Mass %i"), TotalMassToTrigger);

	if (TotalMassOfActors > TotalMassToTrigger)
	{
		OpenDoor(DeltaTime);

		// Set time last opened
		TimeLastOpened = GetWorld()->GetTimeSeconds();
	}
	else if (PressurePlate && DoorCloseDelay < GetWorld()->GetTimeSeconds() - TimeLastOpened)
	{
		CloseDoor(DeltaTime);
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	if (!AudioComponent) { return; }

	CloseDoorSoundUsed = false;

	if (!OpenDoorSoundUsed)
	{
		AudioComponent->Play();
		OpenDoorSoundUsed = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, TargetClosedYaw, DeltaTime, DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	if (!AudioComponent) { return; }

	OpenDoorSoundUsed = false;
	if (!CloseDoorSoundUsed)
	{
		AudioComponent->Play();
		CloseDoorSoundUsed = true;
	}
}

float UOpenDoor::GetTotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find all overlapping actors
	OUT TArray<AActor*> OverlappingActors;

	if (!PressurePlate) { return TotalMass; }
	
	PressurePlate->GetOverlappingActors(OverlappingActors);

	// Calculate total mass
	for (AActor* Actor: OverlappingActors)
	{
		if (!Actor) { continue; }

		UPrimitiveComponent* Component = Actor->FindComponentByClass<UPrimitiveComponent>();

		// FString Description = Component->GetDesc();
		// FString FullName = Component->GetFullName();
		// float CalculatedMass = Component->CalculateMass();
		// float MassScale = Component->GetMassScale();

		// float Mass = Component->GetMass();

		FString Name = Actor->GetName();

		if (Name.Contains("DefaultPawn_Blueprint"))
		{
			TotalMass += 80;
		}
		else
		{
			TotalMass += 25;
		}

		// UE_LOG(LogTemp, Error, TEXT("Adding Actor's Mass: %i"), Mass);
		// UE_LOG(LogTemp, Error, TEXT("Adding Actor's Name: %s"), *Name);
		// UE_LOG(LogTemp, Error, TEXT("Adding Actor's Description: %s"), *Description);
		// UE_LOG(LogTemp, Error, TEXT("Adding Actor's Full name: %s"), *FullName);
		// UE_LOG(LogTemp, Error, TEXT("Adding Actor's Calculated Mass: %i"), CalculatedMass);
		// UE_LOG(LogTemp, Error, TEXT("Adding Actor's Mass Scale: %i"), MassScale);
		// TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	// UE_LOG(LogTemp, Error, TEXT("Total Mass of items: %i"), TotalMass);

	return TotalMass;
}
