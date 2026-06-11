#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "Components/BoxComponent.h"
#include "AColorPanel.generated.h"

UCLASS()
class PRACRICACPP_API AAColorPanel : public AActor, public IIPuzzleInteractable
{
    GENERATED_BODY()

public:
    AAColorPanel();

    // ============================================================
    // COMPONENTES
    // ============================================================

    // Mesh del cuadrado de color en el suelo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
    UStaticMeshComponent* Mesh;

    // Box de overlap: detecta cuando el jugador lo pisa
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
    UBoxComponent* OverlapBox;

    // ============================================================
    // DATOS CONFIGURABLES DESDE EL EDITOR
    // Cada instancia del tile tiene su propio color e inicial
    // ============================================================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    FPuzzleObjectData PuzzleData;

    // ============================================================
    // ESTADO
    // ============================================================

    // True cuando el puzzle esta activo (la pila fue insertada)
    // Si no esta activo, pisar el tile no hace nada
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    bool bIsActive = false;

    // ============================================================
    // INTERFACE
    // ============================================================
    virtual void OnInteract_Implementation(AActor* Interactor) override;
    virtual FPuzzleObjectData GetPuzzleData_Implementation() override;
    virtual bool CanInteract_Implementation() override;

    // ============================================================
    // OVERLAP
    // ============================================================
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};