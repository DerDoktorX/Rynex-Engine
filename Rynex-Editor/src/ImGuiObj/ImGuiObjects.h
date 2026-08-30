#pragma once

struct ImVec2;
namespace Rynex {
	class ImGuiScopeID 
	{
	public:
		ImGuiScopeID(const std::string& name);
		ImGuiScopeID(const char* name);
		ImGuiScopeID(const char* name, uint32_t number);
		~ImGuiScopeID();

		void PushID(const std::string& name);
		void PushID(const char* name);

		void PopID();
	private:
		bool m_Fished;
	};

	enum class ImGuiFlagButten
	{
		None = 0,
		Switch,
		CheckBox,
	};
	class ImGuiFlagsUser
	{
	public:
		struct BitFlag
		{			
			char* name = "Unknown Name";
			int bitMarker = 0;
			ImGuiFlagButten flag;
		};

		struct CheckBoxBool
		{
			char* name = "Unknown Name";
			bool* valuePtr;
		};
	public:
		static bool FlagUI(int& flag, const std::initializer_list<BitFlag>& bitFlagList);

		static bool CheckBoxBoolListUI(std::initializer_list<CheckBoxBool> valuesList);
	};

	class ImGuiPerfomeceRendering
	{
	public:
		struct ElementTransform
		{
			glm::vec2 postion;
			glm::vec2 scale;
		};
	public:
		static glm::vec2 DrawPerformencGrapfs(std::pair<std::string, std::vector<float>>& dataVec);
		static void ResetPos(const glm::vec2& cuserPos);
		static void DrawPerformencGraph(const char* name, std::vector<float>& dataVec);
		static float DrawVerticleBarVec(const char* name, ImVec2 drawSpace, float textSpaceX, std::vector<float>& dataVec, uint32_t firstIndex, uint32_t endIndex, const glm::vec4& color, float textLineHeigth = 0.0);

		static void DrawContectionLine(const ElementTransform& textTransform, const ElementTransform& histrogramTransform, float baseButtemBarHigth, float histrogramBarHigth, const glm::vec4& color);

		
	private:
		static std::tuple<glm::vec2, glm::vec2, float> PlotHistogramGraph(std::vector<float>& dataVec, uint32_t firstIndex, uint32_t endIndex, ImVec2 drawSpace, const glm::vec4& color);
		static std::tuple<glm::vec2, glm::vec2, float> DrawHistogram(std::vector<float>& dataVec, ImVec2 drawSpace, const glm::vec4& color);

		static void DrawQuad(const ImVec2& pLT, const ImVec2& pLB, const ImVec2& pRT, const ImVec2& pRB, const glm::vec4& color);
		static void DrawConection(const ImVec2& pLT, const ImVec2& pLB, const ImVec2& pRT, const ImVec2& pRB, const glm::vec4& color, float offset = 10.0f);

	};


}