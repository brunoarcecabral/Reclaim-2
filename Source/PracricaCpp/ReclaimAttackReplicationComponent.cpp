#include "ReclaimAttackReplicationComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

UReclaimAttackReplicationComponent::UReclaimAttackReplicationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Permite las llamadas de red del componente.
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

void UReclaimAttackReplicationComponent::ServidorCompartirAtaque_Implementation(
    UAnimMontage* Montaje
)
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

    // En el cliente envía la solicitud. En el servidor la ejecuta allí.
    ServidorProcesarAtaque();
}

void UReclaimAttackReplicationComponent::ServidorProcesarAtaque_Implementation()
{
    // Los Blueprints que escuchen este evento se ejecutarán en el servidor.
    OnAtaqueEnServidor.Broadcast();
}

