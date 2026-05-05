// Fill out your copyright notice in the Description page of Project Settings.


#include "MisClases/PracticandoActor.h"
#include "MisClases/PracticandoActor.h"

#include "Components/SphereComponent.h"


// Sets default values
APracticandoActor::APracticandoActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	RootComponent = SphereCollision;
	
	SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
	SphereMesh->SetupAttachment(RootComponent);
	
	HolaComoVa = CreateDefaultSubobject<UStaticMeshComponent> ("HolaComoVa");
	HolaComoVa->SetupAttachment(RootComponent);
	

}

// Called when the game starts or when spawned
void APracticandoActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APracticandoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

