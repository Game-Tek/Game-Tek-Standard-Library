#pragma once

#include "RenderCore.h"
#include <GTSL/FixedVector.hpp>

namespace GAL
{
	struct VertexElement
	{
		ShaderDataTypes DataType;
		GTSL::uint8 Size;
	};

	class VertexDescriptor
	{
		GTSL::FixedVector<VertexElement> Elements;

		//Size in bytes this vertex takes up.
		GTSL::uint8 Size = 0;

	public:
		explicit VertexDescriptor(const GTSL::FixedVector<ShaderDataTypes>& _Elements) : Elements(_Elements.GetLength(), _Elements.GetAllocatorReference())
		{
			Elements.Resize(_Elements.GetLength());

			for (GTSL::uint8 i = 0; i < Elements.GetCapacity(); ++i)
			{
				Elements[i].Size = ShaderDataTypesSize(_Elements[i]);
				Elements[i].DataType = _Elements[i];
				Size += Elements[i].Size;
			}
		}

		[[nodiscard]] GTSL::uint8 GetOffsetToMember(GTSL::uint8 _Index) const
		{
			GTSL::uint8 Offset = 0;

			for (GTSL::uint8 i = 0; i < _Index; ++i)
			{
				Offset += Elements[i].Size;
			}

			return Offset;
		}

		[[nodiscard]] ShaderDataTypes GetAttribute(GTSL::uint8 _I) const { return Elements[_I].DataType; }

		//Returns the size in bytes this vertex takes up.
		[[nodiscard]] GTSL::uint8 GetSize() const { return Size; }
		[[nodiscard]] GTSL::uint8 GetAttributeCount() const { return Elements.GetCapacity(); }
	};

	class RenderMesh : public GALObject
	{
	public:
		struct RenderMeshCreateInfo
		{
			//Pointer to an array holding the vertices that describe the mesh.
			void* VertexData = nullptr;
			//Total number of vertices found in the VertexData array.
			GTSL::uint16 VertexCount = 0;
			//Pointer to an array holding the indices that describe the mesh.
			GTSL::uint16* IndexData = nullptr;
			//Total number of indices found in the IndexData array.
			GTSL::uint16 IndexCount = 0;
			//A vertex descriptor that defines the layout of the vertices found in VertexData.
			VertexDescriptor* VertexLayout = nullptr;

			class CommandBuffer* CommandBuffer = nullptr;
		};
	};

}
