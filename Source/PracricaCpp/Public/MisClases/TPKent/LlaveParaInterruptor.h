#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "Components/BoxComponent.h"
#include "LlaveParaInterruptor.generated.h"

UCLASS()
class PRACRICACPP_API ALlaveParaInterruptor : public AActor, public IIPuzzleInteractable
{
    GENERATED_BODY()

public:
    ALlaveParaInterruptor();

    // Mesh visible de la pila
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
    UStaticMeshComponent* Mesh;

    // Box de overlap: cuando el jugador la toca puede agarrarla
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
    UBoxComponent* OverlapBox;

    // Datos configurables desde el editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    FPuzzleObjectData PuzzleData;

    // True cuando el jugador la esta cargando
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    bool bIsHeld = false;

    // Referencia al actor que la esta cargando
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    AActor* HoldingActor = nullptr;

    // Interface
    virtual void OnInteract_Implementation(AActor* Interactor) override;
    virtual FPuzzleObjectData GetPuzzleData_Implementation() override;
    virtual bool CanInteract_Implementation() override;

    // Overlap
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};