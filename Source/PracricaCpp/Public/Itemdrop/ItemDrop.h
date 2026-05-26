// Fill out your copyright notice in the Description page of Project Settings.
#include "Itemdrop/ItemDrop.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Sets default values
AItemDrop::AItemDrop()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemDrop::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemDrop::Destroyed()
{
	Super::Destroyed();

	if (ItemClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			World->SpawnActor<AActor>(ItemClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	}
}

