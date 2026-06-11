#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPuzzleInteractable.generated.h"

// ============================================================
// ENUM: tipos de objetos que existen en el puzzle
// ============================================================
UENUM(BlueprintType)
enum class EPuzzleObjectType : uint8
{
    Battery      UMETA(DisplayName = "Pila"),
    BatterySlot  UMETA(DisplayName = "Receptaculo"),
    ColorButton  UMETA(DisplayName = "Boton de Color"),
    ColorPanel   UMETA(DisplayName = "Panel de Colores"),
    PasswordDoor UMETA(DisplayName = "Puerta")
};

// ============================================================
// STRUCT: datos configurables de cada objeto del puzzle
// Cada actor del mundo va a tener uno de estos con su info
// ============================================================
USTRUCT(BlueprintType)
struct FPuzzleObjectData
{
    GENERATED_BODY()

    // Nombre del objeto (ej: "Pila Principal")
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ObjectName;

    // Tipo de objeto segun el enum de arriba
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPuzzleObjectType ObjectType;

    // Descripcion opcional para el codice
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    // Se marca true cuando el objeto fue completado
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsCompleted = false;

    // Color del objeto (usado por los botones del tablero)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor ObjectColor = FLinearColor::White;

    // Inicial del color (ej: "R" para rojo, usado en la contraseña)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ColorInitial;
};

// Esta clase no se modifica, es requerida por Unreal para la interface
UINTERFACE(MinimalAPI)
class UIPuzzleInteractable : public UInterface
{
    GENERATED_BODY()
};

// ============================================================
// INTERFACE: contrato que deben cumplir todos los objetos
// del puzzle. Evita que el jugador necesite conocer la clase
// concreta de cada objeto (sin casteos directos)
// ============================================================
class PRACRICACPP_API IIPuzzleInteractable
{
    GENERATED_BODY()

public:
    // Llamado cuando el jugador interactua con el objeto
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnInteract(AActor* Interactor);

    // Devuelve los datos del objeto (el struct de arriba)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FPuzzleObjectData GetPuzzleData();

    // Indica si el objeto puede ser interactuado en este momento
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool CanInteract();
};