// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "VertexShaders/ST_BoidsDrawerVS.h"
#include "CommonRenderResources.h"
#include "Containers/DynamicRHIResourceArray.h"

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FST_SimplePassThroughVS, "/ComputeCore/Private/ComputeRPExample/VS_BoidsTexture.usf", "MainVertexShader", SF_Vertex);

void FST_SimpleScreenVertexBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
	TResourceArray<FFilterVertex, VERTEXBUFFER_ALIGNMENT> Vertices;
	Vertices.SetNumUninitialized(6);

	Vertices[0].Position = FVector4f(-1, 1, 0, 1);
	Vertices[0].UV = FVector2f(0, 0);

	Vertices[1].Position = FVector4f(1, 1, 0, 1);
	Vertices[1].UV = FVector2f(1, 0);

	Vertices[2].Position = FVector4f(-1, -1, 0, 1);
	Vertices[2].UV = FVector2f(0, 1);

	Vertices[3].Position = FVector4f(1, -1, 0, 1);
	Vertices[3].UV = FVector2f(1, 1);

	// Create vertex buffer. Fill buffer with initial data upon creation
	const FRHIBufferCreateDesc CreateDesc =
		FRHIBufferCreateDesc::CreateVertex(TEXT("ShaderDemoSquare"), Vertices.GetResourceDataSize())
		.AddUsage(BUF_Static)
		.SetInitActionResourceArray(&Vertices)
		.DetermineInitialState();

	VertexBufferRHI = RHICmdList.CreateBuffer(CreateDesc);
}

bool FST_SimplePassThroughVS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}