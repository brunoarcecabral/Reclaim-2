#include "UI/ReclaimAvisoTiendaWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Styling/SlateColor.h"
#include "TimerManager.h"


void UReclaimAvisoTiendaWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Panel =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelAvisoTienda")
        );

    if (!Panel)
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    UBorder* Fondo =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass(),
            TEXT("FondoAvisoTienda")
        );

    if (!Fondo)
    {
        return;
    }

    Fondo->SetBrushColor(
        FLinearColor(0.02f, 0.02f, 0.02f, 0.85f)
    );

    Fondo->SetPadding(
        FMargin(18.0f, 10.0f)
    );

    UCanvasPanelSlot* Espacio =
        Panel->AddChildToCanvas(Fondo);

    if (Espacio)
    {
        // Centrado cerca de la parte inferior de la pantalla.
        Espacio->SetAnchors(FAnchors(0.5f, 1.0f));
        Espacio->SetAlignment(FVector2D(0.5f, 1.0f));
        Espacio->SetPosition(FVector2D(0.0f, -90.0f));
        Espacio->SetAutoSize(true);
    }

    TextoAviso =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoAvisoTienda")
        );

    if (!TextoAviso)
    {
        return;
    }

    FSlateFontInfo Fuente = TextoAviso->GetFont();
    Fuente.Size = 20;
    TextoAviso->SetFont(Fuente);

    TextoAviso->SetJustification(ETextJustify::Center);
    TextoAviso->SetWrapTextAt(500.0f);

    Fondo->SetContent(TextoAviso);
}


void UReclaimAvisoTiendaWidget::NativeDestruct()
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerCerrar);
    }

    Super::NativeDestruct();
}


void UReclaimAvisoTiendaWidget::MostrarAviso(
    APlayerController* Jugador,
    FText Mensaje,
    bool bExito,
    float Duracion
)
{
    if (!IsValid(Jugador) || !Jugador->IsLocalController())
    {
        return;
    }

    UReclaimAvisoTiendaWidget* Aviso =
        CreateWidget<UReclaimAvisoTiendaWidget>(
            Jugador,
            UReclaimAvisoTiendaWidget::StaticClass()
        );

    if (!IsValid(Aviso))
    {
        return;
    }

    Aviso->ConfigurarMensaje(Mensaje, bExito);

    if (!Aviso->AddToPlayerScreen(100))
    {
        return;
    }

    if (UWorld* Mundo = Aviso->GetWorld())
    {
        Mundo->GetTimerManager().SetTimer(
            Aviso->TimerCerrar,
            Aviso,
            &UReclaimAvisoTiendaWidget::CerrarAviso,
            FMath::Max(0.1f, Duracion),
            false
        );
    }
}


void UReclaimAvisoTiendaWidget::ConfigurarMensaje(
    const FText& Mensaje,
    bool bExito
)
{
    if (!IsValid(TextoAviso))
    {
        return;
    }

    TextoAviso->SetText(Mensaje);

    const FLinearColor Color =
        bExito
            ? FLinearColor(0.25f, 1.0f, 0.35f, 1.0f)
            : FLinearColor(1.0f, 0.3f, 0.25f, 1.0f);

    TextoAviso->SetColorAndOpacity(
        FSlateColor(Color)
    );
}


void UReclaimAvisoTiendaWidget::CerrarAviso()
{
    RemoveFromParent();
}

