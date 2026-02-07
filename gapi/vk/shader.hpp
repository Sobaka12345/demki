#ifndef GAPI_VK_SHADER_HPP
#define GAPI_VK_SHADER_HPP

#include "../gapi_fwd.hpp"

#include <array>
#include <concepts>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>
#include <vulkan/vulkan.hpp>

#include <spirv_reflect.h>

#include <array>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace gapi::__private {

template <>
struct Shader<Vk> {
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

	//std::string name;

	//std::array<char> spirv;
	VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;

	std::array<VkDescriptorType, 32>  resourceTypes{};
	uint32_t resourceMask{};

	struct LocalSizeIds {
		int32_t x{};
		int32_t y{};
		int32_t z{};
	} localSizeIds{};

	bool usesPushConstants{};
	bool usesDescriptorArray{};
};


consteval void consteval_assert(bool condition)
{
    // unreachable invokes undefined behavior
    // cleaner, but still no useful diagnostic
    if (!condition) std::ignore = 1 / condition;
}

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

#ifdef LITTLE_ENDIAN 
	inline consteval std::uint32_t read_u32(const uint8_t* p) {
		return (std::uint32_t)p[0]
		| ((std::uint32_t)p[1] << 8)
		| ((std::uint32_t)p[2] << 16)
		| ((std::uint32_t)p[3] << 24);
	}

	inline consteval std::uint32_t read_u16(const uint8_t* p) {
		return (std::uint32_t)p[0] | ((std::uint32_t)p[1] << 8);
	}
#elif defined(BIG_ENDIAN)
	inline consteval std::uint32_t read_u32(const uint8_t* p) {
		return ((std::uint32_t)p[0] << 24)
			| ((std::uint32_t)p[1] << 16)
			| ((std::uint32_t)p[2] << 8)
			|  (std::uint32_t)p[3];
	}

	inline consteval std::uint32_t read_u16(const uint8_t* p) {
		return ((std::uint32_t)p[0] << 8) | (std::uint32_t)p[1];
	}
#endif


struct ParseStepData {
	size_t step{};
	struct IdData { 
		size_t idx{};
		Shader<Vk>::Id data{};
	};
	std::optional<VkShaderStageFlagBits> stage{};
	std::optional<IdData> idData{};
	std::optional<Shader<Vk>::LocalSizeIds> localSizeIds{};
};

template<size_t I, size_t Count, size_t IdBound, class F>
constexpr auto static_for(F f) 
{
	std::array<Shader<Vk>::Id, IdBound> ids{};
	Shader<Vk>::LocalSizeIds localSizeIds = {-1, -1, -1};

	if constexpr (I < Count) 
	{
		constexpr ParseStepData parseStepData = f.template operator()<I, IdBound>();

		constexpr auto prevResultArrays = static_for<I + parseStepData.step, Count, IdBound>(f);
		constexpr std::array<Shader<Vk>::Id, IdBound> prevIds = std::get<0>(prevResultArrays);
		constexpr Shader<Vk>::LocalSizeIds prevLocalSizeIds = std::get<1>(prevResultArrays);

		std::copy(prevIds.cbegin(), prevIds.cend(), ids.begin());
		localSizeIds.x = prevLocalSizeIds.x;
		localSizeIds.y = prevLocalSizeIds.y;
		localSizeIds.z = prevLocalSizeIds.z;

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

	}
	
	return std::tuple{ids, localSizeIds};
}

template <typename T>
struct isStdArray : std::false_type {};

template <typename T, size_t size>
struct isStdArray<std::array<T, size>> : std::true_type {}; 

template <auto code>
	requires isStdArray<std::remove_reference_t<decltype(code)>>::value
