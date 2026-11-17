#include "UI/ReclaimEntradaTutorialWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "Styling/SlateColor.h"


void UReclaimEntradaTutorialWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (!WidgetTree)
    {
        return;
    }

    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenedorEntrada")
        );

    if (!Contenedor)
    {
        return;
    }

    WidgetTree->RootWidget = Contenedor;

    // Botón que abre y cierra el contenido.
    BotonTitulo =
        WidgetTree->ConstructWidget<UButton>(
            UButton::StaticClass(),
            TEXT("BotonTitulo")
        );

    TextoTitulo =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoTitulo")
        );

    if (BotonTitulo && TextoTitulo)
    {
        FSlateFontInfo FuenteTitulo = TextoTitulo->GetFont();
        FuenteTitulo.Size = 22;
        TextoTitulo->SetFont(FuenteTitulo);

        TextoTitulo->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        BotonTitulo->AddChild(TextoTitulo);

        UVerticalBoxSlot* EspacioBoton =
            Contenedor->AddChildToVerticalBox(BotonTitulo);

        if (EspacioBoton)
        {
            EspacioBoton->SetPadding(
                FMargin(0.0f, 5.0f, 0.0f, 0.0f)
            );
        }

        BotonTitulo->OnClicked.AddUniqueDynamic(
            this,
            &UReclaimEntradaTutorialWidget::AlternarContenido
        );
    }

    // Texto que aparece al abrir la sección.
    TextoContenido =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoContenido")
        );

    if (TextoContenido)
    {
        FSlateFontInfo FuenteContenido = TextoContenido->GetFont();
        FuenteContenido.Size = 18;
        TextoContenido->SetFont(FuenteContenido);

        TextoContenido->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        TextoContenido->SetAutoWrapText(true);

        // Las secciones empiezan cerradas.
        TextoContenido->SetVisibility(ESlateVisibility::Collapsed);

        UVerticalBoxSlot* EspacioContenido =
            Contenedor->AddChildToVerticalBox(TextoContenido);

        if (EspacioContenido)
        {
            EspacioContenido->SetPadding(
                FMargin(16.0f, 10.0f, 16.0f, 10.0f)
            );
        }
    }
}


void UReclaimEntradaTutorialWidget::Configurar(
    UReclaimNotaQuestData* DatosTutorial
)
{
    if (!IsValid(DatosTutorial))
    {
        return;
    }

    if (IsValid(TextoTitulo))
    {
        TextoTitulo->SetText(DatosTutorial->Titulo);
    }

    if (IsValid(TextoContenido))
    {
        TextoContenido->SetText(DatosTutorial->Contenido);
    }
}


void UReclaimEntradaTutorialWidget::AlternarContenido()
{
    if (!IsValid(TextoContenido))
    {
        return;
    }

    if (TextoContenido->GetVisibility() == ESlateVisibility::Collapsed)
    {
        TextoContenido->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        TextoContenido->SetVisibility(ESlateVisibility::Collapsed);
    }
}

