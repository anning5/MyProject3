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
	UCanvas *canvasForTempRT = NewObject<UCanvas>(GetTransientPackage(), NAME_None);
	UCanvas *canvasForResultRT = NewObject<UCanvas>(GetTransientPackage(), NAME_None);

	UTextureRenderTarget2D* tempRT = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 256, 256, RTF_R16f);
	auto material = UMaterialInstanceDynamic::Create(m_material, nullptr);
	material->SetVectorParameterValue(FName(TEXT("TexelSize")), FLinearColor(1.0f / m_reliefMap->GetSizeX(), 1.0f / m_reliefMap->GetSizeY(), m_reliefMap->GetSizeX(), m_reliefMap->GetSizeY()));

	FDrawToRenderTargetContext tempRTContext, resultRTContext;
	FVector2D size;

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), m_textureRenderTarget, FLinearColor(FColor(1, 1, 1, 1)));
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), tempRT, FLinearColor(FColor(1, 1, 1, 1)));
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), tempRT, canvasForTempRT, size, tempRTContext);
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), m_textureRenderTarget, canvasForResultRT, size, resultRTContext);

	int texelsPerRow = m_reliefMap->GetSizeX();
	int rowCount = m_reliefMap->GetSizeY();
	int batchCount = rowCount / m_rowsPerDrawCall;
	int remainingRowCount = rowCount % m_rowsPerDrawCall;
	for(int i = 0; i < batchCount; i++)
	{
		material->SetTextureParameterValue(FName(TEXT("ReliefMap")), m_reliefMap);
		material->SetTextureParameterValue(FName(TEXT("SteppingConeMap")), i % 2 == 0 ? tempRT : m_textureRenderTarget);
		(i % 2 == 0 ? canvasForResultRT : canvasForTempRT)->K2_DrawMaterial(material, FVector2D(0, 0), FVector2D(size.X, size.Y), FVector2D(0, 0));
	}
//	for(int i = 0; i < remainingRowCount; i++)
//	{
//	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), tempRTContext);
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), resultRTContext);
//	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_textureRenderTarget, material);
}

// Called every frame
void ARelaxedConeSteppingMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

