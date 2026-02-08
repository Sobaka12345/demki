#ifndef GAPI_VK_SHADER_HPP
#define GAPI_VK_SHADER_HPP

#include "../gapi_fwd.hpp"

#include <bytes.hpp>
#include <constraints.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>

#include <spirv_reflect.h>
#include <vulkan/vulkan_core.h>

#include <array>

namespace gapi::__private {

struct MetaShader {
	// https://www.khronos.org/registry/spir-v/specs/1.0/SPIRV.pdf
	struct Id
	{
		uint32_t opcode{};
		uint32_t typeId{};
		uint32_t storageClass{};
		uint32_t binding{};
		uint32_t set{};
		uint32_t constant{};
	};

	struct LocalSizeIds {
		int32_t x{};
		int32_t y{};
		int32_t z{};
	} localSizeIds{};

	std::array<VkDescriptorType, 32>  resourceTypes{};
	
	uint32_t resourceMask{};
	bool usesPushConstants{};
	bool usesDescriptorArray{};

	VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
};

template <size_t codeSize>
struct Shader<Vk, std::integral_constant<size_t, codeSize>> : MetaShader {
	std::array<uint8_t, codeSize> spirv;
};

template <>
struct Shader<Vk> : MetaShader {
	std::vector<uint8_t> spirv;
};

inline constexpr VkShaderStageFlagBits getShaderStage(SpvExecutionModel executionModel)
{
	switch (executionModel)
	{
	case SpvExecutionModelVertex:
		return VK_SHADER_STAGE_VERTEX_BIT;
	case SpvExecutionModelGeometry:
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	case SpvExecutionModelFragment:
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	case SpvExecutionModelGLCompute:
		return VK_SHADER_STAGE_COMPUTE_BIT;
	case SpvExecutionModelTaskNV:
		return VK_SHADER_STAGE_TASK_BIT_EXT;
	case SpvExecutionModelMeshNV:
		return VK_SHADER_STAGE_MESH_BIT_EXT;
	default:
		return VkShaderStageFlagBits(0);
	}
}

inline constexpr VkDescriptorType getDescriptorType(SpvOp op)
{
	switch (op)
	{
	case SpvOpTypeStruct:
		return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	case SpvOpTypeImage:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	case SpvOpTypeSampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
	case SpvOpTypeSampledImage:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	case SpvOpTypeAccelerationStructureKHR:
		return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	default:
		return VkDescriptorType(0);
	}
}

struct ParseStepData {
	size_t step{};
	struct IdData { 
		size_t idx{};
		MetaShader::Id data{};
	};
	std::optional<VkShaderStageFlagBits> stage{};
	std::optional<IdData> idData{};
	std::optional<MetaShader::LocalSizeIds> localSizeIds{};
};

template<size_t I, size_t Count, size_t IdBound, class F>
constexpr auto static_for(F f) 
{
	std::array<MetaShader::Id, IdBound> ids{};
	MetaShader::LocalSizeIds localSizeIds = {-1, -1, -1};
	VkShaderStageFlagBits stage{};

	if constexpr (I < Count) 
	{
		constexpr ParseStepData parseStepData = f.template operator()<I, Count, IdBound>();

		constexpr auto prevResultArrays = static_for<I + parseStepData.step, Count, IdBound>(f);
		constexpr std::array<MetaShader::Id, IdBound> prevIds = std::get<0>(prevResultArrays);
		constexpr MetaShader::LocalSizeIds prevLocalSizeIds = std::get<1>(prevResultArrays);
		constexpr auto prevStage = std::get<2>(prevResultArrays);

		std::copy(prevIds.cbegin(), prevIds.cend(), ids.begin());
		localSizeIds.x = prevLocalSizeIds.x;
		localSizeIds.y = prevLocalSizeIds.y;
		localSizeIds.z = prevLocalSizeIds.z;
		stage = prevStage;

		if constexpr (parseStepData.idData.has_value()) {
			constexpr size_t idx = parseStepData.idData.value().idx;
			ids[idx].opcode = parseStepData.idData.value().data.opcode ? parseStepData.idData.value().data.opcode : ids[idx].opcode; 
			ids[idx].typeId = parseStepData.idData.value().data.typeId ? parseStepData.idData.value().data.typeId : ids[idx].typeId; 
			ids[idx].storageClass = parseStepData.idData.value().data.storageClass ? parseStepData.idData.value().data.storageClass : ids[idx].storageClass; 
			ids[idx].binding = parseStepData.idData.value().data.binding ? parseStepData.idData.value().data.binding : ids[idx].binding; 
			ids[idx].set = parseStepData.idData.value().data.set ? parseStepData.idData.value().data.set : ids[idx].set; 
			ids[idx].constant = parseStepData.idData.value().data.constant ? parseStepData.idData.value().data.constant : ids[idx].constant; 
		}

		if constexpr (parseStepData.localSizeIds.has_value()) {
			localSizeIds.x = parseStepData.localSizeIds.value().x;
			localSizeIds.y = parseStepData.localSizeIds.value().y;
			localSizeIds.z = parseStepData.localSizeIds.value().z;
		}

		if constexpr (parseStepData.stage.has_value()) {
			stage = parseStepData.stage.value();
		}

	}
	
	return std::tuple{ids, localSizeIds, stage};
}

template <auto code>
	requires isStdArray<std::remove_reference_t<decltype(code)>>::value
consteval Shader<Vk, std::integral_constant<size_t, code.size()>> parseShader() noexcept
{
	Shader<Vk, std::integral_constant<size_t, code.size()>> result;
	std::copy(code.begin(), code.end(), result.spirv.begin());

	constexpr uint32_t magicNumber = read_u32(code.data());
	CONSTEVAL_ASSERT(magicNumber == SpvMagicNumber);

	constexpr auto mult = sizeof(uint32_t);
	constexpr uint32_t idBound = read_u32(code.data() + 3 * mult);

	constexpr auto resultTuple = static_for<5 * mult, code.size(), idBound>([]<size_t I, size_t Count, size_t idBound>()
	{
		ParseStepData result{};
		constexpr size_t mult = sizeof(uint32_t); 
		constexpr size_t offset = I;
		
		constexpr uint16_t opcode = read_u16(code.data()  + offset);
		constexpr uint16_t wordCount = read_u16(code.data() + offset + 2);
		result.step = wordCount * mult;
		CONSTEVAL_ASSERT(offset + wordCount * mult <= Count);

		if constexpr (opcode == SpvOpEntryPoint) 
		{
			CONSTEVAL_ASSERT(wordCount >= 2);
			result.stage = getShaderStage(SpvExecutionModel(read_u32(code.data()  + offset + 1 * mult)));
		} else if constexpr (opcode == SpvOpExecutionMode) 
		{
			CONSTEVAL_ASSERT(wordCount >= 3);
			constexpr uint32_t mode = read_u32(code.data()  + offset + 2 * mult);

			if constexpr (mode == SpvExecutionModeLocalSize)
			{
				CONSTEVAL_ASSERT(wordCount == 6);
				result.localSizeIds = MetaShader::LocalSizeIds {
					static_cast<int32_t>(read_u32(code.data() + offset + 3 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 4 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 5 * mult)),
				};
			}
		} else if constexpr (opcode == SpvOpExecutionModeId) 
		{
			CONSTEVAL_ASSERT(wordCount >= 3);
			constexpr uint32_t mode = read_u32(code.data() + offset + 2 * mult);

			if constexpr (mode == SpvExecutionModeLocalSizeId) {
				CONSTEVAL_ASSERT(wordCount == 6);
				result.localSizeIds = MetaShader::LocalSizeIds {
					static_cast<int32_t>(read_u32(code.data() + offset + 3 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 4 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 5 * mult)),
				};
			}
		} else if constexpr (opcode == SpvOpDecorate)
		{
			CONSTEVAL_ASSERT(wordCount >= 3);

			constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
			CONSTEVAL_ASSERT(id < idBound);

			constexpr uint32_t decoration = read_u32(code.data() + offset + 2 * mult); 
			if constexpr (decoration == SpvDecorationDescriptorSet)
			{
				CONSTEVAL_ASSERT(wordCount == 4);
				result.idData = ParseStepData::IdData {
					id, 
					MetaShader::Id{
						.set = read_u32(code.data() + offset + 3 * mult)
					}
				};
			} else if (decoration == SpvDecorationBinding) 
			{
				CONSTEVAL_ASSERT(wordCount == 4);
				result.idData = ParseStepData::IdData {
					id, 
					MetaShader::Id{
						.binding = read_u32(code.data() + offset + 3 * mult)
					}
				};
			}
		} else if constexpr (
			opcode == SpvOpTypeStruct ||
			opcode == SpvOpTypeImage ||
			opcode == SpvOpTypeSampler ||
			opcode == SpvOpTypeSampledImage ||
			opcode == SpvOpTypeAccelerationStructureKHR
		)
		{
			CONSTEVAL_ASSERT(wordCount >= 2);

			constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
			CONSTEVAL_ASSERT(id < idBound);

			result.idData = ParseStepData::IdData {
				id, 
				MetaShader::Id {
					.opcode = opcode
				}
			};
		} else if constexpr (opcode == SpvOpTypePointer)
		{
			CONSTEVAL_ASSERT(wordCount == 4);

			constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
			CONSTEVAL_ASSERT(id < idBound);

			result.idData = ParseStepData::IdData {
				id, 
				MetaShader::Id{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 3 * mult),
					.storageClass = read_u32(code.data() + offset + 2 * mult),
				}
			};
		} else if constexpr (opcode == SpvOpConstant)
		{
			CONSTEVAL_ASSERT(wordCount >= 4); // we currently only correctly handle 32-bit integer constants

			constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
			CONSTEVAL_ASSERT(id < idBound);

			result.idData = ParseStepData::IdData{
				id, 
				MetaShader::Id{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 1 * mult),
					.constant = read_u32(code.data() + offset + 3 * mult), // note: this is the value, not the id of the constant
				} 
			};
		} else if constexpr (opcode == SpvOpVariable)
		{
			CONSTEVAL_ASSERT(wordCount >= 4);

			constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
			CONSTEVAL_ASSERT(id < idBound);

			result.idData = ParseStepData::IdData{
				id,	 
				MetaShader::Id{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 1 * mult),
					.storageClass = read_u32(code.data() + offset + 3 * mult),
				}
			};
		}

