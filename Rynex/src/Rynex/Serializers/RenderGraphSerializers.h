#pragma once

namespace Rynex {
	class RenderGraph;

	class RenderGraphSerializers
	{
	public:
		RenderGraphSerializers();

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		Ref<RenderGraph> m_RenderGraph;
	};

}

