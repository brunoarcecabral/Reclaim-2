#include "UI/ReclaimBibliotecaTutorialWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "SistemaNivel/ReclaimBibliotecaTutorialComponent.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "Styling/SlateColor.h"
#include "UI/ReclaimEntradaTutorialWidget.h"


void UReclaimBibliotecaTutorialWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // Permite que la ventana reciba la tecla T.
    SetIsFocusable(true);

    if (!WidgetTree)
    {
        return;
    }

    UCanvasPanel* Panel =
        WidgetTree->ConstructWidget<UCanvasPanel>(
            UCanvasPanel::StaticClass(),
            TEXT("PanelBiblioteca")
        );

    if (!Panel)
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    // Fondo semitransparente de la ventana.
    UBorder* Fondo =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass(),
            TEXT("FondoBiblioteca")
        );

    if (!Fondo)
    {
        return;
    }

    Fondo->SetBrushColor(
        FLinearColor(0.02f, 0.02f, 0.02f, 0.92f)
    );

    Fondo->SetPadding(FMargin(28.0f));

    UCanvasPanelSlot* Espacio =
        Panel->AddChildToCanvas(Fondo);

    if (Espacio)
    {
        Espacio->SetAnchors(FAnchors(0.5f, 0.5f));
        Espacio->SetAlignment(FVector2D(0.5f, 0.5f));
        Espacio->SetPosition(FVector2D::ZeroVector);

        // Más grande que la ventana de las notas.
        Espacio->SetSize(FVector2D(900.0f, 650.0f));
    }

    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenedorBiblioteca")
        );

    if (!Contenedor)
    {
        return;
    }

    Fondo->SetContent(Contenedor);

    UTextBlock* Titulo =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TituloBiblioteca")
        );

    if (Titulo)
    {
        Titulo->SetText(
            FText::FromString(TEXT("Base de datos de tutoriales"))
        );

        Titulo->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        FSlateFontInfo Fuente = Titulo->GetFont();
        Fuente.Size = 30;
        Titulo->SetFont(Fuente);

        UVerticalBoxSlot* EspacioTitulo =
            Contenedor->AddChildToVerticalBox(Titulo);

        if (EspacioTitulo)
        {
            EspacioTitulo->SetPadding(
                FMargin(0.0f, 0.0f, 0.0f, 20.0f)
            );
        }
    }

    ListaTutoriales =
        WidgetTree->ConstructWidget<UScrollBox>(
            UScrollBox::StaticClass(),
            TEXT("ListaTutoriales")
        );

    if (ListaTutoriales)
    {
        UVerticalBoxSlot* EspacioLista =
            Contenedor->AddChildToVerticalBox(ListaTutoriales);

        if (EspacioLista)
        {
            EspacioLista->SetSize(
                FSlateChildSize(ESlateSizeRule::Fill)
            );
        }
    }

    UTextBlock* TextoCerrar =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoCerrar")
        );

    if (TextoCerrar)
    {
        TextoCerrar->SetText(
            FText::FromString(TEXT("Pulsa T para cerrar"))
        );

        TextoCerrar->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        FSlateFontInfo Fuente = TextoCerrar->GetFont();
        Fuente.Size = 18;
        TextoCerrar->SetFont(Fuente);

        UVerticalBoxSlot* EspacioCerrar =
            Contenedor->AddChildToVerticalBox(TextoCerrar);

        if (EspacioCerrar)
        {
            EspacioCerrar->SetHorizontalAlignment(HAlign_Center);
            EspacioCerrar->SetPadding(
                FMargin(0.0f, 20.0f, 0.0f, 0.0f)
            );
        }
    }
}


void UReclaimBibliotecaTutorialWidget::Configurar(
    UReclaimBibliotecaTutorialComponent* Biblioteca
)
{
    if (!IsValid(ListaTutoriales) || !IsValid(Biblioteca))
    {
        return;
    }

    ListaTutoriales->ClearChildren();

    const TArray<UReclaimNotaQuestData*> Tutoriales =
        Biblioteca->ObtenerTutorialesDisponibles();

    APlayerController* Jugador = GetOwningPlayer();

    if (!IsValid(Jugador))
    {
        return;
    }

    for (UReclaimNotaQuestData* Datos : Tutoriales)
    {
        if (!IsValid(Datos))
        {
            continue;
        }

        UReclaimEntradaTutorialWidget* Entrada =
            CreateWidget<UReclaimEntradaTutorialWidget>(
                Jugador,
                UReclaimEntradaTutorialWidget::StaticClass()
            );

        if (!IsValid(Entrada))
        {
            continue;
        }

        Entrada->Configurar(Datos);
        ListaTutoriales->AddChild(Entrada);
    }
}


FReply UReclaimBibliotecaTutorialWidget::NativeOnPreviewKeyDown(
    const FGeometry& InGeometry,
    const FKeyEvent& InKeyEvent
)
{
    if (InKeyEvent.GetKey() == EKeys::T)
    {
        // Evita repetir el cierre si se mantiene la tecla apretada.
        if (!InKeyEvent.IsRepeat())
        {
            AlCerrarBiblioteca.Broadcast();
        }

        return FReply::Handled();
    }

    return Super::NativeOnPreviewKeyDown(
        InGeometry,
        InKeyEvent
    );
}

