// Este include SIEMPRE tiene que ser el primero en Unreal
#include "PracricaCppCharacter.h"

// Recien despues vienen los demas includes
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PracricaCpp.h"


APracricaCppCharacter::APracricaCppCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	

}

void APracricaCppCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APracricaCppCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &APracricaCppCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APracricaCppCharacter::Look);
		
		// Interaccion (se llama Interact() cuando se presiona la tecla)
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APracricaCppCharacter::Interact);
	}
	else
	{
		UE_LOG(LogPracricaCpp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APracricaCppCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void APracricaCppCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void APracricaCppCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void APracricaCppCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void APracricaCppCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void APracricaCppCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}


void APracricaCppCharacter::CGanarExperiencia(float CExpObtenida)
{
	// 1. Sumamos la experiencia obtenida a la actual
	CExperienciaActual += CExpObtenida;

	// Usamos una variable temporal para saber si tenemos que tirar el mensaje de victoria
	bool bSubioDeNivel = false;

	// 2. Evaluamos si alcanza para subir de nivel (El while nos protege por si gana mucha exp de golpe)
	while (CExperienciaActual >= CExperienciaRequerida)
	{
		bSubioDeNivel = true;

		// Aumentamos el nivel
		CNivelActual++; 

		// Restamos la experiencia que "costó" subir de nivel
		CExperienciaActual -= CExperienciaRequerida;

		// Aumentamos la exp requerida en un 10% (agregamos la 'f' al final de 10.0 para que Unreal sepa que es float)
		CExperienciaRequerida += (CExperienciaRequerida / 10.0f);
	}

	// 3. Los Print Strings
	if (bSubioDeNivel)
	{
		// Armamos el super mensaje de victoria
		// Ojo: Usamos (int32) delante de CNivelActual para esconderle los decimales en el texto.
		// Además, le sumo 1 al segundo CNivelActual para que te muestre correctamente cuál es tu PRÓXIMO nivel.
		FString MensajeVictoria = FString::Printf(TEXT("FELICIDADES\nSubiste al nivel:\n%d\nProximo Nivel: %d\n(Requiere %.1f EXP)\nEXP Actual: %.1f"), 
			(int32)CNivelActual, (int32)CNivelActual + 1, CExperienciaRequerida, CExperienciaActual);

		if (GEngine)
		{
			// FColor::Cyan es ese turquesa que tenías, dura 10 segundos
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, MensajeVictoria);
		}
	}
	else
	{
		// Armamos el mensaje corto de cuando no alcanza para subir
		FString MensajeCorto = FString::Printf(TEXT("Experiencia Actual:\n%.1f"), CExperienciaActual);

		if (GEngine)
		{
			// Dura 2 segundos
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, MensajeCorto);
		}
	}
}
void APracricaCppCharacter::Interact()
{
	// Por ahora vacía, la lógica la manejamos en Blueprint
};