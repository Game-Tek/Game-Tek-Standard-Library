#pragma once

#define VK_ENABLE_BETA_EXTENSIONS
#include "Vulkan.h"
#include "VulkanMemory.h"
#include "GTSL/Ranger.h"

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

		struct AccelerationStructureGeometryType : VulkanRenderInfo
		{
		private:
			GTSL::uint32 stype; void* next = nullptr;
		public:
			VulkanGeometryType GeometryType;
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
			GTSL::float32 Transform[3][4];
			uint32_t InstanceCustomIndex : 24;
			uint32_t Mask : 8;
			uint32_t InstanceShaderBindingTableRecordOffset : 24;
			VkGeometryInstanceFlagsKHR Flags : 8;
			uint64_t AccelerationStructureReference;
		};
		
		struct AccelerationStructureGeometryTriangleData : VulkanRenderInfo
		{
			VulkanShaderDataType VertexType;
			VulkanIndexType IndexType;
			GTSL::uint16 VertexStride = 0;
			VulkanDeviceAddress VertexBufferAddress;
			VulkanDeviceAddress IndexBufferAddress;
		};

		struct AccelerationStructureGeometry : VulkanRenderInfo
		{
			VulkanGeometryType GeometryType;
			VulkanGeometryFlags::value_type GeometryFlags;
			AccelerationStructureGeometryTriangleData* GeometryTriangleData;
		};

		struct AccelerationStructureBuildOffsetInfo : VulkanRenderInfo
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
			GTSL::Ranger<AccelerationStructureGeometryType> Geometries;
		};
		//VulkanAccelerationStructure(const TopLevelCreateInfo& info);

		void Initialize(const TopLevelCreateInfo& info);
		
		struct BottomLevelCreateInfo : VulkanCreateInfo
		{
			VulkanAccelerationStructureFlags::value_type Flags;
			GTSL::uint32 MaxGeometryCount = 0;
			GTSL::uint32 CompactedSize = 0;
			VulkanDeviceAddress DeviceAddress;
			GTSL::Ranger<AccelerationStructureGeometryType> Geometries;
		};
		VulkanAccelerationStructure(const BottomLevelCreateInfo& info);


		void Destroy(const VulkanRenderDevice* renderDevice);

		struct BindToMemoryInfo : VulkanRenderInfo
		{
			VulkanDeviceMemory Memory;
			uint32_t Offset = 0;
		};
		void BindToMemory(const BindToMemoryInfo& info);

		VulkanDeviceAddress GetAddress(const VulkanRenderDevice* renderDevice) const;
		//UTILITY
		static void BuildAccelerationStructure(const struct BuildAccelerationStructuresInfo& info);
	protected:
		VkAccelerationStructureKHR accelerationStructure{};
	};

	struct BuildAccelerationStructureInfo : VulkanRenderInfo
	{
	private:
		GTSL::uint32 stype; void* next;
	public:
		GTSL::uint32 IsTopLevelFalse = false;
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
		GTSL::Ranger<const BuildAccelerationStructureInfo> BuildAccelerationStructureInfos;
		const BuildOffset* const* BuildOffsets;
	};
}
