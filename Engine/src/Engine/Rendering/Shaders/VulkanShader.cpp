// DO NOT MARK except  CreateDefaultShaderFile, CreateDefaultFragmentFile, CreateDefaultGeometryFile.
//This is because it has been submitted for my COMP305. Link to Original: https://github.falmouth.ac.uk/GA-Undergrad-Student-Work-25-26/Comp305-Engine-SL295211.git

#pragma once
#include "VulkanShader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vulkan/vulkan_core.h>

#include "CorePaths.h"
#include "DataBuffers.h"
#include "DataBuffersVulkan.h"
#include "Transform.h"
#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "VulkanRenderPipeline.h"

namespace Vulkan
{

	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
		delete Pipeline;
	}

	Shader::Shader(const std::string_view& InName, const std::string_view& InStorageLocation)
	{
		StorageLocation = InStorageLocation;

		Name = InName;

		if (!DoesVertexShaderExist())
		{
			const bool bIsSuccessful = CreateDefaultShaderFile();
			if (!bIsSuccessful)
			{
#if DEBUG
				std::cout << "ERROR::SHADER::VERTEX::SHADER NOT CREATED" << '\n';
#endif
				return;
			}
		}

		ShaderStages.Add(CompileVertex());
		ShaderStages.Add(CompileGeometry());
		ShaderStages.Add(CompileFragment());


		Array<VkDescriptorSetLayoutBinding> descriptors;
		for (size_t i = 0; i < 2; i++)
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

		Pipeline = new URenderPipeline(*this, descriptors);

		for (const VkPipelineShaderStageCreateInfo& stage: ShaderStages)
		{
			vkDestroyShaderModule(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), stage.module, nullptr);
		}

		VkDeviceSize GlobalSize = sizeof(GlobalTransforms);

		VkDeviceSize LocalSize = sizeof(PerInstanceTransforms);

		Array<VkDeviceSize> sizes = {GlobalSize, LocalSize};

		size_t index = 0;

		for (const auto& size : sizes)
		{
			uint32_t id;
			::DataBuffers::GenBuffer(id);

			UnifromBufferID.Add(id);
			UniformMappedData.Add(::DataBuffers::GenerateUniformDataBuffer(id, size));

			DataBuffer* buffer = dynamic_cast<DataBuffer*>(::DataBuffers::GetBuffer(id));

			VkDescriptorBufferInfo dBufferInfo{};
			dBufferInfo.buffer = *buffer->Buffers.GetLastPtr();
			dBufferInfo.offset = 0;
			dBufferInfo.range = size;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.pBufferInfo = &dBufferInfo;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.dstSet = Pipeline->GetDescriptorSet();
			descriptorWrite.dstBinding = index;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.pImageInfo = nullptr;
			descriptorWrite.pTexelBufferView = nullptr;
			vkUpdateDescriptorSets(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(),
				1, &descriptorWrite, 0, nullptr);
			index++;
		}

	}

	std::shared_ptr<BaseShader> Shader::CreateVulkanShader(const std::string_view& InName, const std::string_view& InStorageLocation)
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>(InName, InStorageLocation);

		return shader;
	}

	void Shader::Use()
	{
		vkCmdBindPipeline(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline->GetPipeline());

		vkCmdBindDescriptorSets(SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetCurrentBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline->GetPipelineLayout(),
			0, 1, &Pipeline->GetDescriptorSet(), 0, nullptr);
	}

	void Shader::SetUniformBuffer(const size_t& Location, const void* Data, const size_t& Size)
	{
		void* data = UniformMappedData[Location];
		memcpy(data, Data, Size);
	}

	
	void Shader::AddTexture(const Texture InTexture)
	{

		TextureBuffer* buffer = dynamic_cast<TextureBuffer*>(::DataBuffers::GetTexture(InTexture.GetID()));

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = SInstance::GetInstance()->GraphicsCard->GetRenderer()->GetSampler();
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = buffer->ImageViews[0];

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.dstSet = Pipeline->GetDescriptorSet();
		descriptorWrite.dstBinding = 2;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pTexelBufferView = nullptr;

		VkDevice dev = *SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice();

		vkUpdateDescriptorSets(dev,
			1, &descriptorWrite, 0, nullptr);

		Textures.Add(InTexture);
	}

	void Shader::AddTexture(const Array<Texture>& InTexture)
	{
		Textures.Add(InTexture);
	}

	bool Shader::CreateDefaultFragmentFile() const
	{
		std::ofstream FragmentFile;

		FragmentFile.open(GetFragmentLocation().data());

		if (!FragmentFile.is_open())
		{
#if DEBUG
			std::cout << "ERROR::FRAGMENT*::COULD NOT CREATE SHADER AT LOCATION" << std::endl;
#endif
			return false;
		}

		FragmentFile << "#version 330 core\n"
			"out vec4 FragColor;\n"

			"in vec3 VertexColor;\n"

			"in vec2 TexCoord; \n"

			"uniform sampler2D ourTexture0; \n"
			"uniform float Visibility; \n"

			"void main()\n"
			"{\n"
			"   FragColor = texture(ourTexture0, TexCoord) * vec4(VertexColor, Visibility);\n"
			"}\0";

		FragmentFile.close();
		return true;
	}

	void Shader::AddCubeMap(const CubeMap& InMap)
	{
		Map = InMap;
	}

	bool Shader::CreateDefaultShaderFile() const
	{
		std::ofstream ShaderFile;

		ShaderFile.open(GetShaderLocation().data());

		if (!ShaderFile.is_open())
		{
#if DEBUG
			std::cout << "ERROR::SHADER::COULD NOT CREATE SHADER AT LOCATION::" << GetShaderLocation() << std::endl;
#endif
			return false;
		}

		ShaderFile << "#version 330 core\n"
			"layout (location = 0) in vec3 aPos;\n"
			"layout (location = 1) in vec3 aColor;\n"
			"layout (location = 2) in vec2 aTexture;\n"

			"out vec3 VertexColor;\n"

			"out vec4 Pos; \n"

			"out vec2 TexCoord; \n"

			"uniform float offset; \n"

			"uniform mat4 Transform; \n"
			"uniform mat4 Model; \n"
			"uniform mat4 View; \n"
			"uniform mat4 Projection; \n"

			"void main()\n"
			"{\n"
			"   Pos = Projection * View * Model * Transform * vec4(aPos, 1.0);\n"
			"   gl_Position = Pos;\n" //can use negative to reverse
			"	VertexColor = aColor + offset;\n"
			"	TexCoord = aTexture;\n"
			"}\0";

		ShaderFile.close();
		return true;
	}

	bool Shader::CreateDefaultGeometryFile() const
	{
		std::ofstream GeometryFile;

		GeometryFile.open(GetGeometryLocation().data());

		if (!GeometryFile.is_open())
		{
#if DEBUG
			std::cout << "ERROR::SHADER::GEOMETRY::COULD NOT CREATE SHADER AT LOCATION" << std::endl;
#endif
			return false;
		}

		GeometryFile << "#version 460 core\n"
			"layout (triangles) in;\n"
			"layout (triangle_strip, max_vertices = 3) out;\n"

			"void main()\n"
			"{\n"
			"   for (int i = 0; i < 3; i++)\n"
			"   {\n"
			"        gl_Position = gl_in[i].gl_Position;\n" //can use negative to reverse
			"	     EmitVertex();\n"
			"	};\n"
			"   EndPrimitive();\n"
			"}\0";

		GeometryFile.close();
		return true;
	}

	bool Shader::DoesVertexShaderExist() const
	{
		struct stat Buffer;

		const bool bDoesFileExist = stat(GetShaderLocation().data(), &Buffer) == 0;

		return bDoesFileExist;
	}

	bool Shader::DoesGeometryShaderExist() const
	{
		struct stat Buffer;

		const bool bDoesFileExist = stat(GetGeometryLocation().data(), &Buffer) == 0;

		return bDoesFileExist;
	}

	bool Shader::DoesFragmentShaderExist() const
	{
		struct stat Buffer;

		const bool bDoesFileExist = stat(GetFragmentLocation().data(), &Buffer) == 0;

		return bDoesFileExist;
	}

	std::string Shader::GetPathUntyped() const
	{
		return CorePaths::Contents.Path  + StorageLocation + Name;
	}

	std::string Shader::GetShaderLocation() const
	{
		return GetPathUntyped() + "Vert.spv";
	}

	std::string Shader::GetGeometryLocation() const
	{
		return GetPathUntyped() + "Geom.spv";
	}

	std::string Shader::GetFragmentLocation() const
	{
		return GetPathUntyped() + "Frag.spv";
	}

	VkPipelineShaderStageCreateInfo Shader::CompileVertex() const
	{

		const std::string vertexCodeString = ReadFileContents(GetShaderLocation());

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = vertexCodeString.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(vertexCodeString.data());

		VkShaderModule vertShader;

		vkCreateShaderModule(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &createInfo, nullptr, &vertShader);

		VkPipelineShaderStageCreateInfo vertPipelineCreateInfo{};
		vertPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertPipelineCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertPipelineCreateInfo.module = vertShader;
		vertPipelineCreateInfo.pName = "main";

		return vertPipelineCreateInfo;
	}

	VkPipelineShaderStageCreateInfo Shader::CompileGeometry() const
	{
		const std::string geomCodeString = ReadFileContents(GetGeometryLocation());

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = geomCodeString.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(geomCodeString.data());

		VkShaderModule geomShader;

		vkCreateShaderModule(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &createInfo, nullptr, &geomShader);

		VkPipelineShaderStageCreateInfo geomPipelineCreateInfo{};
		geomPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		geomPipelineCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		geomPipelineCreateInfo.module = geomShader;
		geomPipelineCreateInfo.pName = "main";

		return geomPipelineCreateInfo;
	}

	VkPipelineShaderStageCreateInfo Shader::CompileFragment() const
	{
		const std::string fragCodeString = ReadFileContents(GetFragmentLocation());

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = fragCodeString.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(fragCodeString.data());

		VkShaderModule fragShader;

		vkCreateShaderModule(*SInstance::GetInstance()->GraphicsCard->GetLogicalDevice()->GetVulkanLogicalDevice(), &createInfo, nullptr, &fragShader);

		VkPipelineShaderStageCreateInfo fragPipelineCreateInfo{};
		fragPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragPipelineCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragPipelineCreateInfo.module = fragShader;
		fragPipelineCreateInfo.pName = "main";

		return fragPipelineCreateInfo;
	}

	std::string Shader::ReadFileContents(const std::string_view& Location) const
	{
		std::ifstream File(Location.data(), std::ios::binary);
		File.open(Location.data());
		std::stringstream Buffer;
		Buffer << File.rdbuf();
		File.close();

		const std::string text = Buffer.str();

		return text;
	}
}
