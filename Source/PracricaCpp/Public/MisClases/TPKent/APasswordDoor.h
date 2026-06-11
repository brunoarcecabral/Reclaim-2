#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "APasswordDoor.generated.h"

UCLASS()
class PRACRICACPP_API AAPasswordDoor : public AActor, public IIPuzzleInteractable
{
	GENERATED_BODY()

public:
	AAPasswordDoor();

	// ============================================================
	// COMPONENTES
	// ============================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
	UStaticMeshComponent* Mesh;

	// ============================================================
	// DATOS CONFIGURABLES
	// ============================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	FPuzzleObjectData PuzzleData;

	// Hacia donde se mueve la puerta al abrirse (configurable en el editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
	FVector OpenOffset = FVector(0.f, 0.f, 300.f);

	// ============================================================
	// ESTADO
	// ============================================================
	UPROPERTY(BlueprintReadOnly, Category="Puzzle")
	bool bIsOpen = false;

	// ============================================================
	// INTERFACE
	// ============================================================
	virtual void OnInteract_Implementation(AActor* Interactor) override;
	virtual FPuzzleObjectData GetPuzzleData_Implementation() override;
	virtual bool CanInteract_Implementation() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};