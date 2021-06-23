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

		void Initialize(const VulkanRenderDevice* renderDevice, GTSL::Range<const BindingsPoolSize*> bindingsPoolSizes, GTSL::uint32 maxSets) {
			GTSL::Array<VkDescriptorPoolSize, MAX_BINDINGS_PER_SET> vkDescriptorPoolSizes;

			for (GTSL::uint32 i = 0; i < static_cast<GTSL::uint32>(bindingsPoolSizes.ElementCount()); ++i) {
				auto& descriptorPoolSize = vkDescriptorPoolSizes.EmplaceBack();
				descriptorPoolSize.type = ToVulkan(bindingsPoolSizes[i].BindingType);
				//Max number of descriptors of VkDescriptorPoolSize::type we can allocate.
				descriptorPoolSize.descriptorCount = bindingsPoolSizes[&descriptorPoolSize - vkDescriptorPoolSizes.begin()].Count;
			}

			VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
			//Is the total number of sets that can be allocated from the pool.
			vkDescriptorPoolCreateInfo.maxSets = maxSets;
			vkDescriptorPoolCreateInfo.poolSizeCount = vkDescriptorPoolSizes.GetLength();
			vkDescriptorPoolCreateInfo.pPoolSizes = vkDescriptorPoolSizes.begin();
			renderDevice->VkCreateDescriptorPool(renderDevice->GetVkDevice(), &vkDescriptorPoolCreateInfo, renderDevice->GetVkAllocationCallbacks(), &descriptorPool);
			//setName(renderDevice, descriptorPool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, createInfo.Name);
		}

		void Destroy(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkDestroyDescriptorPool(renderDevice->GetVkDevice(), descriptorPool, renderDevice->GetVkAllocationCallbacks());
			debugClear(descriptorPool);
		}

		//void FreeBindingsSet(const VulkanRenderDevice* renderDevice) {
		//	vkFreeDescriptorSets(renderDevice->GetVkDevice(), descriptorPool,
		//		static_cast<GTSL::uint32>(freeBindingsSetInfo.BindingsSet.ElementCount()), reinterpret_cast<VkDescriptorSet*>(freeBindingsSetInfo.BindingsSet.begin()));
		//}

		[[nodiscard]] VkDescriptorPool GetVkDescriptorPool() const { return descriptorPool; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(descriptorPool); }

	private:
		VkDescriptorPool descriptorPool;
	};

	class VulkanBindingsSetLayout final
	{
	public:
		struct BindingDescriptor
		{
			BindingType BindingType;
			ShaderStage ShaderStage;
			GTSL::uint32 BindingsCount;
			BindingFlag Flags;
		};

		struct ImageBindingDescriptor : BindingDescriptor
		{
			GTSL::Range<const class VulkanTextureView*> ImageViews;
			GTSL::Range<const class VulkanSampler*> Samplers;
			GTSL::Range<const TextureLayout*> Layouts;
		};

		struct BufferBindingDescriptor : BindingDescriptor
		{
			GTSL::Range<const class VulkanBuffer*> Buffers;
			GTSL::Range<const GTSL::uint32*> Offsets;
			GTSL::Range<const GTSL::uint32*> Sizes;
		};

		VulkanBindingsSetLayout() = default;

		void Initialize(const VulkanRenderDevice* renderDevice, GTSL::Range<const BindingDescriptor*> bindingsDescriptors) {
			GTSL::Array<VkDescriptorBindingFlags, 16> vkDescriptorBindingFlags;
			VkDescriptorSetLayoutBindingFlagsCreateInfo vkDescriptorSetLayoutBindingFlagsCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
			vkDescriptorSetLayoutBindingFlagsCreateInfo.bindingCount = static_cast<GTSL::uint32>(bindingsDescriptors.ElementCount());
			vkDescriptorSetLayoutBindingFlagsCreateInfo.pBindingFlags = vkDescriptorBindingFlags.begin();

			VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			vkDescriptorSetLayoutCreateInfo.pNext = &vkDescriptorSetLayoutBindingFlagsCreateInfo;

			GTSL::Array<VkDescriptorSetLayoutBinding, MAX_BINDINGS_PER_SET> vkDescriptorSetLayoutBindings;

			for (GTSL::uint32 i = 0; i < static_cast<GTSL::uint32>(bindingsDescriptors.ElementCount()); ++i) {
				vkDescriptorBindingFlags.EmplaceBack();
				vkDescriptorSetLayoutBindings.EmplaceBack();
			}

			{
				GTSL::uint32 i = 0;

				for (auto& binding : vkDescriptorSetLayoutBindings) {
					binding.binding = i;
					binding.descriptorCount = bindingsDescriptors[i].BindingsCount;
					binding.descriptorType = ToVulkan(bindingsDescriptors[i].BindingType);
					binding.stageFlags = ToVulkan(bindingsDescriptors[i].ShaderStage);
					binding.pImmutableSamplers = nullptr;

					vkDescriptorBindingFlags[i] = ToVulkan(bindingsDescriptors[i].Flags);
					++i;
				}

			}

			vkDescriptorSetLayoutCreateInfo.bindingCount = vkDescriptorSetLayoutBindings.GetLength();
			vkDescriptorSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBindings.begin();

			renderDevice->VkCreateDescriptorSetLayout(renderDevice->GetVkDevice(), &vkDescriptorSetLayoutCreateInfo, renderDevice->GetVkAllocationCallbacks(), &descriptorSetLayout);
			//setName(createInfo.RenderDevice, &descriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, createInfo.Name);
		}
		void Destroy(const VulkanRenderDevice* renderDevice) {
			renderDevice->VkDestroyDescriptorSetLayout(renderDevice->GetVkDevice(), descriptorSetLayout, renderDevice->GetVkAllocationCallbacks());
			debugClear(descriptorSetLayout);
		}

		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return descriptorSetLayout; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(descriptorSetLayout); }

	private:
		VkDescriptorSetLayout descriptorSetLayout;
	};

	class VulkanBindingsSet final : public BindingsSet
	{
	public:
		VulkanBindingsSet() = default;

		struct TextureBindingUpdateInfo {
			VulkanSampler Sampler;
			VulkanTextureView TextureView;
			TextureLayout TextureLayout;
			FormatDescriptor FormatDescriptor;
		};

		struct BufferBindingUpdateInfo {
			VulkanBuffer Buffer;
			GTSL::uint64 Offset, Range;
		};

		struct AccelerationStructureBindingUpdateInfo {
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
			BindingType Type;
			GTSL::uint32 SubsetIndex = 0, BindingIndex = 0;
			GTSL::Range<const BindingUpdateInfo*> BindingUpdateInfos;
		};

		void Initialize(const VulkanRenderDevice* renderDevice, VulkanBindingsPool bindingsPool, const VulkanBindingsSetLayout bindingsSetLayouts) {
			auto vkDescriptorSetLayout = bindingsSetLayouts.GetVkDescriptorSetLayout();
			
			VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			//vkDescriptorSetAllocateInfo.pNext = &vkDescriptorSetVariableDescriptorCountAllocateInfo;
			vkDescriptorSetAllocateInfo.descriptorPool = bindingsPool.GetVkDescriptorPool();
			vkDescriptorSetAllocateInfo.descriptorSetCount = 1;
			vkDescriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayout;
			renderDevice->VkAllocateDescriptorSets(renderDevice->GetVkDevice(), &vkDescriptorSetAllocateInfo, &descriptorSet);
		}

		template<class ALLOCATOR>
		void Update(const VulkanRenderDevice* renderDevice, GTSL::Range<const BindingsUpdateInfo*> bindingsUpdateInfos, const ALLOCATOR& allocator)
		{
			GTSL::Vector<VkWriteDescriptorSet, ALLOCATOR> vkWriteDescriptorSets(static_cast<uint32_t>(bindingsUpdateInfos.ElementCount()), allocator);

			VkWriteDescriptorSetAccelerationStructureKHR as{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR };

			GTSL::Vector<GTSL::Vector<VkAccelerationStructureKHR, ALLOCATOR>, ALLOCATOR> accelerationStructuresPerSubSetUpdate(8, allocator);
			GTSL::Vector<GTSL::Vector<VkDescriptorBufferInfo, ALLOCATOR>, ALLOCATOR> buffersPerSubSetUpdate(8, allocator);
			GTSL::Vector<GTSL::Vector<VkDescriptorImageInfo, ALLOCATOR>, ALLOCATOR> imagesPerSubSetUpdate(8, allocator);

			for (GTSL::uint32 index = 0; index < static_cast<GTSL::uint32>(bindingsUpdateInfos.ElementCount()); ++index) {
				VkWriteDescriptorSet& writeSet = vkWriteDescriptorSets.EmplaceBack();
				auto& info = bindingsUpdateInfos[index];

				writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; writeSet.pNext = nullptr;
				writeSet.dstSet = descriptorSet;
				writeSet.dstBinding = info.SubsetIndex;
				writeSet.dstArrayElement = info.BindingIndex;
				writeSet.descriptorCount = static_cast<GTSL::uint32>(info.BindingUpdateInfos.ElementCount());
				writeSet.descriptorType = ToVulkan(info.Type);
				writeSet.pImageInfo = nullptr; writeSet.pBufferInfo = nullptr; writeSet.pTexelBufferView = nullptr;

				switch (info.Type)
				{
				case BindingType::SAMPLER:
				case BindingType::COMBINED_IMAGE_SAMPLER:
				case BindingType::SAMPLED_IMAGE:
				case BindingType::STORAGE_IMAGE:
				case BindingType::INPUT_ATTACHMENT: {
					imagesPerSubSetUpdate.EmplaceBack(8, allocator);

					for (auto e : info.BindingUpdateInfos) {
						auto& vkDescriptorImageInfo = imagesPerSubSetUpdate.back().EmplaceBack();
						vkDescriptorImageInfo.sampler = e.TextureBindingUpdateInfo.Sampler.GetVkSampler();
						vkDescriptorImageInfo.imageView = e.TextureBindingUpdateInfo.TextureView.GetVkImageView();
						vkDescriptorImageInfo.imageLayout = ToVulkan(e.TextureBindingUpdateInfo.TextureLayout, e.TextureBindingUpdateInfo.FormatDescriptor);
					}
					
					writeSet.pImageInfo = imagesPerSubSetUpdate.back().begin();

					break;
				}

				case BindingType::UNIFORM_TEXEL_BUFFER: GAL_DEBUG_BREAK;
				case BindingType::STORAGE_TEXEL_BUFFER: GAL_DEBUG_BREAK;

				case BindingType::UNIFORM_BUFFER:
				case BindingType::STORAGE_BUFFER:
				case BindingType::UNIFORM_BUFFER_DYNAMIC:
				case BindingType::STORAGE_BUFFER_DYNAMIC: {
					buffersPerSubSetUpdate.EmplaceBack(8, allocator);

					for (auto e : info.BindingUpdateInfos) {
						auto& vkDescriptorBufferInfo = buffersPerSubSetUpdate.back().EmplaceBack();
						vkDescriptorBufferInfo.buffer = e.BufferBindingUpdateInfo.Buffer.GetVkBuffer();
						vkDescriptorBufferInfo.offset = e.BufferBindingUpdateInfo.Offset;
						vkDescriptorBufferInfo.range = e.BufferBindingUpdateInfo.Range;
					}
					
					writeSet.pBufferInfo = buffersPerSubSetUpdate.back().begin();
					break;
				}

				case BindingType::ACCELERATION_STRUCTURE: { //BUG: IF THERE IS MORE THAN ONE ACC STRCUCT THIS WON'T WORK
					writeSet.pNext = &as;
					as.accelerationStructureCount = static_cast<GTSL::uint32>(info.BindingUpdateInfos.ElementCount());
					accelerationStructuresPerSubSetUpdate.EmplaceBack(8, allocator);

					for (auto e : info.BindingUpdateInfos) { accelerationStructuresPerSubSetUpdate.back().EmplaceBack(e.AccelerationStructureBindingUpdateInfo.AccelerationStructure.GetVkAccelerationStructure()); }
					as.pAccelerationStructures = accelerationStructuresPerSubSetUpdate.back().begin();
					break;
				}
				default: __debugbreak();
				}
			}

			renderDevice->VkUpdateDescriptorSets(renderDevice->GetVkDevice(), vkWriteDescriptorSets.GetLength(), vkWriteDescriptorSets.begin(), 0, nullptr);
		}

		[[nodiscard]] VkDescriptorSet GetVkDescriptorSet() const { return descriptorSet; }
		[[nodiscard]] GTSL::uint64 GetHandle() const { return reinterpret_cast<GTSL::uint64>(descriptorSet); }

	private:
		VkDescriptorSet descriptorSet;
	};
}
