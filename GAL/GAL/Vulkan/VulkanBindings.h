#pragma once

#include "GAL/Bindings.h"

#include "Vulkan.h"
#include "VulkanAccelerationStructures.h"
#include "VulkanBuffer.h"
#include "VulkanRenderDevice.h"
#include "VulkanTexture.h"
#include "GTSL/Vector.hpp"

namespace GAL
{
	class VulkanBindingsPool final : public BindingsPool
	{
	public:
		VulkanBindingsPool() = default;
		struct BindingsPoolSize
		{
			VulkanBindingType BindingType;
			GTSL::uint32 Count = 0;
		};
		
		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::Range<const BindingsPoolSize*> BindingsPoolSizes;
			GTSL::uint32 MaxSets = 0;
		};
		VulkanBindingsPool(const CreateInfo& createInfo);

		void Destroy(const class VulkanRenderDevice* renderDevice);

		struct AllocateBindingsSetsInfo : VulkanRenderInfo
		{
			GTSL::Range<const VulkanCreateInfo*> BindingsSetCreateInfos;
			GTSL::Range<class VulkanBindingsSet**> BindingsSets;
			GTSL::Range<const class VulkanBindingsSetLayout*> BindingsSetLayouts;
		};
		void AllocateBindingsSets(const AllocateBindingsSetsInfo& allocateBindingsSetsInfo);
		
		struct FreeBindingsSetInfo : VulkanRenderInfo
		{
			GTSL::Range<class VulkanBindingsSet*> BindingsSet;
		};
		void FreeBindingsSet(const FreeBindingsSetInfo& freeBindingsSetInfo);

		[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return descriptorPool; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return (GTSL::uint64)descriptorPool; }
	
