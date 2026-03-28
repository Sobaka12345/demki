#ifndef GAPI_VK_SHADER_HPP
#define GAPI_VK_SHADER_HPP

#include "../gapi_fwd.hpp"
#include "assert.hpp"

#include <algorithm>
#include <bytes.hpp>
#include <constraints.hpp>
#include <helpers.hpp>

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <spirv_reflect.h>
#include <vulkan/vulkan_core.h>
#include <array>

struct MetaShader 
{
	struct LocalSizeIds {
		int32_t x{-1};
		int32_t y{-1};
		int32_t z{-1};
	} localSizeIds{};

	std::array<std::array<VkDescriptorType, 32>, GLSL_SET_COUNT> resources{};
	std::array<uint32_t, GLSL_SET_COUNT> resourceMasks{};

	bool usesPushConstants{};
	bool usesDescriptorArray{};

	VkShaderStageFlagBits stage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
};

namespace gapi::__private {

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

// https://www.khronos.org/registry/spir-v/specs/1.0/SPIRV.pdf
template <typename T>
struct IdT
{
	T opcode{};
	T typeId{};
	T storageClass{};
	T binding{};
	T set{};
	T constant{};
};
using IdOptional = IdT<std::optional<size_t>>;
using Id = IdT<size_t>;


template<typename DstT, typename SrcT>
inline constexpr void copyIdT(IdT<DstT>& dst, const IdT<SrcT>& src)
{
	utils::copy(dst.opcode, src.opcode);
	utils::copy(dst.typeId, src.typeId);
	utils::copy(dst.storageClass, src.storageClass);
	utils::copy(dst.binding, src.binding);
	utils::copy(dst.set, src.set);
	utils::copy(dst.constant, src.constant);
}

template <auto code>
struct Shader<Vk, code> : MetaShader {
	constexpr static auto mult = sizeof(uint32_t);
	constexpr static uint32_t IdBound = read_u32(code.data() + 3 * mult);
	// found out about compile-time bit_cast after I already implemented everything with 1-byte layout in mind. Maybe should rewrite later...
	constexpr static auto spirv = std::bit_cast<std::array<uint32_t, code.size() / mult>>(code);

private:	
	struct ParseStepData {
		size_t step{};
		size_t idx{};
		std::optional<VkShaderStageFlagBits> stage{};
		std::optional<IdOptional> idData{};
		std::optional<MetaShader::LocalSizeIds> localSizeIds{};
	};

	struct ParseBatchData {
		std::array<IdOptional, IdBound> ids{};
		std::optional<MetaShader::LocalSizeIds> localSizeIds{};
		std::optional<VkShaderStageFlagBits> stage{};

		consteval void overwrite(const ParseBatchData& other)
		{
			utils::copy(stage, other.stage);
			utils::copy(localSizeIds, other.localSizeIds);
			std::transform(other.ids.begin(), other.ids.end(), ids.begin(), ids.begin(), [](const auto& otherVal, const auto& val) {
				IdOptional res{};
				copyIdT(res, val);
				copyIdT(res, otherVal);
				return res;
			});
		}

		consteval void overwrite(const ParseStepData& parseStepData)
		{
			if (parseStepData.idData.has_value()) {
				copyIdT(
					ids[parseStepData.idx], 
					parseStepData.idData.value()
				);
			}

			utils::copy(localSizeIds, parseStepData.localSizeIds);
			utils::copy(stage, parseStepData.stage);
		}
	};

	template<size_t Begin, size_t End, class F>
	consteval auto static_for(F f) 
	{
		ParseBatchData result{};

		if constexpr (Begin < End) 
		{
			constexpr ParseStepData parseStepData = f.template operator()<Begin, End>();
			constexpr auto prevBatchData = static_for<Begin + parseStepData.step, End>(f);

			result.overwrite(prevBatchData);
			result.overwrite(parseStepData);
			// if constexpr (parseStepData.idData.has_value()) {
			// 	static_assert(parseStepData.idx == 0);
			// }
		}
		
		return result;
	}


	template <size_t Begin, size_t End>
	consteval auto getOffsetForBatch() {
		if constexpr (Begin < End) {
			constexpr uint16_t wordCount = read_u16(code.data() + Begin + 2);
			CONSTEVAL_ASSERT(wordCount > 0);
			return getOffsetForBatch<Begin + wordCount * 4, End>();
		}
		return Begin;
	}

	template <size_t batchSize, size_t startOffset, size_t Begin, size_t End>
	consteval auto getOffsetsForBatches() {
		std::array<size_t, End> result{};

		if constexpr (Begin < End) {
			constexpr size_t begin = startOffset;
			constexpr size_t end = std::min(startOffset + batchSize, code.size());
			result[Begin] = startOffset;
			
			constexpr size_t newOffset = getOffsetForBatch<begin, end>();
			constexpr auto offsets = getOffsetsForBatches<batchSize, newOffset, Begin + 1, End>();
			std::copy(offsets.cbegin() + Begin + 1, offsets.cend(), result.begin() + Begin + 1);
		}

		return result;
	}

