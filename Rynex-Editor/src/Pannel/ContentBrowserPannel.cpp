#include "rypch.h"
#include "ContentBrowserPannel.h"

#include <imgui/imgui.h>

namespace Rynex {

	extern const std::filesystem::path g_AssetsPath = "Assets";
	const int s_MaxFileLength = 12;

	ContentBrowserPannel::ContentBrowserPannel()
		: m_BaseDirectory(g_AssetsPath)
		,m_CurrentDirectory(m_BaseDirectory)
	{
		m_DirectoryIcon		= Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		//Files
		m_FileIconDefault	= Texture2D::Create("Resources/Icons/ContentBrowser/FileIconDefault.png");
		m_FileIconError		= Texture2D::Create("Resources/Icons/ContentBrowser/FileIconError.png");
		m_FileIconScene		= Texture2D::Create("Resources/Icons/ContentBrowser/FileIconScene.png");
		m_FileIconShader	= Texture2D::Create("Resources/Icons/ContentBrowser/FileIconShader.png");
		m_FileIconTexture	= Texture2D::Create("Resources/Icons/ContentBrowser/FileIconTexture.png");
	}

	void ContentBrowserPannel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		static float padding = 16.0f;
		static float thumbernailSize = 128.0f;
		float cellSize = thumbernailSize + padding;
		
		float pannelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(pannelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;
		
		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{	
			const auto& path = directoryEntry.path();
			auto realtivPath = std::filesystem::relative(path, g_AssetsPath);
			std::string fileNameString = path.filename().string();
				
			
			ImGui::PushID(fileNameString.c_str());
			FileFormate(directoryEntry);
			

			if (ImGui::BeginDragDropSource())
			{
				//std::filesystem::path realtivPath(path);
				const wchar_t* itemPath = realtivPath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
				
			}
			//RY_CORE_INFO("after drag drop ContentBrowserPannel");
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
				
			ImGui::TextWrapped(fileNameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
			
		}

		ImGui::End();
	}

	static ImVec4 GetFileStateColor(FileStats state)
	{
		switch (state)
		{
			case FileStats::Error:		return ImVec4(0.85, 0.05, 0.1, 1.0);
			case FileStats::Update:		return ImVec4(0.3, 0.5, 0.7, 1.0);
			case FileStats::Change:		return ImVec4(0.2, 0.5, 0.8, 1.0);
			case FileStats::Ready:		return ImVec4(0.2, 0.8, 0.3, 1.0);
			case FileStats::Wahrning:	return ImVec4(0.8, 0.7, 0.1, 1.0);
			case FileStats::None:		return ImVec4(1.0, 1.0, 0.5, 1.0);
			default:
				break;
		}
		RY_CORE_ASSERT(false, "Error not defined FileStats: ContentBrowserPannel::SetFileStateColor!");
		return ImVec4(0, 0, 0, 1);
	}

	static FileFormats GetFileFormate(const std::filesystem::path& filePath)
	{
		std::filesystem::path extension = filePath.filename().extension();

		if (extension == ".png") return FileFormats::png;
		if (extension == ".txt") return FileFormats::txt;
		if (extension == ".glsl") return FileFormats::glsl;
		if (extension == ".rynexscene") return FileFormats::rynexscene;


		return FileFormats::none;
	}

#if 0
	static Ref<Texture2D> GetFileIconeTexture(FileFormats format)
	{
		switch (format)
		{
		case FileFormats::png:		return;
		case FileFormats::txt:		return;
		}


		return Ref<Texture2D>();
	}
#endif // TODO: Make Separte Funktion for Textur

	void ContentBrowserPannel::FileFormate(const std::filesystem::directory_entry& directoryEntry, FileFormats formate) const
	{
		FileStats state = FileStats::None;
		static float padding = 16.0f;
		static float thumbernailSize = 128.0f;
		Ref<Texture2D> icon;
		if (directoryEntry.is_directory())
		{
			icon = m_DirectoryIcon;
		}
		else
		{
			const auto& path = directoryEntry.path();
			switch (GetFileFormate(path))
			{
				case FileFormats::png: 
			{
				icon = m_FileIconTexture; 
				break;
			}
				case FileFormats::glsl:
			{
				icon = m_FileIconShader;
				break;
			}
				case FileFormats::rynexscene:
			{
				icon = m_FileIconScene;
				break;
			}
				default:
				{
					icon = m_FileIconDefault;
					break;
				}
			}
		}
		 
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton((ImTextureID)icon->GetRenderID(), { thumbernailSize , thumbernailSize }, { 0, 1 }, { 1, 0 },-1, ImVec4(0, 0, 0, 0), directoryEntry.is_directory() ? ImVec4(1, 1, 1, 1) : GetFileStateColor(state));
	}

}

