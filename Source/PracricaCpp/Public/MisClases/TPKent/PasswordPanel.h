#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "Components/BoxComponent.h"
#include "PasswordPanel.generated.h"

UCLASS()
class PRACRICACPP_API APasswordPanel : public AActor, public IIPuzzleInteractable
{
    GENERATED_BODY()

public:
    APasswordPanel();

    // ============================================================
    // COMPONENTES
    // ============================================================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
    UStaticMeshComponent* Mesh;

    // Box grande de overlap: el jugador se acerca y aparece el prompt
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Componentes")
    UBoxComponent* OverlapBox;

    // ============================================================
    // DATOS CONFIGURABLES
    // ============================================================
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    FPuzzleObjectData PuzzleData;

    // La contraseña correcta, se configura en el editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    FString CorrectPassword;

    // Referencia a la puerta que este panel controla
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    AActor* LinkedDoor;

    // ============================================================
    // ESTADO
    // ============================================================

    // True cuando el jugador esta en rango del panel
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    bool bPlayerInRange = false;

    // True cuando el puzzle esta activo (la pila fue insertada)
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    bool bIsActive = false;

    // ============================================================
    // INTERFACE
    // ============================================================
    virtual void OnInteract_Implementation(AActor* Interactor) override;
    virtual FPuzzleObjectData GetPuzzleData_Implementation() override;
    virtual bool CanInteract_Implementation() override;

    // Llamado desde el widget cuando el jugador ingresa la contraseña
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void TryPassword(FString EnteredPassword, AActor* Interactor);

    // ============================================================
    // OVERLAP
    // ============================================================
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};