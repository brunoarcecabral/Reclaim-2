#include "MisClases/TPKent/PuzzlePasswordWidget.h"
#include "MisClases/TPKent/PasswordPanel.h"

void UPuzzlePasswordWidget::SubmitPassword(const FString& EnteredPassword, AActor* Interactor)
{
	if (!OwningPanel) return;

	// Casteamos al panel concreto porque es nuestra propia clase
	// y necesitamos llamar TryPassword directamente
	APasswordPanel* Panel = Cast<APasswordPanel>(OwningPanel);
	if (!Panel) return;

	// Verificamos la contraseña
	if (EnteredPassword == Panel->CorrectPassword)
	{
		Panel->TryPassword(EnteredPassword, Interactor);
		OnPasswordCorrect();
	}
	else
	{
		OnPasswordWrong();
	}
}