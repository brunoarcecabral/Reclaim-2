#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ReclaimNotaQuestData.generated.h"


class UReclaimQuestData;


// Información configurable de una nota de misión.
UCLASS(BlueprintType)
class PRACRICACPP_API UReclaimNotaQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Identificador para distinguir esta nota de otras.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nota")
	FName IdNota = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nota")
	FText Titulo;

	// Texto que se mostrará en la ventana.
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Nota",
		meta = (MultiLine = "true")
	)
	FText Contenido;

	// Misión y objetivo que se completan al llegar al lugar.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdMisionActual = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdObjetivoLlegada = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdObjetivoLectura = NAME_None;

	// Misión que se iniciará cuando un jugador pulse E.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UReclaimQuestData> MisionSiguiente = nullptr;
};

