#pragma once

#include "Vulkan.h"
#include "VulkanMemory.h"
#include "GTSL/Range.h"
#include "GTSL/Math/Matrix4.h"
#include <GAL/Vulkan/VulkanBuffer.h>

namespace GAL
{
	class VulkanAccelerationStructure
	{
	public:
		[[nodiscard]] VkAccelerationStructureKHR GetVkAccelerationStructure() const { return accelerationStructure; }
		
		struct GeometryTriangles
		{	
			VulkanShaderDataType VertexFormat;
			GTSL::uint32 VertexStride, MaxVertices;
			VulkanIndexType IndexType;
			VulkanDeviceAddress VertexData, IndexData, TransformData;
			GTSL::uint32 FirstVertex;
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

		union GeometryData
		{
			GeometryTriangles Triangles;
			GeometryAABB AABB;
			GeometryInstances Instances;
		};

		struct Geometry
		{
			VulkanGeometryType Type;
			GeometryData Geometry;
			VulkanGeometryFlags::value_type Flags;

			void SetGeometryTriangles(const GeometryTriangles triangles) { Type = VulkanGeometryType::TRIANGLES; Geometry.Triangles = triangles; }
			void SetGeometryAABB(const GeometryAABB aabb) { Type = VulkanGeometryType::AABB; Geometry.AABB = aabb; }
			void SetGeometryInstances(const GeometryInstances instances) { Type = VulkanGeometryType::INSTANCES; Geometry.Instances = instances; }
			
			GTSL::uint32 PrimitiveCount, PrimitiveOffset;
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

		VulkanAccelerationStructure() = default;
		
		struct CreateInfo : VulkanCreateInfo
		{		
			VulkanDeviceAddress DeviceAddress = 0;

			/**
			 * \brief These describe the maximum size and format of the data that will be built into the acceleration structure.
			 */
			GTSL::Range<const Geometry*> Geometries;

			VulkanBuffer Buffer;
			GTSL::uint32 Size, Offset;
		};
		//VulkanAccelerationStructure(const TopLevelCreateInfo& info);

		struct GetMemoryRequirementsInfo final : VulkanRenderInfo
		{
			GTSL::Range<const Geometry*> Geometries;
			VulkanAccelerationStructureBuildType BuildType;
			VulkanAccelerationStructureFlags::value_type Flags;
		};
		void GetMemoryRequirements(const GetMemoryRequirementsInfo& memoryRequirements, GTSL::uint32* accStructureSize, GTSL::uint32* scratchSize);
		
		void Initialize(const CreateInfo& info);

		void Destroy(const VulkanRenderDevice* renderDevice);

		VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		
		//UTILITY
		static void BuildAccelerationStructure(const struct BuildAccelerationStructuresInfo& info);
		
	protected:
		VkAccelerationStructureKHR accelerationStructure = nullptr;

	};

	inline void buildGeometryAndRange(const VulkanAccelerationStructure::Geometry& descriptor, VkAccelerationStructureGeometryKHR& vkGeometry, VkAccelerationStructureBuildRangeInfoKHR& buildRange)
	{
		auto& s = descriptor;

		vkGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		vkGeometry.pNext = nullptr;
		vkGeometry.flags = s.Flags;

		buildRange.primitiveCount = descriptor.PrimitiveCount;
		buildRange.primitiveOffset = descriptor.PrimitiveOffset;
		
		switch (s.Type)
		{
		case VulkanGeometryType::TRIANGLES:
		{
			auto& t = s.Geometry.Triangles;

			vkGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
			vkGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
			vkGeometry.geometry.triangles.pNext = nullptr;
			vkGeometry.geometry.triangles.vertexData.deviceAddress = t.VertexData;
			vkGeometry.geometry.triangles.indexData.deviceAddress = t.IndexData;
			vkGeometry.geometry.triangles.transformData.deviceAddress = t.TransformData;
			vkGeometry.geometry.triangles.indexType = (VkIndexType)t.IndexType;
			vkGeometry.geometry.triangles.maxVertex = t.MaxVertices;
			vkGeometry.geometry.triangles.vertexFormat = (VkFormat)t.VertexFormat;
			vkGeometry.geometry.triangles.vertexStride = t.VertexStride;

			buildRange.firstVertex = descriptor.Geometry.Triangles.FirstVertex;
			buildRange.transformOffset = 0;
			break;
		}

		case VulkanGeometryType::AABB:
		{
			auto& a = s.Geometry.AABB;

			vkGeometry.geometryType = VK_GEOMETRY_TYPE_AABBS_KHR;
			vkGeometry.geometry.aabbs.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
			vkGeometry.geometry.aabbs.pNext = nullptr;
			vkGeometry.geometry.aabbs.data.deviceAddress = a.Data;
			vkGeometry.geometry.aabbs.stride = a.Stride;

			buildRange.firstVertex = 0;
			buildRange.transformOffset = 0;
			break;
		}

		case VulkanGeometryType::INSTANCES:
		{
			auto& a = s.Geometry.Instances;

			vkGeometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
			vkGeometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
			vkGeometry.geometry.instances.pNext = nullptr;
			vkGeometry.geometry.instances.data.deviceAddress = a.Data;
			vkGeometry.geometry.instances.arrayOfPointers = false;

			buildRange.firstVertex = 0;
			buildRange.transformOffset = 0;
			break;
		}
		}
	}

	struct BuildAccelerationStructureInfo
	{
		VulkanAccelerationStructure SourceAccelerationStructure;
		VulkanAccelerationStructure DestinationAccelerationStructure;
		GTSL::Range<const VulkanAccelerationStructure::Geometry*> Geometries;
		VulkanDeviceAddress ScratchBufferAddress;
		GTSL::uint32 Flags = 0;
	};
	
	struct BuildAccelerationStructuresInfo : VulkanRenderInfo
	{
		GTSL::Range<BuildAccelerationStructureInfo*> BuildAccelerationStructureInfos;
	};
}
