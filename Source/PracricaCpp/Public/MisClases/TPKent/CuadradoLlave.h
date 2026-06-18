// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StructData.h"  // <-- Agregá este include
#include "CuadradoLlave.generated.h"

UCLASS()
class PRACRICACPP_API ACuadradoLlave : public AActor
{
	GENERATED_BODY()
    
public: 
	ACuadradoLlave();

protected:
	virtual void BeginPlay() override;

public: 
	virtual void Tick(float DeltaTime) override;

	// Variable del struct
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	FStructData MyData;
};