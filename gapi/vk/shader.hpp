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
#include <utility>
#include <vulkan/vulkan_core.h>

#include <array>

namespace gapi::__private {


struct MetaShader 
{
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
	static constexpr void copyIdT(MetaShader::IdT<DstT>& dst, const MetaShader::IdT<SrcT>& src)
	{
		utils::copy(dst.opcode, src.opcode);
		utils::copy(dst.typeId, src.typeId);
		utils::copy(dst.storageClass, src.storageClass);
		utils::copy(dst.binding, src.binding);
		utils::copy(dst.set, src.set);
		utils::copy(dst.constant, src.constant);
	}

	struct LocalSizeIds {
		int32_t x{-1};
		int32_t y{-1};
		int32_t z{-1};
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
	size_t idx{};
	std::optional<VkShaderStageFlagBits> stage{};
	std::optional<MetaShader::IdOptional> idData{};
	std::optional<MetaShader::LocalSizeIds> localSizeIds{};
};

template <size_t IdBound>
struct ParseBatchData {
	std::array<MetaShader::Id, IdBound> ids{};
	std::optional<MetaShader::LocalSizeIds> localSizeIds{};
	std::optional<VkShaderStageFlagBits> stage{};

	consteval void overwrite(const ParseBatchData& other)
	{
		replaceOptional(stage, other.stage);
		replaceOptional(localSizeIds, other.localSizeIds);

		std::transform(other.ids.begin(), other.ids.end(), ids.begin(), ids.begin(), [](const auto& otherVal, const auto& val) {
			MetaShader::Id res{};
			MetaShader::copyIdT(res, val);
			MetaShader::copyIdT(res, otherVal);
			return res;
		});
	}

	consteval void overwrite(const ParseStepData& parseStepData)
	{
		if (parseStepData.idData.has_value()) {
			MetaShader::copyIdT(
				ids[parseStepData.idx], 
				parseStepData.idData.value()
			);
		}

		replaceOptional(localSizeIds, parseStepData.localSizeIds);
		replaceOptional(stage, parseStepData.stage);
	}

private:
	template <typename T>
	static consteval void replaceOptional(T& dst, const T& src) noexcept
	{
		if (src.has_value())
			dst = src.value();
	}
};

template<size_t Begin, size_t End, size_t IdBound, class F>
constexpr auto static_for(F f) 
{
	ParseBatchData<IdBound> result{};

	if constexpr (Begin < End) 
	{
		constexpr ParseStepData parseStepData = f.template operator()<Begin, End, IdBound>();
		constexpr auto prevBatchData = static_for<Begin + parseStepData.step, End, IdBound>(f);

		result.overwrite(prevBatchData);
		result.overwrite(parseStepData);
	}
	
	return result;
}

template <auto code, size_t Begin, size_t End>
consteval auto getOffsetForBatch() {
	if constexpr (Begin < End) {
		constexpr uint16_t wordCount = read_u16(code.data() + Begin + 2);
		CONSTEVAL_ASSERT(wordCount > 0);
		return getOffsetForBatch<code, Begin + wordCount * 4, End>();
	}
	return Begin;
}

template <auto code, size_t batchSize, size_t startOffset, size_t Begin, size_t End>
consteval auto getOffsetsForBatches() {
	std::array<size_t, End> result{};

	if constexpr (Begin < End) {
		constexpr size_t begin = startOffset;
		constexpr size_t end = std::min(startOffset + batchSize, code.size());
		result[Begin] = startOffset;
		
		constexpr size_t newOffset = getOffsetForBatch<code, begin, end>();
		constexpr auto offsets = getOffsetsForBatches<code, batchSize, newOffset, Begin + 1, End>();
		std::copy(offsets.cbegin() + Begin + 1, offsets.cend(), result.begin() + Begin + 1);
    }

	return result;
}

template <auto code, auto offsets, size_t I, size_t IdBound>
consteval auto parseShaderInBatches() {
	ParseBatchData<IdBound> result{};

	if constexpr (I < offsets.size()) {
		constexpr size_t offsetRangeEnd = I == offsets.size() - 1 ? code.size() : offsets[I + 1];

		result = static_for<offsets[I], offsetRangeEnd, IdBound>([]<size_t Offset, size_t End, size_t _IdBound>()
		{
			ParseStepData result{};
			constexpr size_t mult = sizeof(uint32_t); 
			constexpr size_t offset = Offset;
			constexpr size_t idBound = _IdBound;

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
				CONSTEVAL_ASSERT(id < idBound);

				constexpr uint32_t decoration = read_u32(code.data() + offset + 2 * mult); 
				if constexpr (decoration == SpvDecorationDescriptorSet)
				{
					CONSTEVAL_ASSERT(wordCount == 4);
					result.idx = id;
					result.idData = MetaShader::IdOptional{
						.set = read_u32(code.data() + offset + 3 * mult)
					};
				} else if (decoration == SpvDecorationBinding) 
				{
					CONSTEVAL_ASSERT(wordCount == 4);
					result.idx = id;
					result.idData = MetaShader::IdOptional{
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
				CONSTEVAL_ASSERT(id < idBound);

				result.idx = id;
				result.idData = MetaShader::IdOptional{
					.opcode = opcode
				};
			} else if constexpr (opcode == SpvOpTypePointer)
			{
				CONSTEVAL_ASSERT(wordCount == 4);

				constexpr uint32_t id = read_u32(code.data() + offset + 1 * mult);
				CONSTEVAL_ASSERT(id < idBound);

				result.idx = id;
				result.idData = MetaShader::IdOptional{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 3 * mult),
					.storageClass = read_u32(code.data() + offset + 2 * mult),
				};
			} else if constexpr (opcode == SpvOpConstant)
			{
				CONSTEVAL_ASSERT(wordCount >= 4); // we currently only correctly handle 32-bit integer constants

				constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
				CONSTEVAL_ASSERT(id < idBound);

				result.idx = id;
				result.idData = MetaShader::IdOptional{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 1 * mult),
					.constant = read_u32(code.data() + offset + 3 * mult), // note: this is the value, not the id of the constant
				};
			} else if constexpr (opcode == SpvOpVariable)
			{
				CONSTEVAL_ASSERT(wordCount >= 4);

				constexpr uint32_t id = read_u32(code.data() + offset + 2 * mult);
				CONSTEVAL_ASSERT(id < idBound);

				result.idx = id;
				result.idData = MetaShader::IdOptional{
					.opcode = opcode,
					.typeId = read_u32(code.data() + offset + 1 * mult),
					.storageClass = read_u32(code.data() + offset + 3 * mult),
				};
			}

			return result;
		});

		result.overwrite(parseShaderInBatches<code, offsets, I + 1, IdBound>());
	}

