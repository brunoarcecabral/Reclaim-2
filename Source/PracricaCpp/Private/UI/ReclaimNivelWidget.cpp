#include "UI/ReclaimNivelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "SistemaNivel/ReclaimNivelComponent.h"
#include "Styling/SlateColor.h"


void UReclaimNivelWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Panel =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelNivel")
        );

    if (!Panel)
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenedorNivel")
        );

    if (!Contenedor)
    {
        return;
    }

    UCanvasPanelSlot* Espacio =
        Panel->AddChildToCanvas(Contenedor);

    if (Espacio)
    {
        // Esquina superior derecha, por encima de las misiones.
        Espacio->SetAnchors(FAnchors(1.0f, 0.0f));
        Espacio->SetAlignment(FVector2D(1.0f, 0.0f));
        Espacio->SetPosition(FVector2D(-35.0f, 35.0f));
        Espacio->SetAutoSize(true);
    }

    TextoNivel =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoNivel")
        );

    if (TextoNivel)
    {
        TextoNivel->SetText(FText::FromString(TEXT("Nivel 1")));
        TextoNivel->SetJustification(ETextJustify::Right);
        TextoNivel->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        FSlateFontInfo Fuente = TextoNivel->GetFont();
        Fuente.Size = 22;
        TextoNivel->SetFont(Fuente);

        TextoNivel->SetShadowColorAndOpacity(FLinearColor::Black);
        TextoNivel->SetShadowOffset(FVector2D(2.0f, 2.0f));

        UVerticalBoxSlot* EspacioTexto =
            Contenedor->AddChildToVerticalBox(TextoNivel);

        if (EspacioTexto)
        {
            EspacioTexto->SetHorizontalAlignment(HAlign_Right);
        }
    }

    USizeBox* CajaBarra =
        WidgetTree->ConstructWidget<USizeBox>(
            USizeBox::StaticClass(),
            TEXT("CajaBarraExperiencia")
        );

    BarraExperiencia =
        WidgetTree->ConstructWidget<UProgressBar>(
            UProgressBar::StaticClass(),
            TEXT("BarraExperiencia")
        );

    if (CajaBarra && BarraExperiencia)
    {
        CajaBarra->SetWidthOverride(220.0f);
        CajaBarra->SetHeightOverride(14.0f);

        BarraExperiencia->SetPercent(0.0f);
        BarraExperiencia->SetFillColorAndOpacity(
            FLinearColor(0.1f, 0.65f, 1.0f, 1.0f)
        );

        CajaBarra->SetContent(BarraExperiencia);

        UVerticalBoxSlot* EspacioBarra =
            Contenedor->AddChildToVerticalBox(CajaBarra);

        if (EspacioBarra)
        {
            EspacioBarra->SetPadding(
                FMargin(0.0f, 5.0f, 0.0f, 2.0f)
            );
        }
    }

    TextoExperiencia =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoExperiencia")
        );

    if (TextoExperiencia)
    {
        TextoExperiencia->SetText(
            FText::FromString(TEXT("0 / 100 EXP"))
        );

        TextoExperiencia->SetJustification(ETextJustify::Right);
        TextoExperiencia->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        FSlateFontInfo Fuente = TextoExperiencia->GetFont();
        Fuente.Size = 14;
        TextoExperiencia->SetFont(Fuente);

        TextoExperiencia->SetShadowColorAndOpacity(FLinearColor::Black);
        TextoExperiencia->SetShadowOffset(FVector2D(2.0f, 2.0f));

        UVerticalBoxSlot* EspacioExperiencia =
            Contenedor->AddChildToVerticalBox(TextoExperiencia);

        if (EspacioExperiencia)
        {
            EspacioExperiencia->SetHorizontalAlignment(HAlign_Right);
        }
    }
}


void UReclaimNivelWidget::ConectarNivel(
    UReclaimNivelComponent* NuevoComponente
)
{
    if (IsValid(ComponenteNivel))
    {
        ComponenteNivel->AlCambiarNivel.RemoveDynamic(
            this,
            &UReclaimNivelWidget::ActualizarNivel
        );
    }

    ComponenteNivel = NuevoComponente;

    if (!IsValid(ComponenteNivel))
    {
        return;
    }

    ComponenteNivel->AlCambiarNivel.AddDynamic(
        this,
        &UReclaimNivelWidget::ActualizarNivel
    );

    ActualizarNivel();
}


void UReclaimNivelWidget::NativeDestruct()
{
    if (IsValid(ComponenteNivel))
    {
        ComponenteNivel->AlCambiarNivel.RemoveDynamic(
            this,
            &UReclaimNivelWidget::ActualizarNivel
        );
    }

    ComponenteNivel = nullptr;

    Super::NativeDestruct();
}


void UReclaimNivelWidget::ActualizarNivel()
{
    if (!IsValid(ComponenteNivel))
    {
        return;
    }

    const FEstadoNivelReclaim Estado =
        ComponenteNivel->ObtenerEstadoNivel();

    if (IsValid(TextoNivel))
    {
        TextoNivel->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Nivel %d"),
                    Estado.Nivel
                )
            )
        );
    }

    if (Estado.ExperienciaRequerida <= 0)
    {
        if (IsValid(BarraExperiencia))
        {
            BarraExperiencia->SetPercent(1.0f);
        }

        if (IsValid(TextoExperiencia))
        {
            TextoExperiencia->SetText(
                FText::FromString(TEXT("Nivel máximo"))
            );
        }

        return;
    }

    const float Porcentaje = FMath::Clamp(
        static_cast<float>(Estado.ExperienciaActual) /
            static_cast<float>(Estado.ExperienciaRequerida),
        0.0f,
        1.0f
    );

    if (IsValid(BarraExperiencia))
    {
        BarraExperiencia->SetPercent(Porcentaje);
    }

    if (IsValid(TextoExperiencia))
    {
        TextoExperiencia->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("%d / %d EXP"),
                    Estado.ExperienciaActual,
                    Estado.ExperienciaRequerida
                )
            )
        );
    }
}

