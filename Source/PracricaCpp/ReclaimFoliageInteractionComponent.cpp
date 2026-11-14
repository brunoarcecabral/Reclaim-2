#include "ReclaimFoliageInteractionComponent.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "ReclaimFoliageManager.h"

UReclaimFoliageInteractionComponent::UReclaimFoliageInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Permite que el componente utilice llamadas de red.
    SetIsReplicatedByDefault(true);
}

void UReclaimFoliageInteractionComponent::SolicitarGolpeFoliage()
{
    if (!GetOwner())
    {
        return;
    }

    // Si ya estamos en el servidor, se ejecuta allí.
    // Si somos un cliente, se envía la solicitud al servidor.
    ServidorRegistrarGolpeFoliage();
}

void UReclaimFoliageInteractionComponent::
ServidorRegistrarGolpeFoliage_Implementation()
{
    AActor* Personaje = GetOwner();
    UWorld* Mundo = GetWorld();

    if (!Personaje || !Mundo)
    {
        return;
    }

    AReclaimFoliageManager* Administrador =
        Cast<AReclaimFoliageManager>(
            UGameplayStatics::GetActorOfClass(
                Mundo,
                AReclaimFoliageManager::StaticClass()
            )
        );

    if (!Administrador)
    {
        ClienteRecibirResultadoFoliage(false, -1);
        return;
    }

    
    // inicio a 100 unidades de altura y alcance de 120.
    const FVector UbicacionPersonaje = Personaje->GetActorLocation();

    const FVector Inicio =
        UbicacionPersonaje + FVector(0.0, 0.0, 100.0);

    const FVector Fin =
        UbicacionPersonaje +
        Personaje->GetActorForwardVector() * 120.0;

    FHitResult Impacto;

    FCollisionQueryParams Parametros;
    Parametros.AddIgnoredActor(Personaje);

    const bool bHuboImpacto = Mundo->LineTraceSingleByChannel(
        Impacto,
        Inicio,
        Fin,
        ECC_Visibility,
        Parametros
    );

    if (!bHuboImpacto)
    {
        ClienteRecibirResultadoFoliage(false, -2);
        return;
    }

    UHierarchicalInstancedStaticMeshComponent* ComponenteFoliage =
        Cast<UHierarchicalInstancedStaticMeshComponent>(
            Impacto.GetComponent()
        );

    if (!ComponenteFoliage)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Foliage: actor=%s | componente=%s | clase=%s"),
            *GetNameSafe(Impacto.GetActor()),
            *GetNameSafe(Impacto.GetComponent()),
            *GetNameSafe(
                Impacto.GetComponent()
                    ? Impacto.GetComponent()->GetClass()
                    : nullptr
            )
        );

        ClienteRecibirResultadoFoliage(false, -3);
        return;
    }



    if (Impacto.Item == INDEX_NONE)
    {
        ClienteRecibirResultadoFoliage(false, -5);
        return;
    }



    FTransform TransformInstancia;

    const bool bTransformValido =
        ComponenteFoliage->GetInstanceTransform(
            Impacto.Item,
            TransformInstancia,
            true
        );

    if (!bTransformValido)
    {
        ClienteRecibirResultadoFoliage(false, -4);
        return;
    }

    const int32 GolpesAcumulados =
        Administrador->RegistrarGolpe(
            TransformInstancia.GetLocation()
        );

    ClienteRecibirResultadoFoliage(true, GolpesAcumulados);
}

void UReclaimFoliageInteractionComponent::
ClienteRecibirResultadoFoliage_Implementation(
    bool bDetectado,
    int32 GolpesAcumulados
)
{
    OnResultadoGolpeFoliage.Broadcast(
        bDetectado,
        GolpesAcumulados
    );
}

