// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructData.generated.h"

USTRUCT(BlueprintType)
struct PRACRICACPP_API FStructData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	FString MyString = TEXT("");

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	FVector MyVector = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	bool bMyBool = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StructData")
	float MyFloat = 0.0f;

	// Constructor por defecto (opcional pero buena práctica)
	FStructData()
		: MyString(TEXT(""))
		, MyVector(FVector::ZeroVector)
		, bMyBool(false)
		, MyFloat(0.0f)
	{}
};