consteval Shader<Vk> parseShader() noexcept
	//: spirv{code.begin(), code.end()}
{
	Shader<Vk> result;
	//result.spirv = code;

	constexpr uint32_t magicNumber = read_u32(code.data());
	consteval_assert(magicNumber == SpvMagicNumber);

	constexpr auto mult = sizeof(uint32_t);
	constexpr uint32_t idBound = read_u32(code.data() + 3 * mult);

	constexpr auto resultTuple = static_for<5 * mult, code.size(), idBound>([]<size_t I, size_t idBound>()
	{
		ParseStepData result{};
		constexpr size_t mult = sizeof(uint32_t); 
		constexpr size_t offset = I;
		constexpr uint16_t wordCount = read_u16(code.data() + offset);
		constexpr uint16_t opcode = read_u16(code.data()  + offset + 2);
		result.step = wordCount * mult;

		switch (opcode)
		{
		case SpvOpEntryPoint:
		{
			consteval_assert(wordCount >= 2);
			result.stage = getShaderStage(SpvExecutionModel(read_u32(code.data()  + offset + 1 * mult)));
		}
		break;
		case SpvOpExecutionMode:
		{
			consteval_assert(wordCount >= 3);
			constexpr uint32_t mode = read_u32(code.data()  + offset + 2 * mult);

			switch (mode)
			{
			case SpvExecutionModeLocalSize:
				consteval_assert(wordCount == 6);
				result.localSizeIds = Shader<Vk>::LocalSizeIds {
					static_cast<int32_t>(read_u32(code.data() + offset + 3 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 4 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 5 * mult)),
				};

				break;
			}
		}
		break;
		case SpvOpExecutionModeId:
		{
			consteval_assert(wordCount >= 3);
			constexpr uint32_t mode = read_u32(code.data() + offset + 2 * mult);

			switch (mode)
			{
			case SpvExecutionModeLocalSizeId:
				consteval_assert(wordCount == 6);
				result.localSizeIds = Shader<Vk>::LocalSizeIds {
					static_cast<int32_t>(read_u32(code.data() + offset + 3 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 4 * mult)),
					static_cast<int32_t>(read_u32(code.data() + offset + 5 * mult)),
				};
			}
		}
		break;
		case SpvOpDecorate:
		{
			consteval_assert(wordCount >= 3);

			constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
			consteval_assert(id < idBound);

			switch (read_u32(code.data() + offset + 2 * mult))
			{
			case SpvDecorationDescriptorSet:
				consteval_assert(wordCount == 4);
				result.idData = ParseStepData::IdData {
					id, 
					Shader<Vk>::Id{
						.set = read_u32(code.data() + offset + 3 * mult)
					}
				};
			break;
			case SpvDecorationBinding:
				consteval_assert(wordCount == 4);
				result.idData = ParseStepData::IdData {
					id, 
					Shader<Vk>::Id{
						.binding = read_u32(code.data() + offset + 3 * mult)
					}
				};
				break;
			}
		}
		break;
		case SpvOpTypeStruct:
		case SpvOpTypeImage:
		case SpvOpTypeSampler:
		case SpvOpTypeSampledImage:
		case SpvOpTypeAccelerationStructureKHR:
		{
			consteval_assert(wordCount >= 2);

			constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
			consteval_assert(id < idBound);

			result.idData = ParseStepData::IdData {
				id, 
				Shader<Vk>::Id {
					.opcode = opcode
				}
			};
		}
		break;
		case SpvOpTypePointer:
		{
			consteval_assert(wordCount == 4);

			constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
			consteval_assert(id < idBound);

			result.idData = ParseStepData::IdData {
				id, 
				Shader<Vk>::Id{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 3 * mult),
					.storageClass = read_u32(code.data() + offset + 2 * mult),
				}
			};
		}
		break;
		case SpvOpConstant:
		{
			consteval_assert(wordCount >= 4); // we currently only correctly handle 32-bit integer constants

			constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
			consteval_assert(id < idBound);

			result.idData = ParseStepData::IdData{
				id, 
				Shader<Vk>::Id{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 1 * mult),
					.constant = read_u32(code.data() + offset + 3 * mult), // note: this is the value, not the id of the constant
				} 
			};
		}
		break;
		case SpvOpVariable:
		{
			consteval_assert(wordCount >= 4);

			constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
			consteval_assert(id < idBound);

			result.idData = ParseStepData::IdData{
				id,	 
				Shader<Vk>::Id{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 1 * mult),
					.storageClass = read_u32(code.data() + offset + 3 * mult),
				}
			};
		}
		}

		return result;
	});

	constexpr std::array<Shader<Vk>::Id, idBound> ids = std::get<0>(resultTuple);
	constexpr Shader<Vk>::LocalSizeIds localSizeIds = std::get<1>(resultTuple);


	for (auto& id : ids)
	{
		// set 0 is reserved for push descriptors
		if (id.opcode == SpvOpVariable && (id.storageClass == SpvStorageClassUniform || id.storageClass == SpvStorageClassUniformConstant || id.storageClass == SpvStorageClassStorageBuffer) && id.set == 0)
		{
			consteval_assert(id.binding < 32);
			consteval_assert(ids[id.typeId].opcode == SpvOpTypePointer);

			uint32_t typeKind = ids[ids[id.typeId].typeId].opcode;
			VkDescriptorType resourceType = getDescriptorType(SpvOp(typeKind));

			consteval_assert((result.resourceMask & (1 << id.binding)) == 0 || result.resourceTypes[id.binding] == resourceType);

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
			consteval_assert(ids[localSizeIds.x].opcode == SpvOpConstant);
			result.localSizeIds.x = ids[localSizeIds.x].constant;
		}

		if (localSizeIds.y >= 0)
		{
			consteval_assert(ids[localSizeIds.y].opcode == SpvOpConstant);
			result.localSizeIds.y = ids[localSizeIds.y].constant;
		}

		if (localSizeIds.z >= 0)
		{
			consteval_assert(ids[localSizeIds.z].opcode == SpvOpConstant);
			result.localSizeIds.z = ids[localSizeIds.z].constant;
		}

		consteval_assert(result.localSizeIds.x && result.localSizeIds.y && result.localSizeIds.z);
	}

	return result;
}

}

#endif // GAPI_VK_SHADER_HPP