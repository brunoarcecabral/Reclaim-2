#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MisClases/TPKent/IPuzzleInteractable.h" // Necesitamos el struct y la interface
#include "PuzzleManagerComponent.generated.h"

// ============================================================
// DELEGATES: notifican cuando ocurren eventos importantes
// Otros sistemas pueden "escuchar" estos eventos y reaccionar
// ============================================================

// Se dispara cuando el jugador detecta un objeto interactuable
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectDetected, FPuzzleObjectData, ObjectData);

// Se dispara cuando un objeto es completado y se guarda en el codice
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectCompleted, FPuzzleObjectData, ObjectData);

// Se dispara cuando todo el puzzle es completado
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleCompleted);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRACRICACPP_API UPuzzleManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPuzzleManagerComponent();

    // ============================================================
    // CONFIGURACION
    // ============================================================

    // Distancia maxima a la que el jugador puede interactuar
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    float InteractDistance = 300.f;

    // ============================================================
    // DELEGATES (eventos)
    // ============================================================

    UPROPERTY(BlueprintAssignable, Category="Puzzle|Eventos")
    FOnObjectDetected OnObjectDetected;

    UPROPERTY(BlueprintAssignable, Category="Puzzle|Eventos")
    FOnObjectCompleted OnObjectCompleted;

    UPROPERTY(BlueprintAssignable, Category="Puzzle|Eventos")
    FOnPuzzleCompleted OnPuzzleCompleted;

    // ============================================================
    // CODICE: registro de objetos completados
    // ============================================================

    UPROPERTY(BlueprintReadOnly, Category="Puzzle|Codice")
    TArray<FPuzzleObjectData> CompletedObjects;

    // ============================================================
    // ESTADO DE LA PILA
    // El jugador puede estar cargando una pila en un momento dado
    // ============================================================

    // True si el jugador tiene una pila en la mano
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    bool bIsHoldingBattery = false;

    // Referencia a la pila que esta cargando
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    AActor* HeldBattery = nullptr;
    
    // True cuando el puzzle esta activo (la pila fue insertada)
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    bool bPuzzleActive = false;

    // ============================================================
    // FUNCIONES PUBLICAS
    // ============================================================

    // Intenta interactuar con el objeto que el jugador esta mirando
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void TryInteract();

    // Hace el LineTrace y devuelve el actor que el jugador esta mirando
    // Devuelve nullptr si no hay nada en rango
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    AActor* GetLookedAtActor();

    // Registra un objeto en el codice y dispara el delegate
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void RegisterCompletedObject(FPuzzleObjectData ObjectData);
    
    // Secuencia correcta de colores, se configura en el editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Puzzle")
    TArray<FString> CorrectSequence;

    // Secuencia que el jugador fue pisando
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    TArray<FString> CurrentSequence;

    // Llamado por cada tile cuando el jugador lo pisa
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void OnTileStep(FPuzzleObjectData TileData);
    
    // Llamado cuando el jugador entra al overlap del panel
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void OnPasswordPanelEnter(AActor* Panel);

    // Llamado cuando el jugador sale del overlap del panel
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void OnPasswordPanelExit();

    // Abre el widget de contrasena
    UFUNCTION(BlueprintCallable, Category="Puzzle")
    void OpenPasswordWidget(AActor* Panel);

    // Panel activo actualmente
    UPROPERTY(BlueprintReadOnly, Category="Puzzle")
    AActor* ActivePasswordPanel = nullptr;

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;
};