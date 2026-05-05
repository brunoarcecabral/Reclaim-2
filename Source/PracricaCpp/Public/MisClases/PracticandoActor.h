// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PracticandoActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class PRACRICACPP_API APracticandoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APracticandoActor();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int salud = 200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> SphereCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SphereMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> HolaComoVa;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	

};
