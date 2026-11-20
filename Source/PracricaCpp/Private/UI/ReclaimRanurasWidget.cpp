#include "UI/ReclaimRanurasWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "SistemaArmas/ReclaimArmaComponent.h"
#include "SistemaTarjeta/ReclaimTarjetaComponent.h"
#include "TimerManager.h"

void UReclaimRanurasWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree || WidgetTree->RootWidget)
    {
        return;
    }

    UCanvasPanel* Lienzo =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass()
        );

    WidgetTree->RootWidget = Lienzo;

    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass()
        );

    UCanvasPanelSlot* Posicion =
        Lienzo->AddChildToCanvas(Contenedor);

    if (Posicion)
    {
        Posicion->SetAnchors(FAnchors(0.0f, 1.0f));
        Posicion->SetAlignment(FVector2D(0.0f, 0.5f));
        Posicion->SetPosition(FVector2D(45.0f, -180.0f));
        Posicion->SetAutoSize(true);
    }

    // Ranura superior: arma.

    RanuraArma =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass()
        );

    RanuraArma->SetBrushColor(
        FLinearColor(0.05f, 0.05f, 0.05f, 0.9f)
    );

    RanuraArma->SetPadding(FMargin(4.0f));

    USizeBox* TamanoArma =
        WidgetTree->ConstructWidget<USizeBox>(
            USizeBox::StaticClass()
        );

    TamanoArma->SetWidthOverride(64.0f);
    TamanoArma->SetHeightOverride(64.0f);

    ImagenArma =
        WidgetTree->ConstructWidget<UImage>(
            UImage::StaticClass()
        );

    ImagenArma->SetVisibility(ESlateVisibility::Hidden);

    TamanoArma->SetContent(ImagenArma);
    RanuraArma->SetContent(TamanoArma);

    UVerticalBoxSlot* EspacioArma =
        Contenedor->AddChildToVerticalBox(RanuraArma);

    if (EspacioArma)
    {
        EspacioArma->SetPadding(
            FMargin(0.0f, 0.0f, 0.0f, 8.0f)
        );
    }

    // Ranura inferior: tarjeta.

    RanuraTarjeta =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass()
        );

    RanuraTarjeta->SetBrushColor(
        FLinearColor(0.05f, 0.05f, 0.05f, 0.9f)
    );

    RanuraTarjeta->SetPadding(FMargin(4.0f));

    USizeBox* TamanoTarjeta =
        WidgetTree->ConstructWidget<USizeBox>(
            USizeBox::StaticClass()
        );

    TamanoTarjeta->SetWidthOverride(64.0f);
    TamanoTarjeta->SetHeightOverride(64.0f);

    ImagenTarjeta =
        WidgetTree->ConstructWidget<UImage>(
            UImage::StaticClass()
        );

    ImagenTarjeta->SetVisibility(ESlateVisibility::Hidden);

    TamanoTarjeta->SetContent(ImagenTarjeta);

    RanuraTarjeta->SetContent(TamanoTarjeta);
    Contenedor->AddChildToVerticalBox(RanuraTarjeta);

    ActualizarEstadoArma();

    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().SetTimer(
            TimerActualizarArma,
            this,
            &UReclaimRanurasWidget::ActualizarEstadoArma,
            0.2f,
            true
        );
    }
}

void UReclaimRanurasWidget::NativeDestruct()
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerActualizarArma);
    }

    Super::NativeDestruct();
}

void UReclaimRanurasWidget::ActualizarEstadoArma()
{
    // También actualizamos la ranura inferior.
    ActualizarEstadoTarjeta();

    if (!IsValid(RanuraArma) || !IsValid(ImagenArma))
    {
        return;
    }

    APawn* Personaje = GetOwningPlayerPawn();

    UReclaimArmaComponent* Arma = IsValid(Personaje)
        ? Personaje->FindComponentByClass<UReclaimArmaComponent>()
        : nullptr;

    if (!IsValid(Arma) || !Arma->TieneArma())
    {
        ImagenArma->SetVisibility(ESlateVisibility::Hidden);

        RanuraArma->SetBrushColor(
            FLinearColor(0.05f, 0.05f, 0.05f, 0.9f)
        );

        return;
    }

    UTexture2D* Icono = Arma->ObtenerIconoArma();

    if (IsValid(Icono))
    {
        ImagenArma->SetBrushFromTexture(Icono);
        ImagenArma->SetVisibility(ESlateVisibility::HitTestInvisible);

        if (Arma->EstaEquipada())
        {
            ImagenArma->SetColorAndOpacity(FLinearColor::White);

            RanuraArma->SetBrushColor(
                FLinearColor(0.05f, 0.65f, 0.12f, 0.95f)
            );
        }
        else
        {
            ImagenArma->SetColorAndOpacity(
                FLinearColor(0.4f, 0.4f, 0.4f, 1.0f)
            );

            RanuraArma->SetBrushColor(
                FLinearColor(0.05f, 0.05f, 0.05f, 0.9f)
            );
        }
    }
    else
    {
        ImagenArma->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UReclaimRanurasWidget::ActualizarEstadoTarjeta()
{
    if (!IsValid(RanuraTarjeta) || !IsValid(ImagenTarjeta))
    {
        return;
    }

    APawn* Personaje = GetOwningPlayerPawn();

    UReclaimTarjetaComponent* Tarjeta = IsValid(Personaje)
        ? Personaje->FindComponentByClass<UReclaimTarjetaComponent>()
        : nullptr;

    if (IsValid(Tarjeta) &&
        Tarjeta->TieneTarjeta() &&
        IsValid(Tarjeta->IconoTarjeta))
    {
        ImagenTarjeta->SetBrushFromTexture(Tarjeta->IconoTarjeta);
        ImagenTarjeta->SetColorAndOpacity(FLinearColor::White);
        ImagenTarjeta->SetVisibility(ESlateVisibility::HitTestInvisible);

        RanuraTarjeta->SetBrushColor(
            FLinearColor(0.05f, 0.65f, 0.12f, 0.95f)
        );
    }
    else
    {
        ImagenTarjeta->SetVisibility(ESlateVisibility::Hidden);

        RanuraTarjeta->SetBrushColor(
            FLinearColor(0.05f, 0.05f, 0.05f, 0.9f)
        );
    }
}
