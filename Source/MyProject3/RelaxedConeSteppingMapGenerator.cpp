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
	UCanvas *canvases[2];
	canvases[0] = NewObject<UCanvas>(GetTransientPackage(), NAME_None);
	canvases[1] = NewObject<UCanvas>(GetTransientPackage(), NAME_None);

	UTextureRenderTarget2D* tempRT = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), 256, 256, RTF_R16f);
	auto material = UMaterialInstanceDynamic::Create(m_material, nullptr);
	material->SetVectorParameterValue(FName(TEXT("TexelSize")), FLinearColor(1.0f / m_reliefMap->GetSizeX(), 1.0f / m_reliefMap->GetSizeY(), m_reliefMap->GetSizeX(), 0));
	material->SetTextureParameterValue(FName(TEXT("ReliefMap")), m_reliefMap);

	FDrawToRenderTargetContext contexts[2];
	FVector2D size;

	int texelsPerRow = m_reliefMap->GetSizeX();
	int rowCount = m_reliefMap->GetSizeY();
	int batchCount = rowCount / m_rowsPerDrawCall;
	int remainingRowCount = rowCount % m_rowsPerDrawCall;


	UTextureRenderTarget2D* rTs[2];
	//Make sure the last batch's result is stored in m_textureRenderTarget 
	if((batchCount + (remainingRowCount == 0 ? 0 : 1)) % 2 == 0)
	{
		rTs[0] = m_textureRenderTarget;
		rTs[1] = tempRT;
	}
	else
	{
		rTs[0] = tempRT;
		rTs[1] = m_textureRenderTarget;
	}

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), rTs[0], FLinearColor(FColor(1, 1, 1, 1)));
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), rTs[1], FLinearColor(FColor(1, 1, 1, 1)));
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), rTs[0], canvases[0], size, contexts[0]);
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), rTs[1], canvases[1], size, contexts[1]);

	material->SetScalarParameterValue(FName(TEXT("RowCount")), rowCount);
	int i = 0;
	for(; i < batchCount; i++)
	{
		material->SetTextureParameterValue(FName(TEXT("SteppingConeMap")), rTs[static_cast<int>(i % 2 == 0)]);
		material->SetScalarParameterValue(FName(TEXT("StartingRowIndex")), i * m_rowsPerDrawCall);
		canvases[i % 2]->K2_DrawMaterial(material, FVector2D(0, 0), FVector2D(size.X, size.Y), FVector2D(0, 0));
	}
	if(remainingRowCount != 0)
	{
		material->SetTextureParameterValue(FName(TEXT("SteppingConeMap")), rTs[static_cast<int>(i % 2 == 0)]);
		material->SetScalarParameterValue(FName(TEXT("StartingRowIndex")), i * m_rowsPerDrawCall);
		material->SetScalarParameterValue(FName(TEXT("RowCount")), remainingRowCount);
		canvases[i % 2]->K2_DrawMaterial(material, FVector2D(0, 0), FVector2D(size.X, size.Y), FVector2D(0, 0));
	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), contexts[0]);
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), contexts[1]);
//	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), m_textureRenderTarget, material);
}

// Called every frame
void ARelaxedConeSteppingMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

