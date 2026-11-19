#include "UI/ReclaimBibliotecaNotasWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Input/Events.h"
#include "InputCoreTypes.h"
#include "SistemaQuest/ReclaimNotaQuestActor.h"
#include "SistemaQuest/ReclaimNotaQuestData.h"
#include "Styling/SlateColor.h"
#include "UI/ReclaimEntradaTutorialWidget.h"

void UReclaimBibliotecaNotasWidget::NativeOnInitialized()
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
            TEXT("PanelBibliotecaNotas")
        );

    if (!Panel)
    {
        return;
    }

    WidgetTree->RootWidget = Panel;

    UBorder* Fondo =
        WidgetTree->ConstructWidget<UBorder>(
            UBorder::StaticClass(),
            TEXT("FondoBibliotecaNotas")
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
        Espacio->SetSize(FVector2D(900.0f, 650.0f));
    }

    UVerticalBox* Contenedor =
        WidgetTree->ConstructWidget<UVerticalBox>(
            UVerticalBox::StaticClass(),
            TEXT("ContenedorBibliotecaNotas")
        );

    if (!Contenedor)
    {
        return;
    }

    Fondo->SetContent(Contenedor);

    UTextBlock* Titulo =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TituloBibliotecaNotas")
        );

    if (Titulo)
    {
        Titulo->SetText(
            FText::FromString(TEXT("Notas encontradas"))
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

    TextoSinNotas =
        WidgetTree->ConstructWidget<UTextBlock>(
            UTextBlock::StaticClass(),
            TEXT("TextoSinNotas")
        );

    if (TextoSinNotas)
    {
        TextoSinNotas->SetText(
            FText::FromString(
                TEXT("Todavia no encontraste notas de mision.")
            )
        );

        TextoSinNotas->SetColorAndOpacity(
            FSlateColor(FLinearColor::White)
        );

        FSlateFontInfo Fuente = TextoSinNotas->GetFont();
        Fuente.Size = 18;
        TextoSinNotas->SetFont(Fuente);

        Contenedor->AddChildToVerticalBox(TextoSinNotas);
    }

    ListaNotas =
        WidgetTree->ConstructWidget<UScrollBox>(
            UScrollBox::StaticClass(),
            TEXT("ListaNotas")
        );

    if (ListaNotas)
    {
        UVerticalBoxSlot* EspacioLista =
            Contenedor->AddChildToVerticalBox(ListaNotas);

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
            TEXT("TextoCerrarNotas")
        );

    if (TextoCerrar)
    {
        TextoCerrar->SetText(
            FText::FromString(TEXT("Pulsa N para cerrar"))
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

void UReclaimBibliotecaNotasWidget::ActualizarLista()
{
    if (!IsValid(ListaNotas))
    {
        return;
    }

    ListaNotas->ClearChildren();

    UWorld* Mundo = GetWorld();
    APlayerController* Jugador = GetOwningPlayer();

    if (!Mundo || !IsValid(Jugador))
    {
        return;
    }

    int32 CantidadNotas = 0;
    
    TSet<UReclaimNotaQuestData*> NotasAgregadas;

    for (TActorIterator<AReclaimNotaQuestActor> It(Mundo); It; ++It)
    {
        AReclaimNotaQuestActor* Nota = *It;

        if (!IsValid(Nota) || !Nota->FueDescubierta())
        {
            continue;
        }

        UReclaimNotaQuestData* Datos = Nota->ObtenerDatosNota();

        if (!IsValid(Datos) || NotasAgregadas.Contains(Datos))
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
        ListaNotas->AddChild(Entrada);

        NotasAgregadas.Add(Datos);
        ++CantidadNotas;
    }

    if (IsValid(TextoSinNotas))
    {
        TextoSinNotas->SetVisibility(
            CantidadNotas == 0
                ? ESlateVisibility::Visible
                : ESlateVisibility::Collapsed
        );
    }
}

FReply UReclaimBibliotecaNotasWidget::NativeOnPreviewKeyDown(
    const FGeometry& InGeometry,
    const FKeyEvent& InKeyEvent
)
{
    if (InKeyEvent.GetKey() == EKeys::N)
    {
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

