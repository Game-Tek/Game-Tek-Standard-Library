#pragma once

#include "GAL/CommandBuffer.h"

#include "Vulkan.h"
#include "VulkanTexture.h"
#include "VulkanRenderDevice.h"
#include "VulkanAccelerationStructures.h"
#include "VulkanPipelines.h"
#include <GTSL/RGB.h>

#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "GTSL/Vector.hpp"

#undef MemoryBarrier

namespace GAL
{
	class VulkanFramebuffer;
	class VulkanRenderPass;
	class VulkanPipelineLayout;
	class VulkanQueue;
	class VulkanTexture;
	class VulkanPipeline;
	class VulkanBindingsSet;
	class VulkanRenderDevice;
	class VulkanBuffer;

	class VulkanCommandBuffer final
	{
	public:
		VulkanCommandBuffer() = default;

		struct CreateInfo : VulkanCreateInfo
		{
		};
		
		explicit VulkanCommandBuffer(const VkCommandBuffer commandBuffer) : commandBuffer(commandBuffer) {}

		struct BeginRecordingInfo final : VulkanRenderInfo
		{
			/**
			 * \brief Pointer to primary/parent command buffer, can be null if this command buffer is primary/has no children.
			 */
			const CommandBuffer* PrimaryCommandBuffer{ nullptr };
		};
		void BeginRecording(const BeginRecordingInfo& beginRecordingInfo);

		struct EndRecordingInfo final : VulkanRenderInfo
		{
		};
		void EndRecording(const EndRecordingInfo& endRecordingInfo);

		struct BeginRenderPassInfo final : VulkanRenderInfo
		{
			VulkanRenderPass RenderPass;
			VulkanFramebuffer Framebuffer;
			GTSL::Extent2D RenderArea;
			GTSL::Range<const GTSL::RGBA*> ClearValues;
		};
		void BeginRenderPass(const BeginRenderPassInfo& beginRenderPassInfo);

		struct AdvanceSubpassInfo final : VulkanRenderInfo
		{
		};
		void AdvanceSubPass(const AdvanceSubpassInfo& advanceSubpassInfo);

		void EndRenderPass(const VulkanRenderDevice* renderDevice);

		struct SetScissorInfo final : VulkanRenderInfo
		{
			GTSL::Extent2D Area;
			GTSL::Extent2D Offset;
		};
		void SetScissor(const SetScissorInfo& info);
		
		void BindPipeline(const VulkanRenderDevice* renderDevice, VulkanPipeline pipeline, VulkanPipelineType pipelineType);

		void BindIndexBuffer(const VulkanRenderDevice* renderDevice, VulkanBuffer buffer, GTSL::uint32 offset, VulkanIndexType indexType) const;

		void BindVertexBuffer(const VulkanRenderDevice* renderDevice, VulkanBuffer buffer, GTSL::uint32 offset) const;

		void UpdatePushConstant(const VulkanRenderDevice* vulkanRenderDevice, VulkanPipelineLayout pipelineLayout, GTSL::uint32 offset, GTSL::Range<const GTSL::byte*> data, VulkanShaderStage::value_type shaderStages);

