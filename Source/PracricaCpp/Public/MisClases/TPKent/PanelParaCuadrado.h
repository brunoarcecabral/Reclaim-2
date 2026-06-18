#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MisClases/TPKent/ComunicacionPuzzle.h"
#include "Blueprint/UserWidget.h"
#include "PanelParaCuadrado.generated.h"

UCLASS()
class PRACRICACPP_API APanelParaCuadrado : public AActor, public IComunicacionPuzzle
{
	GENERATED_BODY()

public:
	APanelParaCuadrado();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Implementacion de la interfaz
	virtual void ComunicarPuzzle_Implementation(bool llave) override;

	// Variable para controlar si es la primera vez
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	bool PrimeraVe = true;

	// Clase del actor a spawnear
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	TSubclassOf<AActor> Clas;

	// Ubicacion donde spawnear
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	FVector SpawnTransformLocatio = FVector::ZeroVector;

	// Clase del widget a mostrar
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Puzzle")
	TSubclassOf<UUserWidget> WidgetClas;
};