#include "shader.hpp"

#include <cstdint>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

#include <string>
#include <vector>


namespace gapi::__private {

// static uint32_t gatherResources(Shaders shaders, VkDescriptorType (&resourceTypes)[32])
// {
// 	uint32_t resourceMask = 0;

// 	for (const Shader* shader : shaders)
// 	{
// 		for (uint32_t i = 0; i < 32; ++i)
// 		{
// 			if (shader->resourceMask & (1 << i))
// 			{
// 				if (resourceMask & (1 << i))
// 				{
// 					ASSERT(resourceTypes[i] == shader->resourceTypes[i]);
// 				}
// 				else
// 				{
// 					resourceTypes[i] = shader->resourceTypes[i];
// 					resourceMask |= 1 << i;
// 				}
// 			}
// 		}
// 	}

// 	return resourceMask;
// }

// static VkDescriptorSetLayout createSetLayout(VkDevice device, Shaders shaders)
// {
// 	std::vector<VkDescriptorSetLayoutBinding> setBindings;

// 	VkDescriptorType resourceTypes[32] = {};
// 	uint32_t resourceMask = gatherResources(shaders, resourceTypes);

// 	for (uint32_t i = 0; i < 32; ++i)
// 		if (resourceMask & (1 << i))
// 		{
// 			VkDescriptorSetLayoutBinding binding = {};
// 			binding.binding = i;
// 			binding.descriptorType = resourceTypes[i];
// 			binding.descriptorCount = 1;

// 			binding.stageFlags = 0;
// 			for (const Shader* shader : shaders)
// 				if (shader->resourceMask & (1 << i))
// 					binding.stageFlags |= shader->stage;

// 			setBindings.push_back(binding);
// 		}

// 	VkDescriptorSetLayoutCreateInfo setCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
// 	setCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT;
// 	setCreateInfo.bindingCount = uint32_t(setBindings.size());
// 	setCreateInfo.pBindings = setBindings.data();

// 	VkDescriptorSetLayout setLayout = 0;
// 	VK_CHECK(vkCreateDescriptorSetLayout(device, &setCreateInfo, 0, &setLayout));

// 	return setLayout;
// }

// static VkPipelineLayout createPipelineLayout(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorSetLayout arrayLayout, VkShaderStageFlags pushConstantStages, size_t pushConstantSize)
// {
// 	VkDescriptorSetLayout layouts[2] = { setLayout, arrayLayout };

// 	VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
// 	createInfo.setLayoutCount = arrayLayout ? 2 : 1;
// 	createInfo.pSetLayouts = layouts;

// 	VkPushConstantRange pushConstantRange = {};

// 	if (pushConstantSize)
// 	{
// 		pushConstantRange.stageFlags = pushConstantStages;
// 		pushConstantRange.size = uint32_t(pushConstantSize);

// 		createInfo.pushConstantRangeCount = 1;
// 		createInfo.pPushConstantRanges = &pushConstantRange;
// 	}

// 	VkPipelineLayout layout = 0;
// 	VK_CHECK(vkCreatePipelineLayout(device, &createInfo, 0, &layout));

// 	return layout;
// }

// static VkDescriptorUpdateTemplate createUpdateTemplate(VkDevice device, VkPipelineBindPoint bindPoint, VkPipelineLayout layout, Shaders shaders, uint32_t* pushDescriptorCount)
// {
// 	std::vector<VkDescriptorUpdateTemplateEntry> entries;

// 	VkDescriptorType resourceTypes[32] = {};
// 	uint32_t resourceMask = gatherResources(shaders, resourceTypes);

// 	for (uint32_t i = 0; i < 32; ++i)
// 		if (resourceMask & (1 << i))
// 		{
// 			VkDescriptorUpdateTemplateEntry entry = {};
// 			entry.dstBinding = i;
// 			entry.dstArrayElement = 0;
// 			entry.descriptorCount = 1;
// 			entry.descriptorType = resourceTypes[i];
// 			entry.offset = sizeof(DescriptorInfo) * i;
// 			entry.stride = sizeof(DescriptorInfo);

// 			entries.push_back(entry);
// 		}

// 	*pushDescriptorCount = uint32_t(entries.size());

// 	VkDescriptorUpdateTemplateCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_UPDATE_TEMPLATE_CREATE_INFO };

// 	createInfo.descriptorUpdateEntryCount = uint32_t(entries.size());
// 	createInfo.pDescriptorUpdateEntries = entries.data();

// 	createInfo.templateType = VK_DESCRIPTOR_UPDATE_TEMPLATE_TYPE_PUSH_DESCRIPTORS;
// 	createInfo.pipelineBindPoint = bindPoint;
// 	createInfo.pipelineLayout = layout;

// 	VkDescriptorUpdateTemplate updateTemplate = 0;
// 	VK_CHECK(vkCreateDescriptorUpdateTemplate(device, &createInfo, 0, &updateTemplate));

// 	return updateTemplate;
// }

// static VkSpecializationInfo fillSpecializationInfo(std::vector<VkSpecializationMapEntry>& entries, const Constants& constants)
// {
// 	for (size_t i = 0; i < constants.size(); ++i)
// 		entries.push_back({ uint32_t(i), uint32_t(i * 4), 4 });

// 	VkSpecializationInfo result = {};
// 	result.mapEntryCount = uint32_t(entries.size());
// 	result.pMapEntries = entries.data();
// 	result.dataSize = constants.size() * sizeof(int);
// 	result.pData = constants.begin();

// 	return result;
// }

// VkPipeline createGraphicsPipeline(VkDevice device, VkPipelineCache pipelineCache, const VkPipelineRenderingCreateInfo& renderingInfo, const Program& program, Constants constants)
// {
// 	std::vector<VkSpecializationMapEntry> specializationEntries;
// 	VkSpecializationInfo specializationInfo = fillSpecializationInfo(specializationEntries, constants);

// 	VkGraphicsPipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };

// 	std::vector<VkPipelineShaderStageCreateInfo> stages(program.shaderCount);
// 	std::vector<VkShaderModuleCreateInfo> modules(program.shaderCount);
// 	for (size_t i = 0; i < program.shaderCount; ++i)
// 	{
// 		const Shader* shader = program.shaders[i];

// 		VkShaderModuleCreateInfo& module = modules[i];
// 		module.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
// 		module.codeSize = shader->spirv.size(); // note: this needs to be a number of bytes!
// 		module.pCode = reinterpret_cast<const uint32_t*>(shader->spirv.data());

// 		VkPipelineShaderStageCreateInfo& stage = stages[i];
// 		stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
// 		stage.stage = shader->stage;
// 		stage.pName = "main";
// 		stage.pSpecializationInfo = &specializationInfo;
// 		stage.pNext = &module;
// 	}

// 	createInfo.stageCount = uint32_t(stages.size());
// 	createInfo.pStages = stages.data();

// 	VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
// 	createInfo.pVertexInputState = &vertexInput;

// 	VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
// 	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
// 	createInfo.pInputAssemblyState = &inputAssembly;

// 	VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
// 	viewportState.viewportCount = 1;
// 	viewportState.scissorCount = 1;
// 	createInfo.pViewportState = &viewportState;

// 	VkPipelineRasterizationStateCreateInfo rasterizationState = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
// 	rasterizationState.lineWidth = 1.f;
// 	rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
// 	rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
// 	rasterizationState.depthBiasEnable = true;
// 	createInfo.pRasterizationState = &rasterizationState;

// 	VkPipelineMultisampleStateCreateInfo multisampleState = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
// 	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
// 	createInfo.pMultisampleState = &multisampleState;

// 	VkPipelineDepthStencilStateCreateInfo depthStencilState = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
// 	depthStencilState.depthTestEnable = true;
// 	depthStencilState.depthWriteEnable = true;
// 	depthStencilState.depthCompareOp = VK_COMPARE_OP_GREATER;
// 	createInfo.pDepthStencilState = &depthStencilState;

// 	VkPipelineColorBlendAttachmentState colorAttachmentStates[8] = {};
// 	ASSERT(renderingInfo.colorAttachmentCount <= COUNTOF(colorAttachmentStates));
// 	for (uint32_t i = 0; i < renderingInfo.colorAttachmentCount; ++i)
// 		colorAttachmentStates[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

// 	VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
// 	colorBlendState.attachmentCount = renderingInfo.colorAttachmentCount;
// 	colorBlendState.pAttachments = colorAttachmentStates;
// 	createInfo.pColorBlendState = &colorBlendState;

// 	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_DEPTH_BIAS };

// 	VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
// 	dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
// 	dynamicState.pDynamicStates = dynamicStates;
// 	createInfo.pDynamicState = &dynamicState;

// 	createInfo.layout = program.layout;
// 	createInfo.pNext = &renderingInfo;

// 	VkPipeline pipeline = 0;
// 	VK_CHECK(vkCreateGraphicsPipelines(device, pipelineCache, 1, &createInfo, 0, &pipeline));

// 	if (vkSetDebugUtilsObjectNameEXT)
// 	{
// 		std::string name;

// 		for (size_t i = 0; i < program.shaderCount; ++i)
// 		{
// 			const Shader* shader = program.shaders[i];

// 			name += shader->name;
// 			if (i + 1 < program.shaderCount)
// 				name += " / ";
// 		}

// 		VkDebugUtilsObjectNameInfoEXT nameInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
// 		nameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
// 		nameInfo.objectHandle = uint64_t(pipeline);
// 		nameInfo.pObjectName = name.c_str();
// 		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
// 	}

// 	return pipeline;
// }

// VkPipeline createComputePipeline(VkDevice device, VkPipelineCache pipelineCache, const Program& program, Constants constants)
// {
// 	ASSERT(program.shaderCount == 1);
// 	const Shader& shader = *program.shaders[0];

// 	ASSERT(shader.stage == VK_SHADER_STAGE_COMPUTE_BIT);

// 	VkComputePipelineCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };

// 	std::vector<VkSpecializationMapEntry> specializationEntries;
// 	VkSpecializationInfo specializationInfo = fillSpecializationInfo(specializationEntries, constants);

// 	VkShaderModuleCreateInfo module = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
// 	module.codeSize = shader.spirv.size(); // note: this needs to be a number of bytes!
// 	module.pCode = reinterpret_cast<const uint32_t*>(shader.spirv.data());

// 	VkPipelineShaderStageCreateInfo stage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
// 	stage.stage = shader.stage;
// 	stage.pName = "main";
// 	stage.pSpecializationInfo = &specializationInfo;
// 	stage.pNext = &module;

// 	createInfo.stage = stage;
// 	createInfo.layout = program.layout;

// 	VkPipeline pipeline = 0;
// 	VK_CHECK(vkCreateComputePipelines(device, pipelineCache, 1, &createInfo, 0, &pipeline));

// 	if (vkSetDebugUtilsObjectNameEXT)
// 	{
// 		VkDebugUtilsObjectNameInfoEXT nameInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
// 		nameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
// 		nameInfo.objectHandle = uint64_t(pipeline);
// 		nameInfo.pObjectName = shader.name.c_str();
// 		vkSetDebugUtilsObjectNameEXT(device, &nameInfo);
// 	}

// 	return pipeline;
// }

// Program createProgram(VkDevice device, VkPipelineBindPoint bindPoint, Shaders shaders, size_t pushConstantSize, VkDescriptorSetLayout arrayLayout)
// {
// 	VkShaderStageFlags pushConstantStages = 0;
// 	for (const Shader* shader : shaders)
// 		if (shader->usesPushConstants)
// 			pushConstantStages |= shader->stage;

// 	bool usesDescriptorArray = false;
// 	for (const Shader* shader : shaders)
// 		usesDescriptorArray |= shader->usesDescriptorArray;

// 	ASSERT(!usesDescriptorArray || arrayLayout);

// 	Program program = {};

// 	program.bindPoint = bindPoint;

// 	program.setLayout = createSetLayout(device, shaders);
// 	ASSERT(program.setLayout);

// 	program.layout = createPipelineLayout(device, program.setLayout, arrayLayout, pushConstantStages, pushConstantSize);
// 	ASSERT(program.layout);

// 	program.updateTemplate = createUpdateTemplate(device, bindPoint, program.layout, shaders, &program.pushDescriptorCount);
// 	ASSERT(program.updateTemplate);

// 	program.pushConstantStages = pushConstantStages;
// 	program.pushConstantSize = uint32_t(pushConstantSize);

// 	const Shader* shader = shaders.size() == 1 ? *shaders.begin() : nullptr;

// 	if (shader && shader->stage == VK_SHADER_STAGE_COMPUTE_BIT)
// 	{
// 		program.localSizeX = shader->localSizeX;
// 		program.localSizeY = shader->localSizeY;
// 		program.localSizeZ = shader->localSizeZ;
// 	}

// 	memset(program.shaders, 0, sizeof(program.shaders));
// 	program.shaderCount = 0;

// 	for (const Shader* shader : shaders)
// 		program.shaders[program.shaderCount++] = shader;

// 	return program;
// }

// void destroyProgram(VkDevice device, const Program& program)
// {
// 	vkDestroyDescriptorUpdateTemplate(device, program.updateTemplate, 0);
// 	vkDestroyPipelineLayout(device, program.layout, 0);
// 	vkDestroyDescriptorSetLayout(device, program.setLayout, 0);
// }

// VkDescriptorSetLayout createDescriptorArrayLayout(VkDevice device)
// {
// 	VkShaderStageFlags stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT;
// 	VkDescriptorSetLayoutBinding setBinding = { 0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, DESCRIPTOR_LIMIT, stageFlags, nullptr };

// 	VkDescriptorBindingFlags bindingFlags = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
// 	VkDescriptorSetLayoutBindingFlagsCreateInfo setBindingFlags = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
// 	setBindingFlags.bindingCount = 1;
// 	setBindingFlags.pBindingFlags = &bindingFlags;

// 	VkDescriptorSetLayoutCreateInfo setCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
// 	setCreateInfo.pNext = &setBindingFlags;
// 	setCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
// 	setCreateInfo.bindingCount = 1;
// 	setCreateInfo.pBindings = &setBinding;

// 	VkDescriptorSetLayout setLayout = 0;
// 	VK_CHECK(vkCreateDescriptorSetLayout(device, &setCreateInfo, 0, &setLayout));

// 	return setLayout;
// }

// std::pair<VkDescriptorPool, VkDescriptorSet> createDescriptorArray(VkDevice device, VkDescriptorSetLayout layout, uint32_t descriptorCount)
// {
// 	VkDescriptorPoolSize poolSize = { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount };
// 	VkDescriptorPoolCreateInfo poolInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
// 	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
// 	poolInfo.maxSets = 1;
// 	poolInfo.poolSizeCount = 1;
// 	poolInfo.pPoolSizes = &poolSize;

// 	VkDescriptorPool pool = nullptr;
// 	VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, 0, &pool));

// 	VkDescriptorSetVariableDescriptorCountAllocateInfo setAllocateCountInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO };
// 	setAllocateCountInfo.descriptorSetCount = 1;
// 	setAllocateCountInfo.pDescriptorCounts = &descriptorCount;

// 	VkDescriptorSetAllocateInfo setAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
// 	setAllocateInfo.pNext = &setAllocateCountInfo;
// 	setAllocateInfo.descriptorPool = pool;
// 	setAllocateInfo.descriptorSetCount = 1;
// 	setAllocateInfo.pSetLayouts = &layout;

// 	VkDescriptorSet set = 0;
// 	VK_CHECK(vkAllocateDescriptorSets(device, &setAllocateInfo, &set));

// 	return std::make_pair(pool, set);
// }

}