#include "SistemaQuest/ReclaimAvisoQuestWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"


void UReclaimAvisoQuestWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WidgetTree)
	{
		return;
	}

	// Panel principal del widget.
	UCanvasPanel* Panel =
		WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("PanelAviso")
		);

	if (!Panel)
	{
		return;
	}

	WidgetTree->RootWidget = Panel;

	// Texto visible mientras el jugador espera al grupo.
	UTextBlock* Texto =
		WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			TEXT("TextoEspera")
		);

	if (!Texto)
	{
		return;
	}

	Texto->SetText(
		FText::FromString(
			TEXT("Se necesitan todos los jugadores para continuar")
		)
	);

	Texto->SetColorAndOpacity(
		FSlateColor(FLinearColor::White)
	);

	FSlateFontInfo Fuente = Texto->GetFont();
	Fuente.Size = 24;
	Texto->SetFont(Fuente);

	Texto->SetJustification(ETextJustify::Center);

	// Sombra para que el texto sea legible sobre el escenario.
	Texto->SetShadowColorAndOpacity(
		FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)
	);

	Texto->SetShadowOffset(FVector2D(2.0f, 2.0f));

	UCanvasPanelSlot* Espacio =
		Panel->AddChildToCanvas(Texto);

	if (Espacio)
	{
		// Centrado horizontalmente, cerca de la parte superior.
		Espacio->SetAnchors(FAnchors(0.5f, 0.15f));
		Espacio->SetAlignment(FVector2D(0.5f, 0.5f));
		Espacio->SetPosition(FVector2D::ZeroVector);
		Espacio->SetAutoSize(true);
	}
}