		return result;
	});

	constexpr std::array<MetaShader::Id, idBound> ids = std::get<0>(resultTuple);
	constexpr MetaShader::LocalSizeIds localSizeIds = std::get<1>(resultTuple);
	result.stage = std::get<2>(resultTuple);

	for (auto& id : ids)
	{
		// set 0 is reserved for push descriptors
		if (id.opcode == SpvOpVariable && (id.storageClass == SpvStorageClassUniform || id.storageClass == SpvStorageClassUniformConstant || id.storageClass == SpvStorageClassStorageBuffer) && id.set == 0)
		{
			CONSTEVAL_ASSERT(id.binding < 32);
			CONSTEVAL_ASSERT(ids[id.typeId].opcode == SpvOpTypePointer);

			uint32_t typeKind = ids[ids[id.typeId].typeId].opcode;
			VkDescriptorType resourceType = getDescriptorType(SpvOp(typeKind));

			CONSTEVAL_ASSERT((result.resourceMask & (1 << id.binding)) == 0 || result.resourceTypes[id.binding] == resourceType);

			result.resourceTypes[id.binding] = resourceType;
			result.resourceMask |= 1 << id.binding;
		}

		if (id.opcode == SpvOpVariable && id.storageClass == SpvStorageClassUniformConstant && id.set == 1)
		{
			result.usesDescriptorArray = true;
		}

		if (id.opcode == SpvOpVariable && id.storageClass == SpvStorageClassPushConstant)
		{
			result.usesPushConstants = true;
		}
	}

	if (result.stage == VK_SHADER_STAGE_COMPUTE_BIT)
	{
		if (localSizeIds.x >= 0)
		{
			CONSTEVAL_ASSERT(ids[localSizeIds.x].opcode == SpvOpConstant);
			result.localSizeIds.x = ids[localSizeIds.x].constant;
		}

		if (localSizeIds.y >= 0)
		{
			CONSTEVAL_ASSERT(ids[localSizeIds.y].opcode == SpvOpConstant);
			result.localSizeIds.y = ids[localSizeIds.y].constant;
		}

		if (localSizeIds.z >= 0)
		{
			CONSTEVAL_ASSERT(ids[localSizeIds.z].opcode == SpvOpConstant);
			result.localSizeIds.z = ids[localSizeIds.z].constant;
		}

		CONSTEVAL_ASSERT(result.localSizeIds.x && result.localSizeIds.y && result.localSizeIds.z);
	}

	return result;
}

}

#endif // GAPI_VK_SHADER_HPP