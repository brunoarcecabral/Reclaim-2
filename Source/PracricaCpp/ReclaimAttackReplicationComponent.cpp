#include "ReclaimAttackReplicationComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

UReclaimAttackReplicationComponent::UReclaimAttackReplicationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);
}

void UReclaimAttackReplicationComponent::CompartirAnimacionAtaque(
    UAnimMontage* Montaje
)
{
    if (!Montaje || !GetOwner())
    {
        return;
    }

    if (GetOwner()->HasAuthority())
    {
        MulticastReproducirAtaque(Montaje);
    }
    else
    {
        ServidorCompartirAtaque(Montaje);
    }
}

void UReclaimAttackReplicationComponent::
ServidorCompartirAtaque_Implementation(UAnimMontage* Montaje)
{
    if (!Montaje)
    {
        return;
    }

    MulticastReproducirAtaque(Montaje);
}

void UReclaimAttackReplicationComponent::
MulticastReproducirAtaque_Implementation(UAnimMontage* Montaje)
{
    if (!Montaje)
    {
        return;
    }

    ACharacter* Personaje = Cast<ACharacter>(GetOwner());

    if (!Personaje)
    {
        return;
    }

    // El jugador que ataca ya reproduce el montaje localmente.
    if (Personaje->IsLocallyControlled())
    {
        return;
    }

    USkeletalMeshComponent* MeshPersonaje = Personaje->GetMesh();

    if (!MeshPersonaje)
    {
        return;
    }

    UAnimInstance* AnimInstance = MeshPersonaje->GetAnimInstance();

    if (!AnimInstance)
    {
        return;
    }

    AnimInstance->Montage_Play(Montaje);
}

void UReclaimAttackReplicationComponent::SolicitarAtaqueAlServidor()
{
    if (!GetOwner())
    {
        return;
    }

    ServidorProcesarAtaque();
}

void UReclaimAttackReplicationComponent::ServidorProcesarAtaque_Implementation()
{
    OnAtaqueEnServidor.Broadcast();
}

void UReclaimAttackReplicationComponent::AvisarMaderaAlJugador()
{
    // Esta función solo debe ser llamada por el servidor.
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    ClienteMostrarAvisoMadera();
}

void UReclaimAttackReplicationComponent::
ClienteMostrarAvisoMadera_Implementation()
{
    OnAvisoMadera.Broadcast();
}

