#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ReclaimQuestData.generated.h"


// Acciones que pueden completar un objetivo.
UENUM(BlueprintType)
enum class ETipoObjetivoQuest : uint8
{
	LlegarZona UMETA(DisplayName = "Llegar a una zona"),
	LeerNota UMETA(DisplayName = "Leer una nota"),
	Recolectar UMETA(DisplayName = "Recolectar"),
	Interactuar UMETA(DisplayName = "Interactuar"),
	EliminarEnemigo UMETA(DisplayName = "Eliminar enemigo"),
	AlcanzarNivel UMETA(DisplayName = "Alcanzar nivel")
};


// Configuración de un objetivo.
USTRUCT(BlueprintType)
struct FObjetivoQuest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objetivo")
	FName IdObjetivo = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objetivo")
	FText Descripcion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objetivo")
	ETipoObjetivoQuest Tipo = ETipoObjetivoQuest::LlegarZona;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Objetivo",
		meta = (ClampMin = "1", UIMin = "1")
	)
	int32 CantidadRequerida = 1;
};


// Datos configurables de una misión.
UCLASS(BlueprintType)
class PRACRICACPP_API UReclaimQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mision")
	FName IdMision = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mision")
	FText NombreMision;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Mision",
		meta = (MultiLine = "true")
	)
	FText DescripcionMision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mision")
	TArray<FObjetivoQuest> Objetivos;
};

