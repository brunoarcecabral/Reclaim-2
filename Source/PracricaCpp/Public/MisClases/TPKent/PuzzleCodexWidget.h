#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MisClases/TPKent/IPuzzleInteractable.h"
#include "PuzzleCodexWidget.generated.h"

UCLASS()
class PRACRICACPP_API UPuzzleCodexWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // ============================================================
    // HISTORIAL DE COLORES PISADOS
    // Cada entrada es una linea del log (ej: "Rojo - R")
    // ============================================================
    UPROPERTY(BlueprintReadOnly, Category="Codice")
    TArray<FString> ColorHistory;

    // ============================================================
    // ESTADO
    // ============================================================

    // Cuando es true el historial queda fijo en pantalla para siempre
    UPROPERTY(BlueprintReadWrite, Category="Codice")
    bool bIsPermanent = false;

    // ============================================================
    // FUNCIONES LLAMADAS DESDE C++ O BLUEPRINT
    // ============================================================

    // Agrega una entrada al historial (llamado cada vez que se pisa un tile)
    UFUNCTION(BlueprintCallable, Category="Codice")
    void AddColorEntry(FPuzzleObjectData TileData);

    // Hace el historial permanente (llamado al completar el puzzle)
    UFUNCTION(BlueprintCallable, Category="Codice")
    void MakePermanent();

    // ============================================================
    // EVENTOS PARA IMPLEMENTAR EN BLUEPRINT
    // Con BlueprintImplementableEvent le decimos a Unreal que
    // la logica visual de este evento la haremos en el Blueprint
    // ============================================================

    // Llamado cuando se agrega una entrada, para actualizar el texto en pantalla
    UFUNCTION(BlueprintImplementableEvent, Category="Codice")
    void OnColorAdded(const FString& Entry);

    // Llamado cuando el puzzle se completa
    UFUNCTION(BlueprintImplementableEvent, Category="Codice")
    void OnBecomePermanent();
};