// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComunicacionPuzzle.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UComunicacionPuzzle : public UInterface
{
	GENERATED_BODY()
};

class PRACRICACPP_API IComunicacionPuzzle
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Puzzle")
	void ComunicarPuzzle(bool llave);
};