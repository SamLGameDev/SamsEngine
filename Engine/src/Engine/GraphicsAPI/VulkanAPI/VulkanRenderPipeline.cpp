#include "VulkanRenderPipeline.h"
#include "VulkanLogicalDevice.h"
#include "VulkanGraphicsCard.h"
#define GLFW_INCLUDE_VULKAN
#include <iostream>
#include <GLFW/glfw3.h>

#include "Vector2D.h"
#include "VulkanImageView.h"
#include "VulkanInstance.h"
#include "Window.h"
#include "VulkanImageView.h"
#include "VulkanShader.h"


namespace Vulkan
{
	URenderPipeline::URenderPipeline(Shader& InShader)
	{
		Init(InShader);
	}

	URenderPipeline::~URenderPipeline()
	{
		ShutDown();
	}

	ErrorCodes URenderPipeline::Init(Shader& InShader)
	{
		Array<VkDescriptorSetLayoutBinding> descriptors;
		for (size_t i=  0; i < 2 ; i++)
		{
			VkDescriptorSetLayoutBinding descriptor{};
			descriptor.binding = i;
			descriptor.descriptorCount = 1;
			descriptor.pImmutableSamplers = nullptr;
			descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptors.Add(descriptor);
		}
		VkDescriptorSetLayoutBinding descriptor{};
		descriptor.binding = 2;
		descriptor.descriptorCount = 1;
		descriptor.pImmutableSamplers = nullptr;
		descriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptor.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		descriptors.Add(descriptor);

	

		VkDescriptorSetLayoutCreateInfo descriptorLayoutCreateInfo{};
		descriptorLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorLayoutCreateInfo.bindingCount = descriptors.GetSize();
		descriptorLayoutCreateInfo.pBindings = descriptors.GetFirstRef();

		vkCreateDescriptorSetLayout(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &descriptorLayoutCreateInfo, nullptr, &DescriptorLayout);

		VkDescriptorSetAllocateInfo dAllocInfo{};
		dAllocInfo.descriptorPool = *SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetDescriptorPool();
		dAllocInfo.descriptorSetCount = 1;
		dAllocInfo.pSetLayouts = &DescriptorLayout;
		dAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		VkResult rs = vkAllocateDescriptorSets(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
			&dAllocInfo, &DescriptorSet);
		if ( rs != VK_SUCCESS)
		{
			std::cout << rs << " ERROR::VULKAN::RENDERPIPELINE::FAILED TO ALLOCATE DESCRIPTOR SETS" << std::endl;
		}

		Array<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT, VK_DYNAMIC_STATE_VERTEX_INPUT_EXT };

		VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo{};
		dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStatesCreateInfo.dynamicStateCount = dynamicStates.GetSize();
		dynamicStatesCreateInfo.pDynamicStates = dynamicStates.GetFirstRef();

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = nullptr;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
		inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent().width;
		viewport.height = SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent().height;
		viewport.minDepth = 0;
		viewport.maxDepth = 1;

		VkRect2D scissor;
		scissor.extent = SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetSwapChain()->GetSwapChainExtent();
		scissor.offset = { 0, 0 };


		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

		//Dont need these if using dynamic state
		//viewportStateCreateInfo.pViewports = &viewport;
		//viewportStateCreateInfo.pScissors = &scissor;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.scissorCount = 1;


		VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
		rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationCreateInfo.lineWidth = 1;
		rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationCreateInfo.depthBiasConstantFactor = 0;
		rasterizationCreateInfo.depthBiasSlopeFactor = 0;
		rasterizationCreateInfo.depthBiasClamp = 0;

		VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
		multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleCreateInfo.minSampleShading = 1;
		multisampleCreateInfo.pSampleMask = nullptr;
		multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentState.blendEnable = VK_FALSE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.blendConstants[0] = 0;
		colorBlendStateCreateInfo.blendConstants[1] = 0;
		colorBlendStateCreateInfo.blendConstants[2] = 0;
		colorBlendStateCreateInfo.blendConstants[3] = 0;

		VkPipelineLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = nullptr;
		layoutCreateInfo.pSetLayouts = &DescriptorLayout;

		vkCreatePipelineLayout(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &layoutCreateInfo, nullptr, &Layout);

		VkPipelineDepthStencilStateCreateInfo depthCreateInfo{};
		depthCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthCreateInfo.depthTestEnable = VK_TRUE;
		depthCreateInfo.depthWriteEnable = VK_TRUE;
		depthCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthCreateInfo.minDepthBounds = 0;
		depthCreateInfo.maxDepthBounds = 1;
		depthCreateInfo.stencilTestEnable = VK_FALSE;
		depthCreateInfo.front = {};
		depthCreateInfo.back = {};

		VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = InShader.GetShaderStages().GetSize();
		pipelineCreateInfo.pStages = InShader.GetShaderStages().GetFirstRef();
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = &depthCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStatesCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
		pipelineCreateInfo.layout = Layout;
		pipelineCreateInfo.renderPass = SInstance::GetInstance()->RenderPass->GetVulkanRenderPass();
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		vkCreateGraphicsPipelines(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &Pipeline);
		return SUCCEEDED;
	}

	ErrorCodes URenderPipeline::ShutDown()
	{
		vkDestroyDescriptorSetLayout(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), DescriptorLayout, nullptr);
		vkDestroyPipeline(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), Pipeline, nullptr);
		vkDestroyPipelineLayout(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), Layout, nullptr);
		return SUCCEEDED;
	}
}