	return result;
}

template <auto code>
consteval auto split_static_for() 
{
	constexpr uint32_t magicNumber = read_u32(code.data());
	CONSTEVAL_ASSERT(magicNumber == SpvMagicNumber);
	
	constexpr auto mult = sizeof(uint32_t);
	constexpr uint32_t idBound = read_u32(code.data() + 3 * mult);
	
	constexpr size_t maxTemplateRecursionDepth = 256;
	constexpr size_t batchCount = std::max(code.size() / maxTemplateRecursionDepth, static_cast<size_t>(1));
	constexpr std::array<size_t, batchCount> offsets = getOffsetsForBatches<code, maxTemplateRecursionDepth, 5 * mult, 0, batchCount>();

	return parseShaderInBatches<code, offsets, 0, idBound>();
}

template <auto code>
	requires utils::isStdArray<std::remove_reference_t<decltype(code)>>::value
consteval Shader<Vk, std::integral_constant<size_t, code.size()>> parseShader() noexcept
{
	Shader<Vk, std::integral_constant<size_t, code.size()>> result;
	std::copy(code.begin(), code.end(), result.spirv.begin());

	constexpr uint32_t magicNumber = read_u32(code.data());
	CONSTEVAL_ASSERT(magicNumber == SpvMagicNumber);

	constexpr auto parsedData = split_static_for<code>();
	
	result.localSizeIds = parsedData.localSizeIds.value_or(MetaShader::LocalSizeIds{});
	result.stage = parsedData.stage.value_or(VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM);
	
	constexpr auto ids = parsedData.ids;
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
		if (result.localSizeIds.x >= 0)
		{
			CONSTEVAL_ASSERT(ids[result.localSizeIds.x].opcode == SpvOpConstant);
			result.localSizeIds.x = ids[result.localSizeIds.x].constant;
		}

		if (result.localSizeIds.y >= 0)
		{
			CONSTEVAL_ASSERT(ids[result.localSizeIds.y].opcode == SpvOpConstant);
			result.localSizeIds.y = ids[result.localSizeIds.y].constant;
		}

		if (result.localSizeIds.z >= 0)
		{
			CONSTEVAL_ASSERT(ids[result.localSizeIds.z].opcode == SpvOpConstant);
			result.localSizeIds.z = ids[result.localSizeIds.z].constant;
		}

		CONSTEVAL_ASSERT(result.localSizeIds.x && result.localSizeIds.y && result.localSizeIds.z);
	}

	return result;
}

}

#endif // GAPI_VK_SHADER_HPP