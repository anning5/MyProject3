// Fill out your copyright notice in the Description page of Project Settings.


#include "RelaxedConeSteppingMapGenerator.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ARelaxedConeSteppingMapGenerator::ARelaxedConeSteppingMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARelaxedConeSteppingMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	UCanvas *canvasForDrawMaterialToRenderTarget = NewObject<UCanvas>(GetTransientPackage(), NAME_None);

	UTextureRenderTarget2D* tempRT = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 256, 256, RTF_R16f);
	auto material = UMaterialInstanceDynamic::Create(m_material, nullptr);

	FDrawToRenderTargetContext context;
	FVector2D size;

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_textureRenderTarget, FLinearColor(FColor(0, 0, 0, 0)));
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), tempRT, FLinearColor(FColor(0, 0, 0, 0)));
	for(int i = 1; i < m_loopCount; i++)
	{
		material->SetTextureParameterValue(FName(TEXT("Tex")), i % 2 == 0 ?  tempRT : m_textureRenderTarget);
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), i % 2 == 0 ? m_textureRenderTarget : tempRT, canvasForDrawMaterialToRenderTarget, size, context);
		canvasForDrawMaterialToRenderTarget->K2_DrawMaterial(material, FVector2D(0, 0), FVector2D(size.X, size.Y), FVector2D(0, 0));
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), context);
	}
//	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_textureRenderTarget, material);
}

// Called every frame
void ARelaxedConeSteppingMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

