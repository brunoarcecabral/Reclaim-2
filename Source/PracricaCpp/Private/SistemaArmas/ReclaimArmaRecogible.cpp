#include "SistemaArmas/ReclaimArmaRecogible.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "SistemaArmas/ReclaimArmaComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "SistemaQuest/ReclaimQuestComponent.h"
#include "SistemaQuest/ReclaimQuestData.h"


AReclaimArmaRecogible::AReclaimArmaRecogible()
{
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    Raiz = CreateDefaultSubobject<USceneComponent>(TEXT("Raiz"));
    SetRootComponent(Raiz);

    ModeloArma =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ModeloArma"));

    ModeloArma->SetupAttachment(Raiz);

    CartelRecoger =
        CreateDefaultSubobject<UTextRenderComponent>(TEXT("CartelRecoger"));

    CartelRecoger->SetupAttachment(Raiz);
    CartelRecoger->SetText(FText::FromString(TEXT("E - Recoger")));
    CartelRecoger->SetHorizontalAlignment(EHTA_Center);
    CartelRecoger->SetTextRenderColor(FColor::White);
    CartelRecoger->SetWorldSize(20.0f);
    CartelRecoger->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
    CartelRecoger->SetVisibility(false);
}

bool AReclaimArmaRecogible::EntregarAlJugador(
    UReclaimArmaComponent* ArmaJugador
)
{
    if (!HasAuthority() ||
    !IsValid(ArmaJugador) ||
    IdArma.IsNone() ||
    !EstaDisponibleEnMision())
    
    {
        return false;
    }

    return ArmaJugador->EntregarArma(IdArma);
}

void AReclaimArmaRecogible::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!EstaDisponibleEnMision())
    {
        ModeloArma->SetVisibility(false);
        CartelRecoger->SetVisibility(false);
        return;
    }
    
    APawn* PersonajeLocal = UGameplayStatics::GetPlayerPawn(this, 0);

    if (!IsValid(PersonajeLocal))
    {
        CartelRecoger->SetVisibility(false);
        return;
    }

    UReclaimArmaComponent* ComponenteArma =
        PersonajeLocal->FindComponentByClass<UReclaimArmaComponent>();
    
    const bool bEstaEsMiArma =
        IsValid(ComponenteArma) &&
        ComponenteArma->TieneArma() &&
        ComponenteArma->ObtenerIdArma() == IdArma;

    ModeloArma->SetVisibility(!bEstaEsMiArma);
    
    if (!IsValid(ComponenteArma) || ComponenteArma->TieneArma())
    {
        CartelRecoger->SetVisibility(false);
        return;
    }

    const float DistanciaMaxima = 250.0f;

    const float DistanciaCuadrada = FVector::DistSquared(
        PersonajeLocal->GetActorLocation(),
        GetActorLocation()
    );

    CartelRecoger->SetVisibility(
        DistanciaCuadrada <= FMath::Square(DistanciaMaxima)
    );
}
bool AReclaimArmaRecogible::EstaDisponibleEnMision() const
{
    const UWorld* Mundo = GetWorld();

    if (!Mundo)
    {
        return false;
    }

    AGameStateBase* EstadoJuego = Mundo->GetGameState();

    if (!IsValid(EstadoJuego))
    {
        return false;
    }

    UReclaimQuestComponent* Quest =
        EstadoJuego->FindComponentByClass<UReclaimQuestComponent>();

    if (!IsValid(Quest))
    {
        return false;
    }

    for (const FEstadoQuest& Estado : Quest->ObtenerMisiones())
    {
        if (IsValid(Estado.DatosMision) &&
            Estado.DatosMision->IdMision ==
                FName(TEXT("Mision_ConseguirArma")))
        {
            return true;
        }
    }

    return false;
}

