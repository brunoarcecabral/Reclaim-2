#include "SistemaQuest/ReclaimMisionesWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"
#include "Styling/SlateColor.h"
#include "TimerManager.h"


void UReclaimMisionesWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Panel =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelMisiones")
        );

    if (!Panel)
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    ListaMisiones =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ListaMisiones")
        );

    if (!ListaMisiones)
    {
        return;
    }

    UCanvasPanelSlot* Espacio =
        Panel->AddChildToCanvas(ListaMisiones);

    if (Espacio)
    {
        // Esquina superior derecha, debajo de la interfaz de nivel.
        Espacio->SetAnchors(FAnchors(1.0f, 0.0f));
        Espacio->SetAlignment(FVector2D(1.0f, 0.0f));
        Espacio->SetPosition(FVector2D(-35.0f, 160.0f));
        Espacio->SetAutoSize(true);
    }
}


void UReclaimMisionesWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return;
    }

    ComponenteQuest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(ComponenteQuest))
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("QUEST UI: No se encontro el componente de misiones.")
        );

        return;
    }

    ComponenteQuest->AlCambiarMisiones.RemoveDynamic(
        this,
        &UReclaimMisionesWidget::ActualizarMisiones
    );

    ComponenteQuest->AlCambiarMisiones.AddDynamic(
        this,
        &UReclaimMisionesWidget::ActualizarMisiones
    );

    ActualizarMisiones();
}


void UReclaimMisionesWidget::NativeDestruct()
{
    if (UWorld* Mundo = GetWorld())
    {
        Mundo->GetTimerManager().ClearTimer(TimerDespedida);
    }

    if (IsValid(ComponenteQuest))
    {
        ComponenteQuest->AlCambiarMisiones.RemoveDynamic(
            this,
            &UReclaimMisionesWidget::ActualizarMisiones
        );
    }

    ComponenteQuest = nullptr;

    Super::NativeDestruct();
}


void UReclaimMisionesWidget::AgregarTitulo(
    const FText& Titulo
)
{
    if (!WidgetTree || !IsValid(ListaMisiones))
    {
        return;
    }

    UTextBlock* Texto =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass()
        );

    if (!Texto)
    {
        return;
    }

    Texto->SetText(Titulo);
    Texto->SetJustification(ETextJustify::Right);

    Texto->SetColorAndOpacity(
        FSlateColor(FLinearColor::White)
    );

    FSlateFontInfo Fuente = Texto->GetFont();
    Fuente.Size = 22;
    Texto->SetFont(Fuente);

    Texto->SetShadowColorAndOpacity(
        FLinearColor::Black
    );

    Texto->SetShadowOffset(FVector2D(2.0f, 2.0f));

    UVerticalBoxSlot* Espacio =
        ListaMisiones->AddChildToVerticalBox(Texto);

    if (Espacio)
    {
        Espacio->SetHorizontalAlignment(HAlign_Right);
        Espacio->SetPadding(FMargin(0.0f, 12.0f, 0.0f, 5.0f));
    }
}


void UReclaimMisionesWidget::AgregarObjetivo(
    const FText& Descripcion,
    int32 CantidadActual,
    int32 CantidadRequerida,
    bool bCompletado
)
{
    if (!WidgetTree || !IsValid(ListaMisiones))
    {
        return;
    }

    UOverlay* Contenedor =
        WidgetTree->ConstructWidget<UOverlay>(
            UOverlay::StaticClass()
        );

    UTextBlock* Texto =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass()
        );

    if (!Contenedor || !Texto)
    {
        return;
    }

    const FString Contenido = FString::Printf(
        TEXT("%s (%d/%d)"),
        *Descripcion.ToString(),
        CantidadActual,
        CantidadRequerida
    );

    Texto->SetText(FText::FromString(Contenido));
    Texto->SetJustification(ETextJustify::Right);
    Texto->SetWrapTextAt(420.0f);

    const FLinearColor Color =
        bCompletado
            ? FLinearColor(0.2f, 1.0f, 0.25f, 1.0f)
            : FLinearColor::White;

    Texto->SetColorAndOpacity(FSlateColor(Color));

    FSlateFontInfo Fuente = Texto->GetFont();
    Fuente.Size = 20;
    Texto->SetFont(Fuente);

    Texto->SetShadowColorAndOpacity(FLinearColor::Black);
    Texto->SetShadowOffset(FVector2D(2.0f, 2.0f));

    Contenedor->AddChildToOverlay(Texto);

    if (bCompletado)
    {
        // Línea verde sobre el texto para mostrarlo tachado.
        USizeBox* CajaLinea =
            WidgetTree->ConstructWidget<USizeBox>(
                USizeBox::StaticClass()
            );

        UBorder* Linea =
            WidgetTree->ConstructWidget<UBorder>(
                UBorder::StaticClass()
            );

        if (CajaLinea && Linea)
        {
            CajaLinea->SetHeightOverride(2.0f);

            Linea->SetPadding(FMargin(0.0f));
            Linea->SetBrushColor(Color);

            CajaLinea->SetContent(Linea);

            UOverlaySlot* EspacioLinea =
                Contenedor->AddChildToOverlay(CajaLinea);

            if (EspacioLinea)
            {
                EspacioLinea->SetHorizontalAlignment(HAlign_Fill);
                EspacioLinea->SetVerticalAlignment(VAlign_Center);
            }
        }
    }

    UVerticalBoxSlot* EspacioObjetivo =
        ListaMisiones->AddChildToVerticalBox(Contenedor);

    if (EspacioObjetivo)
    {
        EspacioObjetivo->SetHorizontalAlignment(HAlign_Right);
        EspacioObjetivo->SetPadding(
            FMargin(0.0f, 2.0f, 0.0f, 2.0f)
        );
    }
}


