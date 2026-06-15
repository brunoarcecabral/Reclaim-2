#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PuzzlePasswordWidget.generated.h"

UCLASS()
class PRACRICACPP_API UPuzzlePasswordWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// ============================================================
	// REFERENCIA AL PANEL QUE ABRIO ESTE WIDGET
	// Lo guardamos para poder llamar TryPassword despues
	// ============================================================
	UPROPERTY(BlueprintReadWrite, Category="Password")
	AActor* OwningPanel = nullptr;

	// ============================================================
	// FUNCIONES
	// ============================================================

	// Llamado desde el boton confirmar del widget
	// Toma el texto ingresado y se lo manda al panel
	UFUNCTION(BlueprintCallable, Category="Password")
	void SubmitPassword(const FString& EnteredPassword, AActor* Interactor);

	// ============================================================
	// EVENTOS PARA BLUEPRINT
	// ============================================================

	// Llamado cuando la contraseña es incorrecta, para mostrar feedback
	UFUNCTION(BlueprintImplementableEvent, Category="Password")
	void OnPasswordWrong();

	// Llamado cuando la contraseña es correcta, para cerrar el widget
	UFUNCTION(BlueprintImplementableEvent, Category="Password")
	void OnPasswordCorrect();
};