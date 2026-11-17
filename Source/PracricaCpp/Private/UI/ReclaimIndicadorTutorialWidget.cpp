#include "UI/ReclaimIndicadorTutorialWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"


void UReclaimIndicadorTutorialWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!WidgetTree)
	{
		return;
	}

	UCanvasPanel* Panel =
		WidgetTree->ConstructWidget<UCanvasPanel>(
			UCanvasPanel::StaticClass(),
			TEXT("PanelIndicadorTutorial")
		);

	if (!Panel)
	{
		return;
	}

	WidgetTree->RootWidget = Panel;

	UTextBlock* Texto =
		WidgetTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			TEXT("TextoIndicadorTutorial")
		);

	if (!Texto)
	{
		return;
	}

	Texto->SetText(
		FText::FromString(TEXT("[T] Tutoriales"))
	);

	Texto->SetColorAndOpacity(
		FSlateColor(FLinearColor::White)
	);

	FSlateFontInfo Fuente = Texto->GetFont();
	Fuente.Size = 16;
	Texto->SetFont(Fuente);

	Texto->SetShadowColorAndOpacity(FLinearColor::Black);
	Texto->SetShadowOffset(FVector2D(2.0f, 2.0f));

	UCanvasPanelSlot* Espacio =
		Panel->AddChildToCanvas(Texto);

	if (Espacio)
	{
		Espacio->SetAnchors(FAnchors(1.0f, 1.0f));
		Espacio->SetAlignment(FVector2D(1.0f, 1.0f));
		Espacio->SetPosition(FVector2D(-35.0f, -35.0f));
		Espacio->SetAutoSize(true);
	}
}