		void Draw(const VulkanRenderDevice* renderDevice, uint32_t vertexCount, uint32_t instanceCount = 0, uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
		
		void DrawIndexed(const VulkanRenderDevice* renderDevice, uint32_t indexCount, uint32_t instanceCount = 0) const;

		struct ShaderTableDescriptor
		{
			VulkanDeviceAddress Address = 0;

			/**
			 * \brief Number of entries in the shader group.
			 */
			GTSL::uint32 Entries = 0;

			/**
			 * \brief Size of each entry in the shader group.
			 */
			GTSL::uint32 EntrySize = 0;
		};
		
		void TraceRays(const VulkanRenderDevice* renderDevice, GTSL::Array<ShaderTableDescriptor, 4> shaderTableDescriptors, GTSL::Extent3D dispatchSize);
		
		void AddLabel(const VulkanRenderDevice* renderDevice, GTSL::Range<const GTSL::UTF8*> name);

		void BeginRegion(const VulkanRenderDevice* renderDevice, GTSL::Range<const GTSL::UTF8*> name) const;

		void EndRegion(const VulkanRenderDevice* renderDevice) const;
		
		void Dispatch(const VulkanRenderDevice* renderDevice, GTSL::Extent3D workGroups);

		void BindBindingsSets(const VulkanRenderDevice* renderDevice, VulkanPipelineType pipelineType, GTSL::Range<const VulkanBindingsSet*> bindingsSets, GTSL::Range<const GTSL::uint32*> offsets, VulkanPipelineLayout pipelineLayout, GTSL::uint32 firstSet);

		void CopyTextureToTexture(const VulkanRenderDevice* renderDevice, VulkanTexture sourceTexture, VulkanTexture destinationTexture, VulkanTextureLayout sourceLayout, VulkanTextureLayout destinationLayout, GTSL::Extent3D extent);

		struct TextureCopyInfo
		{
			GTSL::Extent3D Extent;
		};

		template<class ALLOCATOR>
		void CopyTextureToTexture(const VulkanRenderDevice* renderDevice, const GTSL::Range<TextureCopyInfo*> textureCopyInfos, VulkanTexture sourceTexture, VulkanTexture destinationTexture, VulkanTextureLayout sourceLayout, VulkanTextureLayout destinationLayout, const ALLOCATOR& allocator)
		{
			GTSL::Vector<VkImageCopy, ALLOCATOR> vkImagesCopies;

			for(auto& e : textureCopyInfos)
			{
				auto& vkImageCopy = vkImagesCopies.EmplaceBack();
				vkImageCopy.extent = Extent3DToVkExtent3D(e.Extent);
				vkImageCopy.srcOffset = {};
				vkImageCopy.dstOffset = {};
				vkImageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				vkImageCopy.srcSubresource.baseArrayLayer = 0;
				vkImageCopy.srcSubresource.layerCount = 1;
				vkImageCopy.srcSubresource.mipLevel = 0;

				vkImageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				vkImageCopy.dstSubresource.baseArrayLayer = 0;
				vkImageCopy.dstSubresource.layerCount = 1;
				vkImageCopy.dstSubresource.mipLevel = 0;
			}

			vkCmdCopyImage(commandBuffer, sourceTexture.GetVkImage(), static_cast<VkImageLayout>(sourceLayout),
				destinationTexture.GetVkImage(), static_cast<VkImageLayout>(destinationLayout), vkImagesCopies.GetLength(), vkImagesCopies.begin());
		}

		struct CopyBufferToTextureInfo : VulkanRenderInfo
		{
			VulkanBuffer SourceBuffer;
			VulkanTextureLayout TextureLayout;
			VulkanTexture DestinationTexture;

			GTSL::Extent3D Extent;
			GTSL::Extent3D Offset;
		};
		void CopyBufferToTexture(const CopyBufferToTextureInfo& copyBufferToImageInfo);

		struct MemoryBarrier
		{
			VulkanAccessFlags::value_type SourceAccessFlags, DestinationAccessFlags;
		};

		struct BufferBarrier
		{
		};

		struct TextureBarrier
		{			
			VulkanTexture Texture;
			VulkanTextureLayout CurrentLayout, TargetLayout;
			VulkanAccessFlags::value_type SourceAccessFlags, DestinationAccessFlags;
			VulkanTextureType::value_type TextureType;
		};

		enum class BarrierType : GTSL::uint8
		{
			MEMORY, BUFFER, TEXTURE
		};
		
		struct BarrierData
		{
			BarrierData() = default;
			BarrierData(const MemoryBarrier memoryBarrier) : Type(BarrierType::MEMORY), Barrier(memoryBarrier) {}
			BarrierData(const BufferBarrier bufferBarrier) : Type(BarrierType::BUFFER), Barrier(bufferBarrier) {}
			BarrierData(const TextureBarrier textureBarrier) : Type(BarrierType::TEXTURE), Barrier(textureBarrier) {}
			
			BarrierType Type;

			union Barriers
			{
				Barriers() = default;
				Barriers(const TextureBarrier& textureBarrier) : TextureBarrier(textureBarrier) {}
				Barriers(const MemoryBarrier& memoryBarrier) : MemoryBarrier(memoryBarrier) {}
				Barriers(const BufferBarrier& bufferBarrier) : BufferBarrier(bufferBarrier) {}
				
				MemoryBarrier MemoryBarrier;
				BufferBarrier BufferBarrier;
				TextureBarrier TextureBarrier;
			} Barrier;

			void SetMemoryBarrier(MemoryBarrier memoryBarrier) { Type = BarrierType::MEMORY; Barrier.MemoryBarrier = memoryBarrier; }
			void SetTextureBarrier(TextureBarrier textureBarrier) { Type = BarrierType::TEXTURE; Barrier.TextureBarrier = textureBarrier; }
			void SetBufferBarrier(BufferBarrier bufferBarrier) { Type = BarrierType::BUFFER; Barrier.BufferBarrier = bufferBarrier; }
		};

		template<class ALLOCATOR>
		void AddPipelineBarrier(const VulkanRenderDevice* renderDevice, GTSL::Range<const BarrierData*> barriers, VulkanPipelineStage::value_type initialStage, VulkanPipelineStage::value_type finalStage, const ALLOCATOR& allocator) const
		{
			GTSL::Vector<VkImageMemoryBarrier, ALLOCATOR> imageMemoryBarriers(4, allocator);
			GTSL::Vector<VkMemoryBarrier, ALLOCATOR> memoryBarriers(4, allocator);

			for(auto& b : barriers)
			{
				switch (b.Type)
				{
				case BarrierType::MEMORY:
				{
					auto& barrier = b.Barrier.MemoryBarrier;
					auto& memoryBarrier = memoryBarriers.EmplaceBack();

					memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER; memoryBarrier.pNext = nullptr;
					memoryBarrier.srcAccessMask = static_cast<VkAccessFlags>(barrier.SourceAccessFlags);
					memoryBarrier.dstAccessMask = static_cast<VkAccessFlags>(barrier.DestinationAccessFlags);
						
					break;
				}
				case BarrierType::BUFFER:
				{
					break;
				}
				case BarrierType::TEXTURE:
				{
					auto& barrier = b.Barrier.TextureBarrier;
					auto& textureBarrier = imageMemoryBarriers.EmplaceBack();
						
					textureBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER; textureBarrier.pNext = nullptr;
					textureBarrier.oldLayout = static_cast<VkImageLayout>(barrier.CurrentLayout);
					textureBarrier.newLayout = static_cast<VkImageLayout>(barrier.TargetLayout);
					textureBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					textureBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					textureBarrier.image = barrier.Texture.GetVkImage();
					textureBarrier.subresourceRange.aspectMask = (VkImageAspectFlags)barrier.TextureType;
					textureBarrier.subresourceRange.baseMipLevel = 0;
					textureBarrier.subresourceRange.levelCount = 1;
					textureBarrier.subresourceRange.baseArrayLayer = 0;
					textureBarrier.subresourceRange.layerCount = 1;
					textureBarrier.srcAccessMask = static_cast<VkAccessFlags>(barrier.SourceAccessFlags);
					textureBarrier.dstAccessMask = static_cast<VkAccessFlags>(barrier.DestinationAccessFlags);
					break;
				}
				}
			}

			vkCmdPipelineBarrier(commandBuffer, static_cast<VkPipelineStageFlags>(initialStage), static_cast<VkPipelineStageFlags>(finalStage), 0,
				memoryBarriers.GetLength(), memoryBarriers.begin(), 0, nullptr, imageMemoryBarriers.GetLength(), imageMemoryBarriers.begin());
		}

		struct CopyBuffersInfo : VulkanRenderInfo
		{
			VulkanBuffer Source;
			GTSL::uint32 SourceOffset{ 0 };
			VulkanBuffer Destination;
			GTSL::uint32 DestinationOffset{ 0 };

			GTSL::uint32 Size{ 0 };
		};
		void CopyBuffers(const CopyBuffersInfo& copyBuffersInfo);

		template<typename ALLOCATOR>
		void BuildAccelerationStructure(const VulkanRenderDevice* renderDevice, GTSL::Range<BuildAccelerationStructureInfo*> infos, const ALLOCATOR& allocator) const
		{
			GTSL::Vector<VkAccelerationStructureBuildGeometryInfoKHR, ALLOCATOR> buildGeometryInfos(infos.ElementCount(), allocator);
			GTSL::Vector<GTSL::Vector<VkAccelerationStructureGeometryKHR, ALLOCATOR>, ALLOCATOR> geoPerAccStructure(infos.ElementCount(), allocator);
			GTSL::Vector<GTSL::Vector<VkAccelerationStructureBuildRangeInfoKHR, ALLOCATOR>, ALLOCATOR> buildRangesPerAccelerationStructure(infos.ElementCount(), allocator);
			GTSL::Vector<VkAccelerationStructureBuildRangeInfoKHR*, ALLOCATOR> buildRangesRangePerAccelerationStructure(infos.ElementCount(), allocator);

			for (GTSL::uint32 accStrInfoIndex = 0; accStrInfoIndex < infos.ElementCount(); ++accStrInfoIndex)
			{
				auto& source = infos[accStrInfoIndex];

				geoPerAccStructure.EmplaceBack().Initialize(source.Geometries.ElementCount(), allocator);
				buildRangesPerAccelerationStructure.EmplaceBack().Initialize(source.Geometries.ElementCount(), allocator);
				buildRangesRangePerAccelerationStructure.EmplaceBack(buildRangesPerAccelerationStructure[accStrInfoIndex].begin());

				for (GTSL::uint32 i = 0; i < source.Geometries.ElementCount(); ++i)
				{
					VkAccelerationStructureGeometryKHR accelerationStructureGeometry; VkAccelerationStructureBuildRangeInfoKHR buildRange;
					buildGeometryAndRange(source.Geometries[i], accelerationStructureGeometry, buildRange);
					geoPerAccStructure[accStrInfoIndex].EmplaceBack(accelerationStructureGeometry);
					buildRangesPerAccelerationStructure[accStrInfoIndex].EmplaceBack(buildRange);
				}

				VkAccelerationStructureBuildGeometryInfoKHR buildGeometryInfo{ VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR };
				buildGeometryInfo.flags = source.Flags;
				buildGeometryInfo.srcAccelerationStructure = source.SourceAccelerationStructure.GetVkAccelerationStructure();
				buildGeometryInfo.dstAccelerationStructure = source.DestinationAccelerationStructure.GetVkAccelerationStructure();
				buildGeometryInfo.type = source.Geometries[0].Type == VulkanGeometryType::INSTANCES ? VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR : VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
				buildGeometryInfo.pGeometries = geoPerAccStructure[accStrInfoIndex].begin();
				buildGeometryInfo.ppGeometries = nullptr;
				buildGeometryInfo.geometryCount = geoPerAccStructure[accStrInfoIndex].GetLength();
				buildGeometryInfo.scratchData.deviceAddress = source.ScratchBufferAddress;
				buildGeometryInfo.mode = source.SourceAccelerationStructure.GetVkAccelerationStructure() ? VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR : VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
				buildGeometryInfos.EmplaceBack(buildGeometryInfo);
			}

			renderDevice->vkCmdBuildAccelerationStructuresKHR(commandBuffer, buildGeometryInfos.GetLength(),
				buildGeometryInfos.begin(), buildRangesRangePerAccelerationStructure.begin());
		}

		void SetEvent(VulkanEvent event, VulkanPipelineStage pipelineStage);
		void ResetEvent(VulkanEvent event, VulkanPipelineStage pipelineStage);
		
		[[nodiscard]] VkCommandBuffer GetVkCommandBuffer() const { return commandBuffer; }

	private:
		VkCommandBuffer commandBuffer = nullptr;
		friend class VulkanCommandPool;
	};

	class VulkanCommandPool final : public CommandPool
	{
	public:
		VulkanCommandPool() = default;

		struct CreateInfo final : VulkanCreateInfo
		{
			VulkanQueue Queue;
			bool IsPrimary = true;
		};
		void Initialize(const CreateInfo& createInfo);

		void ResetPool(RenderDevice* renderDevice) const;

		struct AllocateCommandBuffersInfo final : VulkanRenderInfo
		{
			bool IsPrimary = true;
			GTSL::Range<const VulkanCommandBuffer::CreateInfo*> CommandBufferCreateInfos;
			GTSL::Range<VulkanCommandBuffer*> CommandBuffers;
		};
		void AllocateCommandBuffer(const AllocateCommandBuffersInfo& allocateCommandBuffersInfo) const;
		
		struct FreeCommandBuffersInfo final : VulkanRenderInfo
		{
			GTSL::Range<VulkanCommandBuffer*> CommandBuffers;
		};
		void FreeCommandBuffers(const FreeCommandBuffersInfo& freeCommandBuffers) const;
		
		void Destroy(const class VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkCommandPool GetVkCommandPool() const { return commandPool; }
		
	private:
		VkCommandPool commandPool = nullptr;
	};
}
