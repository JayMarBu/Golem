#pragma once
#include "golpch.h"
#include <filesystem>
#include <shaderc/shaderc.hpp>

namespace golem
{
	std::vector<char> ReadShaderFile(const std::string& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			//throw std::runtime_error("failed to open file: " + filepath);
			std::vector<char> buffer{};
			return buffer;
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}

	std::vector<std::string> GetFilesWithExtension(std::string in_directory, std::string in_ext)
	{
		std::vector<std::string> files;
		for (auto& p : std::filesystem::recursive_directory_iterator(in_directory))
		{
			if (p.path().extension() == in_ext)
				files.push_back(p.path().stem().string());
		}
		return files;
	}

	bool CompileShader(std::string in_shader, shaderc_shader_kind in_type, std::vector<uint32_t>& out_shaderCode)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		GOL_CORE_INFO("attempting to compile shader : {0}", in_shader);

		std::vector<char> ShaderSource = ReadShaderFile(in_shader);
		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(ShaderSource.data(), ShaderSource.size(), in_type, in_shader.c_str(), options);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			GOL_CORE_ERROR("failed to compile shader: [{0}] : {1}", in_shader, result.GetErrorMessage());
			return false;
		}

		out_shaderCode.assign(result.begin(), result.end());
		return true;
	}
}