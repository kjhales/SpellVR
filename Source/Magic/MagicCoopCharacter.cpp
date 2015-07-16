#include "Magic.h"
#include "MagicCoopCharacter.h"
#include "SpellSystem.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMagicCoopCharacter

AMagicCoopCharacter::AMagicCoopCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 40.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("CharacterMesh1P"));
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bOnlyOwnerSee = true;		// only the owning player will see this mesh
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	/** Set Player unable to see their 3rd person body**/
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SpellSystemChildComponent = ObjectInitializer.CreateDefaultSubobject<UChildActorComponent>(this, TEXT("SpellSystemComponent"));
	SpellSystemChildComponent->SetChildActorClass(ASpellSystem::StaticClass());
	SpellSystemChildComponent->AttachParent = RootComponent;
}

void AMagicCoopCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (SpellSystem == NULL)
	{
		SpellSystemChildComponent->CreateChildActor();
		SpellSystem = Cast<ASpellSystem>(SpellSystemChildComponent->ChildActor);
		SpellSystem->SetOwningPawn(this);
		SpellSystem->AddSpell(0);
	}
}
//////////////////////////////////////////////////////////////////////////
// Input
void AMagicCoopCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AMagicCoopCharacter::OnFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AMagicCoopCharacter::OnRelease);

	InputComponent->BindAxis("MoveForward", this, &AMagicCoopCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMagicCoopCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AMagicCoopCharacter::TurnAtRate);

	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AMagicCoopCharacter::LookUpAtRate);
}

void AMagicCoopCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicCoopCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicCoopCharacter::OnRelease()
{
	SpellSystem->ReleaseSpell();
}

void AMagicCoopCharacter::OnFire()
{
	if (SpellSystem)
	{
		SpellSystem->CastSpell(Mesh1P, GetActorRotation(), GunOffset, NULL, false);
	}
}

void AMagicCoopCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMagicCoopCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMagicCoopCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMagicCoopCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
