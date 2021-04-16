#include "GAL/DX12/DX12Pipeline.h"

#include "GAL/DX12/DX12RenderDevice.h"
#include "GAL/RenderCore.h"
#include "GTSL/Array.hpp"
#include "GTSL/Buffer.hpp"

void GAL::DX12PipelineLayout::Initialize(const CreateInfo& info)
{
	GTSL::Array<D3D12_ROOT_PARAMETER, 12> rootParameters;

	for(GTSL::uint32 i = 0; i < info.BindingsDescriptors.ElementCount(); ++i)
	{
		D3D12_ROOT_PARAMETER rootParameter;
		rootParameter.ParameterType;
		rootParameter.ShaderVisibility;
		rootParameter.Constants;
		rootParameter.Descriptor;
		rootParameter.DescriptorTable;
		rootParameters.EmplaceBack(rootParameter);
	}
	
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	rootSignatureDesc.NumParameters = rootParameters.GetLength();
	rootSignatureDesc.pParameters = rootParameters.begin();
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	DX_CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, nullptr, nullptr))
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreateRootSignature(0, nullptr, 0, __uuidof(ID3D12RootSignature), nullptr))
	setName(rootSignature, info);
}

void GAL::DX12PipelineLayout::Destroy(const DX12RenderDevice* renderDevice)
{
	rootSignature->Release();
	debugClear(rootSignature);
}

DXGI_FORMAT ShaderDataTypeToDXGI_FORMAT(GAL::ShaderDataType type)
{
	switch (type)
	{
		case GAL::ShaderDataType::FLOAT:  break;
		case GAL::ShaderDataType::FLOAT2: break;
		case GAL::ShaderDataType::FLOAT3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case GAL::ShaderDataType::FLOAT4: break;
		case GAL::ShaderDataType::INT:	break;
		case GAL::ShaderDataType::INT2: break;
		case GAL::ShaderDataType::INT3: break;
		case GAL::ShaderDataType::INT4: break;
		case GAL::ShaderDataType::BOOL: break;
		case GAL::ShaderDataType::MAT3: break;
		case GAL::ShaderDataType::MAT4: break;
	default: ;
	}
}

void GAL::DX12RasterPipeline::Initialize(const CreateInfo& info)
{	
	GTSL::Buffer<GTSL::StackAllocator<1024>> buffer;
	//buffer.Allocate(1024, 8, allocator);

	GTSL::Array<D3D12_INPUT_ELEMENT_DESC, 12> vertexElements;
	
	D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type;
	
	{
		{
			for (GTSL::uint32 i = 0; i < info.Stages.ElementCount(); ++i)
			{
				switch (info.Stages[i].Type)
				{
				case ShaderType::VERTEX_SHADER: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS; break;
				case ShaderType::TESSELLATION_CONTROL_SHADER: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS; break;
				case ShaderType::TESSELLATION_EVALUATION_SHADER: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS; break;
				case ShaderType::GEOMETRY_SHADER: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS; break;
				case ShaderType::FRAGMENT_SHADER: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS; break;
				case ShaderType::COMPUTE_SHADER: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS; break;
				default: type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MAX_VALID;
				}

				buffer.CopyBytes(sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE), reinterpret_cast<const byte*>(&type));
				D3D12_SHADER_BYTECODE bytecode;
				bytecode.BytecodeLength = info.Stages[i].ShaderData.ElementCount();
				bytecode.pShaderBytecode = info.Stages[i].ShaderData.begin();
				buffer.CopyBytes(sizeof(D3D12_SHADER_BYTECODE), reinterpret_cast<const byte*>(&bytecode));
			}
		}

		{
			type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;
			buffer.CopyBytes(sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE), reinterpret_cast<const byte*>(&type));

			D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
			inputLayoutDesc.NumElements = static_cast<UINT32>(info.VertexDescriptor.ElementCount());

			GTSL::uint32 offset = 0;

			for (GTSL::uint32 i = 0; i < inputLayoutDesc.NumElements; ++i)
			{
				
				D3D12_INPUT_ELEMENT_DESC elementDesc;
				elementDesc.Format = ShaderDataTypeToDXGI_FORMAT(info.VertexDescriptor[i].Type);
				elementDesc.AlignedByteOffset = offset;
				elementDesc.SemanticIndex = 0;
				elementDesc.InputSlot = i;
				elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				elementDesc.InstanceDataStepRate = 0;

				offset += ShaderDataTypesSize(info.VertexDescriptor[i].Type);

				elementDesc.SemanticName = info.VertexDescriptor[i].Identifier.begin();
				vertexElements.EmplaceBack(elementDesc);
			}

			inputLayoutDesc.pInputElementDescs = vertexElements.begin();
			buffer.CopyBytes(sizeof(D3D12_INPUT_LAYOUT_DESC), reinterpret_cast<const byte*>(&inputLayoutDesc));
		}
	}
	
	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStream;
	pipelineStateStream.SizeInBytes = buffer.GetLength();
	pipelineStateStream.pPipelineStateSubobjectStream = buffer.GetData();
	
	info.RenderDevice->GetID3D12Device2()->CreatePipelineState(&pipelineStateStream, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(&pipelineState));
	setName(pipelineState, info);

	//buffer.Free(8, allocator);
}

void GAL::DX12RasterPipeline::Destroy(const DX12RenderDevice* renderDevice)
{
	pipelineState->Release();
	debugClear(pipelineState);
}