void UReclaimMisionesWidget::ActualizarMisiones()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo ||
        !IsValid(ListaMisiones) ||
        !IsValid(ComponenteQuest))
    {
        return;
    }

    const TArray<FEstadoQuest> Misiones =
        ComponenteQuest->ObtenerMisiones();

    const double TiempoActual = Mundo->GetTimeSeconds();

    // Registrar qué misiones estuvieron activas en esta pantalla.
    for (const FEstadoQuest& Estado : Misiones)
    {
        if (!IsValid(Estado.DatosMision))
        {
            continue;
        }

        const FName IdMision = Estado.DatosMision->IdMision;

        if (!Estado.bCompletada)
        {
            MisionesObservadasActivas.Add(IdMision);
        }
        else if (
            MisionesObservadasActivas.Contains(IdMision) &&
            !MisionesEnDespedida.Contains(IdMision)
        )
        {
            MisionesEnDespedida.Add(
                IdMision,
                TiempoActual + 3.0
            );

            MisionesObservadasActivas.Remove(IdMision);
        }
    }

    // Quitar las misiones cuyo tiempo de presentación terminó.
    for (auto It = MisionesEnDespedida.CreateIterator(); It; ++It)
    {
        if (TiempoActual >= It.Value())
        {
            It.RemoveCurrent();
        }
    }

    ListaMisiones->ClearChildren();

    const bool bMostrandoDespedida =
        !MisionesEnDespedida.IsEmpty();

    for (const FEstadoQuest& Estado : Misiones)
    {
        const UReclaimQuestData* Datos = Estado.DatosMision;

        if (!IsValid(Datos))
        {
            continue;
        }

        const bool bMostrarCompletada =
            Estado.bCompletada &&
            MisionesEnDespedida.Contains(Datos->IdMision);

        if (Estado.bCompletada && !bMostrarCompletada)
        {
            continue;
        }

        // Espera a terminar la presentación de la misión anterior.
        if (!Estado.bCompletada && bMostrandoDespedida)
        {
            continue;
        }

        AgregarTitulo(Datos->NombreMision);

        // Primero: el primer objetivo que falta completar.
        if (!Estado.bCompletada)
        {
            for (const FObjetivoQuest& Objetivo : Datos->Objetivos)
            {
                const FProgresoObjetivoQuest* Progreso =
                    Estado.ProgresoObjetivos.FindByPredicate(
                        [&Objetivo](const FProgresoObjetivoQuest& Actual)
                        {
                            return Actual.IdObjetivo == Objetivo.IdObjetivo;
                        }
                    );

                const int32 Cantidad =
                    Progreso ? Progreso->CantidadActual : 0;

                if (Cantidad < Objetivo.CantidadRequerida)
                {
                    AgregarObjetivo(
                        Objetivo.Descripcion,
                        Cantidad,
                        Objetivo.CantidadRequerida,
                        false
                    );

                    break;
                }
            }
        }

        // Después: todos los objetivos completados.
        for (const FObjetivoQuest& Objetivo : Datos->Objetivos)
        {
            const FProgresoObjetivoQuest* Progreso =
                Estado.ProgresoObjetivos.FindByPredicate(
                    [&Objetivo](const FProgresoObjetivoQuest& Actual)
                    {
                        return Actual.IdObjetivo == Objetivo.IdObjetivo;
                    }
                );

            const int32 Cantidad =
                Progreso ? Progreso->CantidadActual : 0;

            if (Cantidad >= Objetivo.CantidadRequerida)
            {
                AgregarObjetivo(
                    Objetivo.Descripcion,
                    Cantidad,
                    Objetivo.CantidadRequerida,
                    true
                );
            }
        }
    }

    Mundo->GetTimerManager().ClearTimer(TimerDespedida);

    if (!MisionesEnDespedida.IsEmpty())
    {
        double TiempoRestante = 3.0;

        for (const auto& Entrada : MisionesEnDespedida)
        {
            TiempoRestante = FMath::Min(
                TiempoRestante,
                Entrada.Value - TiempoActual
            );
        }

        Mundo->GetTimerManager().SetTimer(
            TimerDespedida,
            this,
            &UReclaimMisionesWidget::ActualizarMisiones,
            static_cast<float>(FMath::Max(0.05, TiempoRestante)),
            false
        );
    }
}

