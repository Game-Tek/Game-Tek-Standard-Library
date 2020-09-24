#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"
#include "VulkanMemory.h"
#include "GTSL/Ranger.h"
#include "GTSL/Math/Matrix4.h"

namespace GAL
{
	struct AccelerationStructureGeometry
	{
		VulkanGeometryFlags::value_type Flags;
		VulkanGeometryType Type;
		VulkanShaderDataType VertexFormat;
		GTSL::uint32 VertexStride;
		VulkanIndexType IndexType;
		VulkanDeviceAddress VertexData, IndexData;
	};

	struct BuildOffset
	{
		GTSL::uint32 PrimitiveCount = 0, PrimitiveOffset = 0, FirstVertex = 0, TransformOffset = 0;
	};
	
	class VulkanAccelerationStructure
	{
	public:
		[[nodiscard]] VkAccelerationStructureKHR GetVkAccelerationStructure() const { return accelerationStructure; }

		struct GeometryType
		{
		private:
			GTSL::uint32 stype; void* next = nullptr;
		public:
			VulkanGeometryType Type;
			GTSL::uint32 MaxPrimitiveCount = 0;
			VulkanIndexType IndexType;
			GTSL::uint32 MaxVertexCount = 0;
			VulkanShaderDataType VertexType;
			GTSL::uint32 AllowTransforms = false;

		private:
			friend VulkanAccelerationStructure;
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
		
		struct GeometryTriangleData
		{
			VulkanShaderDataType VertexType;
			VulkanIndexType IndexType;
			GTSL::uint16 VertexStride = 0;
			VulkanDeviceAddress VertexBufferAddress;
			VulkanDeviceAddress IndexBufferAddress;
		};

		struct Geometry
		{
			VulkanGeometryType GeometryType;
			VulkanGeometryFlags::value_type GeometryFlags;
			GeometryTriangleData* GeometryTriangleData;
		};

		struct AccelerationStructureBuildOffsetInfo
		{
			GTSL::uint32 FirstVertex;
			GTSL::uint32 PrimitiveCount;
			GTSL::uint32 PrimitiveOffset;
			GTSL::uint32 TransformOffset;
		};

		VulkanAccelerationStructure() = default;
		
		struct TopLevelCreateInfo : VulkanCreateInfo
		{
			VulkanAccelerationStructureFlags::value_type Flags;
			GTSL::uint32 MaxGeometryCount = 0;
			GTSL::uint32 CompactedSize = 0;
			VulkanDeviceAddress DeviceAddress;
			GTSL::Ranger<GeometryType> GeometryInfos;
		};
		//VulkanAccelerationStructure(const TopLevelCreateInfo& info);

		void Initialize(const TopLevelCreateInfo& info);
		
		struct BottomLevelCreateInfo : VulkanCreateInfo
		{
			VulkanAccelerationStructureFlags::value_type Flags;
			GTSL::uint32 MaxGeometryCount = 0;
			GTSL::uint32 CompactedSize = 0;
			VulkanDeviceAddress DeviceAddress;
			GTSL::Ranger<GeometryType> GeometryInfos;
		};
		VulkanAccelerationStructure(const BottomLevelCreateInfo& info);


		void Destroy(const VulkanRenderDevice* renderDevice);

		struct BindToMemoryInfo : VulkanRenderInfo
		{
			VulkanDeviceMemory Memory;
			uint32_t Offset = 0;
		};
		void BindToMemory(const BindToMemoryInfo& info) const;

		VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		
		//UTILITY
		static void BuildAccelerationStructure(const struct BuildAccelerationStructuresInfo& info);
		
	protected:
		VkAccelerationStructureKHR accelerationStructure = nullptr;

	};

	struct BuildAccelerationStructureInfo
	{
	private:
		GTSL::uint32 stype; void* next;

		friend VulkanAccelerationStructure;
		friend class VulkanCommandBuffer;
	public:
		GTSL::uint32 IsTopLevel = false;
		GTSL::uint32 Flags = false;
		GTSL::uint32 Update = false;
		VulkanAccelerationStructure SourceAccelerationStructure;
		VulkanAccelerationStructure DestinationAccelerationStructure;
		GTSL::uint32 IsArrayOfPointers = false;
		GTSL::uint32 Count = 0;
		const AccelerationStructureGeometry* Geometries;
		VulkanDeviceAddress ScratchBufferAddress;
	};
	
	struct BuildAccelerationStructuresInfo : VulkanRenderInfo
	{
		GTSL::Ranger<BuildAccelerationStructureInfo> BuildAccelerationStructureInfos;
		const BuildOffset* const* BuildOffsets;
	};
}
