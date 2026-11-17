// Este include debe ser el primero.
#include "PracricaCppCharacter.h"

#include "ShopActor.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Habilidades/ReclaimHabilidadWallRunComponent.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PracricaCpp.h"


APracricaCppCharacter::APracricaCppCharacter()
{
    // Tamaño de la cápsula de colisión.
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // La cámara puede rotar sin rotar directamente al personaje.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configuración de movimiento.
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 500.0f, 0.0f);

    GetCharacterMovement()->JumpZVelocity = 500.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Brazo de cámara.
    CameraBoom =
        CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Cámara.
    FollowCamera =
        CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

    FollowCamera->SetupAttachment(
        CameraBoom,
        USpringArmComponent::SocketName
    );

    FollowCamera->bUsePawnControlRotation = false;
}


void APracricaCppCharacter::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent
)
{
    // Tecla 1 de la fila superior del teclado.
    PlayerInputComponent->BindKey(
        EKeys::One,
        IE_Pressed,
        this,
        &APracricaCppCharacter::ActivarHabilidadWallRun
    );

    if (UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Salto.
        EnhancedInputComponent->BindAction(
            JumpAction,
            ETriggerEvent::Started,
            this,
            &ACharacter::Jump
        );

        EnhancedInputComponent->BindAction(
            JumpAction,
            ETriggerEvent::Completed,
            this,
            &ACharacter::StopJumping
        );

        // Movimiento.
        EnhancedInputComponent->BindAction(
            MoveAction,
            ETriggerEvent::Triggered,
            this,
            &APracricaCppCharacter::Move
        );

        EnhancedInputComponent->BindAction(
            MouseLookAction,
            ETriggerEvent::Triggered,
            this,
            &APracricaCppCharacter::Look
        );

        // Cámara.
        EnhancedInputComponent->BindAction(
            LookAction,
            ETriggerEvent::Triggered,
            this,
            &APracricaCppCharacter::Look
        );

        // Interacción.
        EnhancedInputComponent->BindAction(
            InteractAction,
            ETriggerEvent::Started,
            this,
            &APracricaCppCharacter::Interact
        );
    }
    else
    {
        UE_LOG(
            LogPracricaCpp,
            Error,
            TEXT("'%s' Failed to find an Enhanced Input component! "
                 "This template is built to use the Enhanced Input system. "
                 "If you intend to use the legacy system, then you will need "
                 "to update this C++ file."),
            *GetNameSafe(this)
        );
    }
}


void APracricaCppCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    DoMove(MovementVector.X, MovementVector.Y);
}


void APracricaCppCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    DoLook(LookAxisVector.X, LookAxisVector.Y);
}


void APracricaCppCharacter::DoMove(float Right, float Forward)
{
    if (GetController() != nullptr)
    {
        const FRotator Rotation = GetController()->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection =
            FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        const FVector RightDirection =
            FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        AddMovementInput(ForwardDirection, Forward);
        AddMovementInput(RightDirection, Right);
    }
}


void APracricaCppCharacter::DoLook(float Yaw, float Pitch)
{
    if (GetController() != nullptr)
    {
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}


void APracricaCppCharacter::DoJumpStart()
{
    Jump();
}


void APracricaCppCharacter::DoJumpEnd()
{
    StopJumping();
}


void APracricaCppCharacter::ActivarHabilidadWallRun()
{
    UReclaimHabilidadWallRunComponent* Habilidad =
        FindComponentByClass<UReclaimHabilidadWallRunComponent>();

    if (IsValid(Habilidad))
    {
        Habilidad->SolicitarActivarWallRun();
    }
}


void APracricaCppCharacter::CGanarExperiencia(float CExpObtenida)
{
    CExperienciaActual += CExpObtenida;

    bool bSubioDeNivel = false;

    while (CExperienciaActual >= CExperienciaRequerida)
    {
        bSubioDeNivel = true;

        CNivelActual++;

        CExperienciaActual -= CExperienciaRequerida;

        CExperienciaRequerida +=
            (CExperienciaRequerida / 10.0f);
    }

    if (bSubioDeNivel)
    {
        const FString MensajeVictoria = FString::Printf(
            TEXT("FELICIDADES\n")
            TEXT("Subiste al nivel:\n%d\n")
            TEXT("Proximo Nivel: %d\n")
            TEXT("(Requiere %.1f EXP)\n")
            TEXT("EXP Actual: %.1f"),
            static_cast<int32>(CNivelActual),
            static_cast<int32>(CNivelActual) + 1,
            CExperienciaRequerida,
            CExperienciaActual
        );

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                10.0f,
                FColor::Magenta,
                MensajeVictoria
            );
        }
    }
    else
    {
        const FString MensajeCorto = FString::Printf(
            TEXT("Experiencia Actual:\n%.1f"),
            CExperienciaActual
        );

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Cyan,
                MensajeCorto
            );
        }
    }
}


void APracricaCppCharacter::Interact()
{
    const FVector Start =
        GetActorLocation() + FVector(0.f, 0.f, 60.f);

    const FVector End =
        Start + (GetControlRotation().Vector() * 300.f);

    FHitResult HitResult;
    TArray<AActor*> ActorsToIgnore;

    UKismetSystemLibrary::LineTraceSingle(
        this,
        Start,
        End,
        ETraceTypeQuery::TraceTypeQuery1,
        true,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResult,
        true
    );

    if (AActor* HitActor = HitResult.GetActor())
    {
        if (AShopActor* Shop = Cast<AShopActor>(HitActor))
        {
            Shop->Interact();
            return;
        }

        if (HitActor->GetClass()->ImplementsInterface(
            UComunicacionPuzzle::StaticClass()))
        {
            IComunicacionPuzzle::Execute_ComunicarPuzzle(
                HitActor,
                TengoLaLlave
            );

            return;
        }
    }

    // Mensaje temporal existente para la interacción.
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            1.f,
            FColor::Red,
            TEXT("No hay nada interactuable frente a ti.")
        );
    }
}


void APracricaCppCharacter::ComunicarPuzzle_Implementation(bool llave)
{
    TengoLaLlave = llave;
}


void APracricaCppCharacter::InformacionColor_Implementation(
    const FString& Color,
    bool ColorActivado
)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            10.f,
            FColor::Cyan,
            Color
        );
    }
}

