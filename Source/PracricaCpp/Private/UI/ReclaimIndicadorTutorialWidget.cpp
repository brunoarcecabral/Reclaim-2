#include "UI/ReclaimIndicadorTutorialWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
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

    UHorizontalBox* Contenedor =
        WidgetTree->ConstructWidget<UHorizontalBox>(
            UHorizontalBox::StaticClass(),
            TEXT("ContenedorIndicadores")
        );

    if (!Contenedor)
    {
        return;
    }

    UCanvasPanelSlot* Espacio =
        Panel->AddChildToCanvas(Contenedor);

    if (Espacio)
    {
        Espacio->SetAnchors(FAnchors(1.0f, 1.0f));
        Espacio->SetAlignment(FVector2D(1.0f, 1.0f));
        Espacio->SetPosition(FVector2D(-35.0f, -35.0f));
        Espacio->SetAutoSize(true);
    }

    auto CrearIndicador = [this, Contenedor](
        const FName NombreBorde,
        const FName NombreFondo,
        const FName NombreTexto,
        const FString& Etiqueta
    )
    {
        UBorder* Borde =
            WidgetTree->ConstructWidget<UBorder>(
                UBorder::StaticClass(),
                NombreBorde
            );

        UBorder* Fondo =
            WidgetTree->ConstructWidget<UBorder>(
                UBorder::StaticClass(),
                NombreFondo
            );

        UTextBlock* Texto =
            WidgetTree->ConstructWidget<UTextBlock>(
                UTextBlock::StaticClass(),
                NombreTexto
            );

        if (!Borde || !Fondo || !Texto)
        {
            return;
        }
        
        Borde->SetBrushColor(FLinearColor::Black);
        Borde->SetPadding(FMargin(2.0f));
        
        Fondo->SetBrushColor(
            FLinearColor(1.0f, 0.75f, 0.0f, 1.0f)
        );

        Fondo->SetPadding(
            FMargin(10.0f, 5.0f, 10.0f, 5.0f)
        );

        Texto->SetText(FText::FromString(Etiqueta));

        Texto->SetColorAndOpacity(
            FSlateColor(FLinearColor::Black)
        );

        FSlateFontInfo Fuente = Texto->GetFont();
        Fuente.Size = 16;
        Texto->SetFont(Fuente);

        Fondo->SetContent(Texto);
        Borde->SetContent(Fondo);

        UHorizontalBoxSlot* Slot =
            Contenedor->AddChildToHorizontalBox(Borde);

        if (Slot)
        {
            Slot->SetPadding(
                FMargin(0.0f, 0.0f, 8.0f, 0.0f)
            );
        }
    };

    CrearIndicador(
        TEXT("BordeTutoriales"),
        TEXT("FondoTutoriales"),
        TEXT("TextoTutoriales"),
        TEXT("[T] Tutoriales")
    );

    CrearIndicador(
        TEXT("BordeNotas"),
        TEXT("FondoNotas"),
        TEXT("TextoNotas"),
        TEXT("[N] Notas")
    );
}

