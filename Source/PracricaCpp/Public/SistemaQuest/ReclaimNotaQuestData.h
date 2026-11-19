#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ReclaimNotaQuestData.generated.h"


class UReclaimQuestData;


UCLASS(BlueprintType)
class PRACRICACPP_API UReclaimNotaQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nota")
	FName IdNota = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nota")
	FText Titulo;
	
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Nota",
		meta = (MultiLine = "true")
	)
	FText Contenido;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdMisionActual = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdObjetivoLlegada = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName IdObjetivoLectura = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UReclaimQuestData> MisionSiguiente = nullptr;
};

