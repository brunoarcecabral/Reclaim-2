#include "UI/ReclaimRecoleccionWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

void UReclaimRecoleccionWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Panel =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelRecoleccion")
        );

    if (!IsValid(Panel))
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    ListaRecursos =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ListaRecursos")
        );

    if (!IsValid(ListaRecursos))
    {
        return;
    }

    ListaRecursos->SetVisibility(
        ESlateVisibility::HitTestInvisible
    );

    UCanvasPanelSlot* SlotLista =
        Panel->AddChildToCanvas(ListaRecursos);

    if (SlotLista)
    {
        SlotLista->SetAnchors(FAnchors(0.0f, 0.5f));
        SlotLista->SetAlignment(FVector2D(0.0f, 0.5f));
        SlotLista->SetPosition(FVector2D(40.0f, 0.0f));
        SlotLista->SetAutoSize(true);
    }
}

void UReclaimRecoleccionWidget::ActualizarRecurso(
    const FString& Clave,
    const FText& Nombre,
    int32 Cantidad
)
{
    if (!WidgetTree || !IsValid(ListaRecursos))
    {
        return;
    }

    TObjectPtr<UTextBlock>* TextoExistente =
        TextosPorRecurso.Find(Clave);

    UTextBlock* Texto =
        TextoExistente ? TextoExistente->Get() : nullptr;

    if (!IsValid(Texto))
    {
        Texto = WidgetTree->ConstructWidget<UTextBlock>();

        if (!IsValid(Texto))
        {
            return;
        }

        Texto->SetColorAndOpacity(
    FSlateColor(FLinearColor(0.1f, 1.0f, 0.1f, 1.0f))
);
        
        FSlateFontInfo Fuente = Texto->GetFont();
        Fuente.Size = 24;
        Fuente.OutlineSettings.OutlineSize = 3;
        Fuente.OutlineSettings.OutlineColor = FLinearColor::Black;

        Texto->SetFont(Fuente);

        UVerticalBoxSlot* SlotRecurso =
    ListaRecursos->AddChildToVerticalBox(Texto);

        if (SlotRecurso)
        {
            SlotRecurso->SetPadding(
                FMargin(0.0f, 0.0f, 0.0f, 4.0f)
            );
        }
        
        TextosPorRecurso.Add(Clave, Texto);
    }

    Texto->SetText(
        FText::Format(
            FText::FromString(TEXT("+{0} {1}")),
            FText::AsNumber(Cantidad),
            Nombre
        )
    );
}

void UReclaimRecoleccionWidget::QuitarRecurso(
    const FString& Clave
)
{
    TObjectPtr<UTextBlock>* Texto = TextosPorRecurso.Find(Clave);

    if (Texto && IsValid(Texto->Get()))
    {
        Texto->Get()->RemoveFromParent();
    }

    TextosPorRecurso.Remove(Clave);
}

