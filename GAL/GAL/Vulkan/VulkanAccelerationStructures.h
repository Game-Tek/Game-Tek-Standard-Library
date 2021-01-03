#pragma once

#include "Vulkan.h"
#include "VulkanMemory.h"
#include "GTSL/Range.h"
#include "GTSL/Math/Matrix4.h"
#include <GAL/Vulkan/VulkanBuffer.h>

namespace GAL
{
	struct BuildOffset
	{
		GTSL::uint32 PrimitiveCount = 0, PrimitiveOffset = 0, FirstVertex = 0, TransformOffset = 0;
	};
	
	class VulkanAccelerationStructure
	{
	public:
		[[nodiscard]] VkAccelerationStructureKHR GetVkAccelerationStructure() const { return accelerationStructure; }

		struct GeometryDescriptor
		{
			VulkanGeometryType Type;
			void* Data = nullptr;
			VulkanGeometryFlags::value_type Flags = 0;
		};

		struct GeometryTriangles
		{
			VulkanShaderDataType VertexFormat;
			GTSL::uint32 VertexStride, MaxVertices;
			VulkanIndexType IndexType;
			VulkanDeviceAddress VertexData, IndexData, TransformData;
		};

		struct GeometryAABB
		{
			VulkanDeviceAddress Data;
			GTSL::uint32 Stride;
		};
		
		struct GeometryInstances
		{
			VulkanDeviceAddress Data;
		};
		
		struct Instance
		{
			GTSL::Matrix3x4 Transform;
			uint32_t InstanceCustomIndex : 24;
			uint32_t Mask : 8;
			uint32_t InstanceShaderBindingTableRecordOffset : 24;
			VkGeometryInstanceFlagsKHR Flags : 8;
			uint64_t AccelerationStructureReference;
		};

		static_assert(sizeof(Instance) == 64, "Size must be 64 bytes");

		struct AccelerationStructureBuildOffsetInfo
		{
			GTSL::uint32 FirstVertex;
			GTSL::uint32 PrimitiveCount;
			GTSL::uint32 PrimitiveOffset;
			GTSL::uint32 TransformOffset;
		};

		VulkanAccelerationStructure() = default;
		
		struct CreateInfo : VulkanCreateInfo
		{
			bool IsTopLevel = false;
		
			VulkanDeviceAddress DeviceAddress = 0;

			/**
			 * \brief These describe the maximum size and format of the data that will be built into the acceleration structure.
			 */
			GTSL::Range<GeometryDescriptor*> GeometryDescriptors;

			VulkanBuffer Buffer;
			GTSL::uint32 Size, Offset;
		};
		//VulkanAccelerationStructure(const TopLevelCreateInfo& info);

		struct GetMemoryRequirementsInfo final : VulkanRenderInfo
		{
			CreateInfo* CreateInfo;

			GTSL::Range<const GTSL::uint32*> PrimitiveCounts;
			
			VulkanAccelerationStructureBuildType BuildType;
			VulkanAccelerationStructureMemoryRequirementsType MemoryRequirementsType;

			VulkanAccelerationStructureFlags::value_type Flags;
		};
		void GetMemoryRequirements(GetMemoryRequirementsInfo* memoryRequirements, GTSL::uint32* bufferSize, GTSL::uint32* scratchSize);
		
		void Initialize(const CreateInfo& info);

		void Destroy(const VulkanRenderDevice* renderDevice);

		VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		
		//UTILITY
		static void BuildAccelerationStructure(const struct BuildAccelerationStructuresInfo& info);
		
	protected:
		VkAccelerationStructureKHR accelerationStructure = nullptr;

	};

	inline void buildGeometries(GTSL::Range<VkAccelerationStructureGeometryKHR*> vkGeometries, GTSL::Range<const VulkanAccelerationStructure::GeometryDescriptor*> descriptors)
	{
		for (GTSL::uint32 i = 0; i < vkGeometries.ElementCount(); ++i)
		{
			auto& s = descriptors[i];

			vkGeometries[i].sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			vkGeometries[i].pNext = nullptr;
			vkGeometries[i].flags = s.Flags;

			switch (s.Type)
			{
			case VulkanGeometryType::TRIANGLES:
			{
				auto& t = *(VulkanAccelerationStructure::GeometryTriangles*)s.Data;

				vkGeometries[i].geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
				vkGeometries[i].geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
				vkGeometries[i].geometry.triangles.pNext = nullptr;
				vkGeometries[i].geometry.triangles.indexData.deviceAddress = t.IndexData;
				vkGeometries[i].geometry.triangles.indexType = (VkIndexType)t.IndexType;
				vkGeometries[i].geometry.triangles.maxVertex = t.MaxVertices;
				vkGeometries[i].geometry.triangles.transformData.deviceAddress = t.TransformData;
				vkGeometries[i].geometry.triangles.vertexData.deviceAddress = t.VertexData;
				vkGeometries[i].geometry.triangles.vertexFormat = (VkFormat)t.VertexFormat;
				vkGeometries[i].geometry.triangles.vertexStride = t.VertexStride;
				break;
			}

			case VulkanGeometryType::AABB:
			{
				auto& a = *(VulkanAccelerationStructure::GeometryAABB*)s.Data;

				vkGeometries[i].geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
				vkGeometries[i].geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
				vkGeometries[i].geometry.aabbs.pNext = nullptr;
				vkGeometries[i].geometry.aabbs.data.deviceAddress = a.Data;
				vkGeometries[i].geometry.aabbs.stride = a.Stride;
				break;
			}

			case VulkanGeometryType::INSTANCES:
			{
				auto& a = *(VulkanAccelerationStructure::GeometryInstances*)s.Data;

				vkGeometries[i].geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
				vkGeometries[i].geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
				vkGeometries[i].geometry.instances.pNext = nullptr;
				vkGeometries[i].geometry.instances.data.deviceAddress = a.Data;
				vkGeometries[i].geometry.instances.arrayOfPointers = false;
				break;
			}

			}
		}
	}

	struct BuildAccelerationStructureInfo
	{
		VulkanAccelerationStructure SourceAccelerationStructure;
		VulkanAccelerationStructure DestinationAccelerationStructure;
		GTSL::Range<const VulkanAccelerationStructure::GeometryDescriptor*> Geometries;
		VulkanDeviceAddress ScratchBufferAddress;
		GTSL::uint32 Flags = false;
		GTSL::uint32 Count = 0;
		VKAccelerationStructureType Type;
		bool Update = false;
	};
	
	struct BuildAccelerationStructuresInfo : VulkanRenderInfo
	{
		GTSL::Range<BuildAccelerationStructureInfo*> BuildAccelerationStructureInfos;
		const BuildOffset* const* BuildOffsets;
	};
}
