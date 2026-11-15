#include "ReclaimFoliageManager.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/UObjectIterator.h"

AReclaimFoliageManager::AReclaimFoliageManager()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    bAlwaysRelevant = true;
}

void AReclaimFoliageManager::BeginPlay()
{
    Super::BeginPlay();

    // En clientes, vuelve a intentar aplicar el estado por si una
    // zona de World Partition todavía no estaba cargada al recibirlo.
    if (!HasAuthority() && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            TemporizadorAplicarTalados,
            this,
            &AReclaimFoliageManager::AplicarArbolesTalados,
            2.0f,
            true
        );
    }
}

void AReclaimFoliageManager::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(
            TemporizadorAplicarTalados
        );
    }

    Super::EndPlay(EndPlayReason);
}

void AReclaimFoliageManager::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReclaimFoliageManager, EstadosFoliage);
    DOREPLIFETIME(AReclaimFoliageManager, ArbolesTalados);
}

int32 AReclaimFoliageManager::BuscarIndice(FVector Ubicacion) const
{
    for (int32 Indice = 0; Indice < EstadosFoliage.Num(); ++Indice)
    {
        if (EstadosFoliage[Indice].Ubicacion.Equals(Ubicacion, 1.0))
        {
            return Indice;
        }
    }

    return INDEX_NONE;
}

int32 AReclaimFoliageManager::RegistrarGolpe(FVector Ubicacion)
{
    if (!HasAuthority())
    {
        return 0;
    }

    int32 Indice = BuscarIndice(Ubicacion);

    if (Indice == INDEX_NONE)
    {
        FReclaimFoliageEstado NuevoEstado;
        NuevoEstado.Ubicacion = Ubicacion;
        NuevoEstado.Golpes = 0;

        Indice = EstadosFoliage.Add(NuevoEstado);
    }

    EstadosFoliage[Indice].Golpes += 1;

    ForceNetUpdate();

    return EstadosFoliage[Indice].Golpes;
}

int32 AReclaimFoliageManager::ObtenerGolpes(FVector Ubicacion) const
{
    const int32 Indice = BuscarIndice(Ubicacion);

    if (Indice == INDEX_NONE)
    {
        return 0;
    }

    return EstadosFoliage[Indice].Golpes;
}

bool AReclaimFoliageManager::YaEstaTalado(
    const FString& RutaMalla,
    const FVector& Ubicacion
) const
{
    for (const FReclaimFoliageTalado& Arbol : ArbolesTalados)
    {
        if (
            Arbol.RutaMalla == RutaMalla &&
            Arbol.Ubicacion.Equals(Ubicacion, 1.0)
        )
        {
            return true;
        }
    }

    return false;
}

bool AReclaimFoliageManager::RegistrarTala(
    UHierarchicalInstancedStaticMeshComponent* Componente,
    int32 IndiceInstancia
)
{
    if (!HasAuthority() || !IsValid(Componente))
    {
        return false;
    }

    UStaticMesh* Malla = Componente->GetStaticMesh();

    if (!Malla)
    {
        return false;
    }

    FTransform TransformInstancia;

    if (
        !Componente->GetInstanceTransform(
            IndiceInstancia,
            TransformInstancia,
            true
        )
    )
    {
        return false;
    }

    const FVector Ubicacion = TransformInstancia.GetLocation();
    const FString RutaMalla = Malla->GetPathName();

    if (YaEstaTalado(RutaMalla, Ubicacion))
    {
        return false;
    }

    // Primero eliminamos la instancia del mundo del servidor.
    if (!Componente->RemoveInstance(IndiceInstancia))
    {
        return false;
    }

    // Después guardamos el estado para que llegue a los clientes.
    FReclaimFoliageTalado NuevoTalado;
    NuevoTalado.Ubicacion = Ubicacion;
    NuevoTalado.RutaMalla = RutaMalla;

    ArbolesTalados.Add(NuevoTalado);
    ForceNetUpdate();

    return true;
}

void AReclaimFoliageManager::OnRep_ArbolesTalados()
{
    AplicarArbolesTalados();
}

void AReclaimFoliageManager::AplicarArbolesTalados()
{
    UWorld* Mundo = GetWorld();

    if (!Mundo || ArbolesTalados.IsEmpty())
    {
        return;
    }

    for (
        TObjectIterator<UHierarchicalInstancedStaticMeshComponent> It;
        It;
        ++It
    )
    {
        UHierarchicalInstancedStaticMeshComponent* Componente = *It;

        if (
            !IsValid(Componente) ||
            !Componente->IsRegistered() ||
            Componente->GetWorld() != Mundo
        )
        {
            continue;
        }

        UStaticMesh* Malla = Componente->GetStaticMesh();

        if (!Malla)
        {
            continue;
        }

        const FString RutaMalla = Malla->GetPathName();

        for (const FReclaimFoliageTalado& Arbol : ArbolesTalados)
        {
            if (Arbol.RutaMalla != RutaMalla)
            {
                continue;
            }

            // Busca candidatos cercanos en espacio de mundo.
            const TArray<int32> Candidatos =
                Componente->GetInstancesOverlappingSphere(
                    Arbol.Ubicacion,
                    10.0f,
                    true
                );

            for (const int32 Indice : Candidatos)
            {
                FTransform TransformInstancia;

                if (
                    Componente->GetInstanceTransform(
                        Indice,
                        TransformInstancia,
                        true
                    ) &&
                    TransformInstancia.GetLocation().Equals(
                        Arbol.Ubicacion,
                        1.0
                    )
                )
                {
                    Componente->RemoveInstance(Indice);
                    break;
                }
            }
        }
    }
}

