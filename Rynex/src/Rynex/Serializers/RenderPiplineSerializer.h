#pragma once

namespace Rynex {
	class RenderPipline;

	class RenderPiplineSerializer
	{
	public:
		RenderPiplineSerializer(const Ref<RenderPipline>& renderPipline);

		bool Serlize(const std::filesystem::path& path);
		bool Deserlize(const std::filesystem::path& path);

	private:
		Ref<RenderPipline> m_RenderPipline;

	};

}