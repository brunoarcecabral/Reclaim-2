#include "SistemaQuest/ReclaimNotaQuestWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "Styling/SlateColor.h"


void UReclaimNotaQuestWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    SetIsFocusable(true);

    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Panel =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelNota")
        );

    if (!Panel)
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    UBorder* Fondo =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass(),
            TEXT("FondoNota")
        );

    if (!Fondo)
    {
        return;
    }

    Fondo->SetBrushColor(
        FLinearColor(0.02f, 0.02f, 0.02f, 0.85f)
    );

    Fondo->SetPadding(FMargin(28.0f));

    UCanvasPanelSlot* Espacio =
        Panel->AddChildToCanvas(Fondo);

    if (Espacio)
    {
        Espacio->SetAnchors(FAnchors(0.5f, 0.5f));
        Espacio->SetAlignment(FVector2D(0.5f, 0.5f));
        Espacio->SetPosition(FVector2D::ZeroVector);
        Espacio->SetSize(FVector2D(700.0f, 420.0f));
    }

    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenidoNota")
        );

    if (!Contenedor)
    {
        return;
    }

    Fondo->SetContent(Contenedor);

    TextoTitulo =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TituloNota")
        );

    if (TextoTitulo)
    {
        FSlateFontInfo FuenteTitulo = TextoTitulo->GetFont();
        FuenteTitulo.Size = 28;
        TextoTitulo->SetFont(FuenteTitulo);

        TextoTitulo->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        UVerticalBoxSlot* EspacioTitulo =
            Contenedor->AddChildToVerticalBox(TextoTitulo);

        if (EspacioTitulo)
        {
            EspacioTitulo->SetPadding(
                FMargin(0.0f, 0.0f, 0.0f, 20.0f)
            );
        }
    }

    TextoContenido =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoNota")
        );

    if (TextoContenido)
    {
        FSlateFontInfo FuenteContenido = TextoContenido->GetFont();
        FuenteContenido.Size = 20;
        TextoContenido->SetFont(FuenteContenido);

        TextoContenido->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        TextoContenido->SetAutoWrapText(true);

        UVerticalBoxSlot* EspacioContenido =
            Contenedor->AddChildToVerticalBox(TextoContenido);

        if (EspacioContenido)
        {
            EspacioContenido->SetSize(
                FSlateChildSize(ESlateSizeRule::Fill)
            );
        }
    }

    TextoContinuar =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoContinuar")
        );

    if (TextoContinuar)
    {
        // Las notas normales siguen usando E.
        TextoContinuar->SetText(
            FText::FromString(TEXT("Pulsa E para continuar"))
        );

        FSlateFontInfo FuenteContinuar = TextoContinuar->GetFont();
        FuenteContinuar.Size = 18;
        TextoContinuar->SetFont(FuenteContinuar);

        TextoContinuar->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        UVerticalBoxSlot* EspacioContinuar =
            Contenedor->AddChildToVerticalBox(TextoContinuar);

        if (EspacioContinuar)
        {
            EspacioContinuar->SetHorizontalAlignment(HAlign_Center);

            EspacioContinuar->SetPadding(
                FMargin(0.0f, 20.0f, 0.0f, 0.0f)
            );
        }
    }
}


void UReclaimNotaQuestWidget::MostrarNota(
    UReclaimNotaQuestData* DatosNota
)
{
    if (!IsValid(DatosNota))
    {
        return;
    }

    if (IsValid(TextoTitulo))
    {
        TextoTitulo->SetText(DatosNota->Titulo);
    }

    if (IsValid(TextoContenido))
    {
        TextoContenido->SetText(DatosNota->Contenido);
    }
}


void UReclaimNotaQuestWidget::UsarEscapeParaContinuar()
{
    bContinuarConEscape = true;

    if (IsValid(TextoContinuar))
    {
        TextoContinuar->SetText(
            FText::FromString(TEXT("Pulsa Esc para continuar"))
        );
    }
}


FReply UReclaimNotaQuestWidget::NativeOnKeyDown(
    const FGeometry& InGeometry,
    const FKeyEvent& InKeyEvent
)
{
    const FKey Tecla = InKeyEvent.GetKey();

    const bool bTeclaCorrecta =
        (bContinuarConEscape && Tecla == EKeys::Escape) ||
        (!bContinuarConEscape && Tecla == EKeys::E);

    if (bTeclaCorrecta)
    {
        AlContinuarNota.Broadcast();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
void UReclaimNotaQuestWidget::UsarCierreAutomatico()
{
    bContinuarConEscape = true;

    if (IsValid(TextoContinuar))
    {
        TextoContinuar->SetText(
            FText::FromString(
                TEXT("Esta ventana se cerrara automaticamente")
            )
        );
    }
}