	private:
		VkDescriptorPool descriptorPool = nullptr;
	};

	class VulkanBindingsSetLayout final
	{
	public:
		struct BindingDescriptor
		{
			VulkanBindingType BindingType = VulkanBindingType::UNIFORM_BUFFER;
			VulkanShaderStage::value_type ShaderStage = VulkanShaderStage::ALL;
			GTSL::uint32 BindingsCount = 0;
			VulkanBindingFlags::value_type Flags;
		};

		struct ImageBindingDescriptor : BindingDescriptor
		{
			GTSL::Range<const class VulkanTextureView*> ImageViews;
			GTSL::Range<const class VulkanSampler*> Samplers;
			GTSL::Range<const VulkanTextureLayout*> Layouts;
		};

		struct BufferBindingDescriptor : BindingDescriptor
		{
			GTSL::Range<const class VulkanBuffer*> Buffers;
			GTSL::Range<const GTSL::uint32*> Offsets;
			GTSL::Range<const GTSL::uint32*> Sizes;
		};

		VulkanBindingsSetLayout() = default;
		
		struct CreateInfo : VulkanCreateInfo
		{
			GTSL::Range<const BindingDescriptor*> BindingsDescriptors;
		};
		VulkanBindingsSetLayout(const CreateInfo& createInfo);
		void Destroy(const VulkanRenderDevice* renderDevice);

		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return descriptorSetLayout; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return (GTSL::uint64)descriptorSetLayout; }
	
	private:
		VkDescriptorSetLayout descriptorSetLayout = nullptr;
	};
	
	class VulkanBindingsSet final : public BindingsSet
	{
	public:
		VulkanBindingsSet() = default;

		struct TextureBindingUpdateInfo
		{
			VulkanSampler Sampler;
			VulkanTextureView TextureView;
			VulkanTextureLayout TextureLayout;
		};

		struct BufferBindingUpdateInfo
		{
			VulkanBuffer Buffer;
			GTSL::uint64 Offset, Range;
		};

		struct AccelerationStructureBindingUpdateInfo
		{
			VulkanAccelerationStructure AccelerationStructure;
		};
		
		union BindingUpdateInfo
		{
			BindingUpdateInfo(TextureBindingUpdateInfo info) : TextureBindingUpdateInfo(info) {}
			BindingUpdateInfo(BufferBindingUpdateInfo info) : BufferBindingUpdateInfo(info) {}
			BindingUpdateInfo(AccelerationStructureBindingUpdateInfo info) : AccelerationStructureBindingUpdateInfo(info) {}
			
			TextureBindingUpdateInfo TextureBindingUpdateInfo;
			BufferBindingUpdateInfo BufferBindingUpdateInfo;
			AccelerationStructureBindingUpdateInfo AccelerationStructureBindingUpdateInfo;
		};

		struct BindingsUpdateInfo
		{
			VulkanBindingType Type;
			GTSL::uint32 SubsetIndex = 0, BindingIndex = 0;
			GTSL::Range<const BindingUpdateInfo*> BindingUpdateInfos;
		};
		
		struct BindingsSetUpdateInfo final : VulkanRenderInfo
		{
			GTSL::Range<const BindingsUpdateInfo*> BindingsUpdateInfos;
		};
		template<class ALLOCATOR>
		void Update(const BindingsSetUpdateInfo& bindingsUpdateInfo, const ALLOCATOR& allocator)
		{
			GTSL::Vector<VkWriteDescriptorSet, ALLOCATOR> vkWriteDescriptorSets(static_cast<uint32_t>(bindingsUpdateInfo.BindingsUpdateInfos.ElementCount()), allocator);

			VkWriteDescriptorSetAccelerationStructureKHR as{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };

			GTSL::Vector<GTSL::Vector<VkAccelerationStructureKHR, ALLOCATOR>, ALLOCATOR> accelerationStructuresPerSubSetUpdate(8, allocator);
			GTSL::Vector<GTSL::Vector<VkDescriptorBufferInfo, ALLOCATOR>, ALLOCATOR> buffersPerSubSetUpdate(8, allocator);
			GTSL::Vector<GTSL::Vector<VkDescriptorImageInfo, ALLOCATOR>, ALLOCATOR> imagesPerSubSetUpdate(8, allocator);

			for (GTSL::uint32 index = 0; index < bindingsUpdateInfo.BindingsUpdateInfos.ElementCount(); ++index)
			{
				VkWriteDescriptorSet writeSet{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
				auto& info = bindingsUpdateInfo.BindingsUpdateInfos[index];

				writeSet.dstSet = descriptorSet;
				writeSet.dstBinding = info.SubsetIndex;
				writeSet.dstArrayElement = info.BindingIndex;
				writeSet.descriptorCount = info.BindingUpdateInfos.ElementCount();
				writeSet.descriptorType = static_cast<VkDescriptorType>(info.Type);
				writeSet.pImageInfo = nullptr;
				writeSet.pBufferInfo = nullptr;
				writeSet.pTexelBufferView = nullptr;

				switch (info.Type)
				{
				case VulkanBindingType::SAMPLER:
				case VulkanBindingType::COMBINED_IMAGE_SAMPLER:
				case VulkanBindingType::SAMPLED_IMAGE:
				case VulkanBindingType::STORAGE_IMAGE:
				case VulkanBindingType::INPUT_ATTACHMENT:
				{
					imagesPerSubSetUpdate.EmplaceBack(8, allocator);
						
					for (auto e : info.BindingUpdateInfos)
					{
						VkDescriptorImageInfo vkDescriptorImageInfo{ e.TextureBindingUpdateInfo.Sampler.GetVkSampler(), e.TextureBindingUpdateInfo.TextureView.GetVkImageView(), (VkImageLayout)e.TextureBindingUpdateInfo.TextureLayout };
						imagesPerSubSetUpdate[index].EmplaceBack(vkDescriptorImageInfo);
					}
					writeSet.pImageInfo = imagesPerSubSetUpdate[index].begin();

					break;
				}

				case VulkanBindingType::UNIFORM_TEXEL_BUFFER: GAL_DEBUG_BREAK;
				case VulkanBindingType::STORAGE_TEXEL_BUFFER: GAL_DEBUG_BREAK;

				case VulkanBindingType::UNIFORM_BUFFER:
				case VulkanBindingType::STORAGE_BUFFER:
				case VulkanBindingType::UNIFORM_BUFFER_DYNAMIC:
				case VulkanBindingType::STORAGE_BUFFER_DYNAMIC:
				{
					buffersPerSubSetUpdate.EmplaceBack(8, allocator);
						
					for (auto e : info.BindingUpdateInfos)
					{
						VkDescriptorBufferInfo vkDescriptorBufferInfo{ e.BufferBindingUpdateInfo.Buffer.GetVkBuffer(), e.BufferBindingUpdateInfo.Offset, e.BufferBindingUpdateInfo.Range };
						buffersPerSubSetUpdate[index].EmplaceBack(vkDescriptorBufferInfo);
					}
					writeSet.pBufferInfo = buffersPerSubSetUpdate[index].begin();
					break;
				}

				case VulkanBindingType::ACCELERATION_STRUCTURE:
				{
					writeSet.pNext = &as;
					as.accelerationStructureCount = info.BindingUpdateInfos.ElementCount();
					accelerationStructuresPerSubSetUpdate.EmplaceBack(8, allocator);
						
					for (auto e : info.BindingUpdateInfos) { accelerationStructuresPerSubSetUpdate[index].EmplaceBack(e.AccelerationStructureBindingUpdateInfo.AccelerationStructure.GetVkAccelerationStructure()); }
					as.pAccelerationStructures = accelerationStructuresPerSubSetUpdate[index].begin();
					break;
				}
				default: __debugbreak();
				}

				vkWriteDescriptorSets.EmplaceBack(writeSet);
			}

			vkUpdateDescriptorSets(bindingsUpdateInfo.RenderDevice->GetVkDevice(), vkWriteDescriptorSets.GetLength(), vkWriteDescriptorSets.begin(), 0, nullptr);
		}

		[[nodiscard]] VkDescriptorSet GetVkDescriptorSet() const { return descriptorSet; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return (GTSL::uint64)descriptorSet; }

	private:
		VkDescriptorSet descriptorSet{ nullptr };

		friend VulkanBindingsPool;
	};
}
