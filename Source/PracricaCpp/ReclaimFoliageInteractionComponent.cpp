#include "ReclaimFoliageInteractionComponent.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "ReclaimFoliageManager.h"

UReclaimFoliageInteractionComponent::UReclaimFoliageInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UReclaimFoliageInteractionComponent::SolicitarGolpeFoliage()
{
    if (!GetOwner())
    {
        return;
    }

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
        ClienteRecibirResultadoFoliage(false, -3);
        return;
    }

    UStaticMesh* MallaGolpeada = ComponenteFoliage->GetStaticMesh();

    if (!MallaGolpeada || !MallasTalables.Contains(MallaGolpeada))
    {
        ClienteRecibirResultadoFoliage(false, -6);
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

    if (GolpesAcumulados <= 0)
    {
        ClienteRecibirResultadoFoliage(false, 0);
        return;
    }

    // A partir del cuarto golpe, el servidor intenta talar el árbol.
    if (GolpesAcumulados >= 4)
    {
        const bool bTalado = Administrador->RegistrarTala(
            ComponenteFoliage,
            Impacto.Item
        );

        if (!bTalado)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Reclaim: no se pudo registrar la tala del foliage.")
            );

            ClienteRecibirResultadoFoliage(false, -7);
            return;
        }
    }

    // Se ejecuta en el servidor. Lo conectaremos al inventario.
    OnGolpeFoliageValidado.Broadcast(GolpesAcumulados);

    // Devuelve el resultado al jugador que golpeó.
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