	template <auto offsets, size_t I>
	consteval auto parseShaderInBatches() {
		ParseBatchData result{};

		if constexpr (I < offsets.size()) {
			constexpr size_t offsetRangeEnd = I == offsets.size() - 1 ? code.size() : offsets[I + 1];

			result = static_for<offsets[I], offsetRangeEnd>([]<size_t Offset, size_t End>()
			{
				ParseStepData result{};
				constexpr size_t offset = Offset;

				constexpr uint16_t opcode = read_u16(code.data() + offset);
				constexpr uint16_t wordCount = read_u16(code.data() + offset + 2);
				result.step = wordCount * mult;
				CONSTEVAL_ASSERT(offset + wordCount * mult <= End);

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
					CONSTEVAL_ASSERT(id < IdBound);

					constexpr uint32_t decoration = read_u32(code.data() + offset + 2 * mult); 
					if constexpr (decoration == SpvDecorationDescriptorSet)
					{
						CONSTEVAL_ASSERT(wordCount == 4);
						result.idx = id;
						result.idData = IdOptional{
							.set = read_u32(code.data() + offset + 3 * mult)
						};
					} else if (decoration == SpvDecorationBinding) 
					{
						CONSTEVAL_ASSERT(wordCount == 4);
						result.idx = id;
						result.idData = IdOptional{
							.binding = read_u32(code.data() + offset + 3 * mult)
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
					CONSTEVAL_ASSERT(id < IdBound);

					result.idx = id;
					result.idData = IdOptional{
						.opcode = opcode
					};
				} else if constexpr (opcode == SpvOpTypePointer)
				{
					CONSTEVAL_ASSERT(wordCount == 4);

					constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
					CONSTEVAL_ASSERT(id < IdBound);

					result.idx = id;
					result.idData = IdOptional{
						.opcode = opcode,
						.typeId = read_u32(code.data() + offset + 3 * mult),
						.storageClass = read_u32(code.data() + offset + 2 * mult),
					};
				} else if constexpr (opcode == SpvOpConstant)
				{
					CONSTEVAL_ASSERT(wordCount >= 4); // we currently only correctly handle 32-bit integer constants

					constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
					CONSTEVAL_ASSERT(id < IdBound);

					result.idx = id;
					result.idData = IdOptional{
						.opcode = opcode,
						.typeId = read_u32(code.data() + offset + 1 * mult),
						.constant = read_u32(code.data() + offset + 3 * mult), // note: this is the value, not the id of the constant
					};
				} else if constexpr (opcode == SpvOpVariable)
				{
					CONSTEVAL_ASSERT(wordCount >= 4);

					constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
					CONSTEVAL_ASSERT(id < IdBound);

					result.idx = id;
					result.idData = IdOptional{
						.opcode = opcode,
						.typeId = read_u32(code.data() + offset + 1 * mult),
						.storageClass = read_u32(code.data() + offset + 3 * mult),
					};
				}

				return result;
			});
			result.overwrite(parseShaderInBatches<offsets, I + 1>());
		}

		return result;
	}

public:
	consteval Shader()
	{
		constexpr uint32_t magicNumber = read_u32(code.data());
		CONSTEVAL_ASSERT(magicNumber == SpvMagicNumber);
		
		constexpr size_t maxTemplateRecursionDepth = 256;
		constexpr size_t batchCount = std::max(code.size() / maxTemplateRecursionDepth, static_cast<size_t>(1));
		constexpr std::array<size_t, batchCount> offsets = getOffsetsForBatches<maxTemplateRecursionDepth, 5 * mult, 0, batchCount>();

		constexpr auto parsedData = parseShaderInBatches<offsets, 0>();
		
		localSizeIds = parsedData.localSizeIds.value_or(MetaShader::LocalSizeIds{});
		stage = parsedData.stage.value_or(VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);
		constexpr auto ids = parsedData.ids;
		for (auto& idOpt : ids)
		{
			Id id{};
			copyIdT(id, idOpt);
			if (id.opcode == SpvOpVariable && (id.storageClass == SpvStorageClassUniform || id.storageClass == SpvStorageClassUniformConstant || id.storageClass == SpvStorageClassStorageBuffer) 
				&& (id.set == GLSL_SET_STATIC || id.set == GLSL_SET_DYNAMIC))
			{
				CONSTEVAL_ASSERT(id.binding < 32);
				CONSTEVAL_ASSERT(ids[id.typeId].opcode == SpvOpTypePointer);

				uint32_t typeKind = ids[ids[id.typeId].typeId.value()].opcode.value();
				VkDescriptorType resourceType = getDescriptorType(SpvOp(typeKind));
				CONSTEVAL_ASSERT(resourceType);

				CONSTEVAL_ASSERT((resourceMasks[id.set] & (1 << id.binding)) == 0 || resources[id.set][id.binding] == resourceType);

				resources[id.set][id.binding] = resourceType;
				resourceMasks[id.set] |= 1 << id.binding;
			}

			if (id.opcode == SpvOpVariable && id.storageClass == SpvStorageClassUniformConstant && id.set == GLSL_SET_DESCRIPTOR_ARRAY)
			{
				usesDescriptorArray = true;
			}

			if (id.opcode == SpvOpVariable && id.storageClass == SpvStorageClassPushConstant)
			{
				usesPushConstants = true;
			}
		}

		if (stage == VK_SHADER_STAGE_COMPUTE_BIT)
		{
			if (localSizeIds.x >= 0)
			{
				CONSTEVAL_ASSERT(ids[localSizeIds.x].opcode.value() == SpvOpConstant);
				localSizeIds.x = ids[localSizeIds.x].constant.value();
			}

			if (localSizeIds.y >= 0)
			{
				CONSTEVAL_ASSERT(ids[localSizeIds.y].opcode.value() == SpvOpConstant);
				localSizeIds.y = ids[localSizeIds.y].constant.value();
			}

			if (localSizeIds.z >= 0)
			{
				CONSTEVAL_ASSERT(ids[localSizeIds.z].opcode.value() == SpvOpConstant);
				localSizeIds.z = ids[localSizeIds.z].constant.value();
			}

			CONSTEVAL_ASSERT(localSizeIds.x && localSizeIds.y && localSizeIds.z);
		}
	}

};

template <>
struct Shader<Vk> : MetaShader {
};


}

#endif // GAPI_VK_SHADER_HPP