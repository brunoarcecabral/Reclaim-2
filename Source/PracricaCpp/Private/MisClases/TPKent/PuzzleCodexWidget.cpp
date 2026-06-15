#include "MisClases/TPKent/PuzzleCodexWidget.h"

void UPuzzleCodexWidget::AddColorEntry(FPuzzleObjectData TileData)
{
	// Armamos la entrada del log: "Rojo - R"
	FString Entry = TileData.ObjectName + " - " + TileData.ColorInitial;

	// La agregamos al historial
	ColorHistory.Add(Entry);

	// Notificamos al Blueprint para que actualice el texto en pantalla
	OnColorAdded(Entry);

	UE_LOG(LogTemp, Log, TEXT("[CODICE] %s"), *Entry);
}

void UPuzzleCodexWidget::MakePermanent()
{
	bIsPermanent = true;

	// Notificamos al Blueprint para que cambie el estilo visual
	OnBecomePermanent();

	UE_LOG(LogTemp, Log, TEXT("[CODICE] Historial permanente"));
}