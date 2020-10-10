#include "GAL/DX12/DX12Pipeline.h"

#include "GAL/DX12/DX12RenderDevice.h"
#include "GAL/RenderCore.h"
#include "GTSL/Array.hpp"
#include "GTSL/Buffer.h"

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
	DX_CHECK(info.RenderDevice->GetID3D12Device2()->CreateRootSignature(0, nullptr, 0, IID(), nullptr))
	setName(rootSignature, info);
}

void GAL::DX12PipelineLayout::Destroy(const DX12RenderDevice* renderDevice)
{
	rootSignature->Release();
	debugClear(rootSignature);
}

void GAL::DX12RasterPipeline::Initialize(const CreateInfo& info)
{
	GTSL::StackAllocator<1024> allocator;
	
	GTSL::Buffer buffer;
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

				buffer.WriteBytes(sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE), reinterpret_cast<const byte*>(&type));
				D3D12_SHADER_BYTECODE bytecode;
				bytecode.BytecodeLength = info.Stages[i].ShaderData.ElementCount();
				bytecode.pShaderBytecode = info.Stages[i].ShaderData.begin();
				buffer.WriteBytes(sizeof(D3D12_SHADER_BYTECODE), reinterpret_cast<const byte*>(&bytecode));
			}
		}

		{
			type = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;
			buffer.WriteBytes(sizeof(D3D12_PIPELINE_STATE_SUBOBJECT_TYPE), reinterpret_cast<const byte*>(&type));

			D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
			inputLayoutDesc.NumElements = static_cast<UINT32>(info.VertexDescriptor.ElementCount());

			GTSL::uint32 offset = 0;

			for (GTSL::uint32 i = 0; i < inputLayoutDesc.NumElements; ++i)
			{
				D3D12_INPUT_ELEMENT_DESC elementDesc;
				elementDesc.Format = static_cast<DXGI_FORMAT>(info.VertexDescriptor[i]);
				elementDesc.AlignedByteOffset = offset;
				elementDesc.SemanticIndex = 0;
				elementDesc.InputSlot = i;
				elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				elementDesc.InstanceDataStepRate = 0;

				switch (info.VertexDescriptor[i])
				{
				case DX12ShaderDataType::FLOAT:  offset += 4; break;
				case DX12ShaderDataType::FLOAT2: offset += 8; break;
				case DX12ShaderDataType::FLOAT3: offset += 12; break;
				case DX12ShaderDataType::FLOAT4: offset += 16; break;
				case DX12ShaderDataType::INT: offset += 4; break;
				case DX12ShaderDataType::INT2: offset += 8; break;
				case DX12ShaderDataType::INT3: offset += 12; break;
				case DX12ShaderDataType::INT4: offset += 16; break;
				case DX12ShaderDataType::BOOL: offset += 1; break;
				default:;
				}

				elementDesc.SemanticName = "POSITION";
				vertexElements.EmplaceBack(elementDesc);
			}

			inputLayoutDesc.pInputElementDescs = vertexElements.begin();
			buffer.WriteBytes(sizeof(D3D12_INPUT_LAYOUT_DESC), reinterpret_cast<const byte*>(&inputLayoutDesc));
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
