#include <rypch.h>
#include "ImGuiObjects.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace Rynex {
	namespace Utils {


		static glm::vec2 ConvertImVecInGlm(const ImVec2& value)
		{
			return glm::vec2{ value.x, value.y };
		}

		static glm::vec4 ConvertImVecInGlm(const ImVec4& value)
		{
			return glm::vec4{ value.x, value.y, value.z, value.w };
		}


		static ImVec2 ConvertGlmInImVec(const glm::vec2& value)
		{
			return ImVec2{ value.x, value.y };
		}

		static ImVec4 ConvertGlmInImVec(const glm::vec4& value)
		{
			return ImVec4{ value.x, value.y, value.z, value.w };
		}

	}

	ImGuiScopeID::ImGuiScopeID(const std::string& name)
		: m_Fished(true)
	{
		PushID(name.c_str());
	}

	ImGuiScopeID::ImGuiScopeID(const char* name)
		: m_Fished(true)
	{
		PushID(name);
	}

	ImGuiScopeID::ImGuiScopeID(const char* name, uint32_t number)
		: m_Fished(true)
	{
		PushID(name);
	}

	ImGuiScopeID::~ImGuiScopeID()
	{
		PopID();
	}

	void ImGuiScopeID::PushID(const char* name)
	{
		if (m_Fished)
		{
			ImGui::PushID(name);
			m_Fished = false;
		}
	}

	void ImGuiScopeID::PushID(const std::string& name)
	{
		if (m_Fished)
		{
			ImGui::PushID(name.c_str());
			m_Fished = false;
		}

	}

	void ImGuiScopeID::PopID()
	{
		if (!m_Fished)
		{
			ImGui::PopID();
			m_Fished = true;
		}
	}


	bool ImGuiFlagsUser::FlagUI(int& flag, const std::initializer_list<BitFlag>& bitFlagList)
	{
		RY_CORE_NOT_IMPL();
		return false;
	}

	bool ImGuiFlagsUser::CheckBoxBoolListUI(std::initializer_list<CheckBoxBool> valuesList)
	{
		uint32_t i = 0u;
		bool input = false;
		for (const CheckBoxBool& chexBoxData : valuesList)
		{
			ImGuiScopeID Id("CheckBox " + std::to_string(i));
			input = ImGui::Checkbox(chexBoxData.name, chexBoxData.valuePtr) || input;

			i++;
		}
		return input;

	}

	
	glm::vec2 ImGuiPerfomeceRendering::DrawPerformencGrapfs(std::pair<std::string, std::vector<float>>& dataVec)
	{
		ImVec2 region = ImGui::GetContentRegionAvail();
		

		return glm::vec2();
	}

	void ImGuiPerfomeceRendering::ResetPos(const glm::vec2& cuserPos)
	{
		ImVec2 curserPosImVec2 = ImVec2{ cuserPos.x,cuserPos.y };
		ImGui::SetCursorPos(curserPosImVec2);
	}

	void ImGuiPerfomeceRendering::DrawPerformencGraph(const char* name, std::vector<float>& dataVec)
	{
		float lineHeight = ImGui::GetTextLineHeight();

		ImVec2 pos = ImGui::GetCursorPos();



		ImGui::Text("%s", name);
		ImGui::SameLine();
		ImGui::Text("%.2fms", dataVec.back());
		float frameSpacing = ImGui::GetFrameHeightWithSpacing();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + lineHeight + frameSpacing);

	}

	float ImGuiPerfomeceRendering::DrawVerticleBarVec(const char* diplayText, ImVec2 drawSpace, float textSpaceX, std::vector<float>& dataVec, uint32_t firstIndex, uint32_t endIndex, const glm::vec4& color, float textLineHeigth)
	{
		ImGuiScopeID id(diplayText);
#if 0
		constexpr double maxGraph = (1.0 / 50.0) * 1000.0;
		constexpr double minGraph = 0.0f;
		constexpr double absGraph = glm::abs(maxGraph - minGraph);
		constexpr int offset = 0;

		float lastValue = dataVec.back();

#if 0
		ImVec2 curserPosBefor = ImGui::GetCursorScreenPos();
#else
		glm::vec2 curserPosBefor = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());
#endif
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Utils::ConvertGlmInImVec(color));

		ImGui::PlotHistogram("##Frame Histogram", dataVec.data(), dataVec.size(), offset, nullptr, minGraph, maxGraph, drawSpace);

		ImGui::PopStyleColor();

		ImGuiContext* context = ImGui::GetCurrentContext();
		const ImGuiStyle& style = context->Style;
		
#if 1
		glm::vec2 curserPosAfter = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());
		ImGui::SameLine();
		curserPosAfter.x = ImGui::GetCursorScreenPos().x;


		glm::vec2 framePadding = Utils::ConvertImVecInGlm(style.FramePadding);

		glm::vec2 histoGramPos = curserPosBefor - framePadding;
		glm::vec2 histoGramScale = curserPosAfter - curserPosBefor;
		histoGramScale -= 1.0f * framePadding;
#else
		ImVec2 curserPosAfter = ImGui::GetCursorScreenPos();
		ImGui::SameLine();
		curserPosAfter.x = ImGui::GetCursorScreenPos().x;


		glm::vec2 histoGramPos = {
			curserPosBefor.x - style.FramePadding.x, curserPosBefor.y - style.FramePadding.y
		};
		glm::vec2 histoGramScale = {
			(curserPosAfter.x - curserPosBefor.x) - (1.0f * style.FramePadding.x),
			(curserPosAfter.y - curserPosBefor.y) - (1.0f * style.FramePadding.y)
		};
#endif
		// histoGramScale -= offsetVisuelPixel * 2.0;
		double graphHigth = lastValue - minGraph;
		double inveredGraphHigth = maxGraph - graphHigth;

		double absUnitHigth = static_cast<double>(histoGramScale.y) / absGraph;
		// double minLastHeithShow = absUnitHigth * inveredGraphHigth;
		double minLastHeithShow = absUnitHigth * graphHigth;

		float lastHeithShow = curserPosAfter.y - minLastHeithShow;
		float xPosLeft = histoGramPos.x + histoGramScale.x;
		float xPosRigthe = xPosLeft + textSpaceX * 5.0f / 8.0f;

#else
		glm::vec2 curserPosBefor = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());
		auto[lastEllementBottemPos, histoGramScale, lastElementTopPosY] = PlotHistogramGraph(dataVec,firstIndex, endIndex, drawSpace, color);
		float& lastHeithShow = lastElementTopPosY;
		glm::vec2 curserPosAfter = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());

		const ImGuiStyle& style = ImGui::GetStyle();
		glm::vec2 framePadding = Utils::ConvertImVecInGlm(style.FramePadding);

		glm::vec2 histoGramPos = curserPosBefor - framePadding;

		const float& xPosLeft = lastEllementBottemPos.x;
		const float yPosText = curserPosBefor.y + textLineHeigth;
		const float yPosTextLine = yPosText + style.ItemSpacing.y;


		float xPosRigthe = xPosLeft + textSpaceX *  5.0f / 8.0f;
		float yPosRigthe = xPosLeft + textSpaceX * 5.0f / 8.0f;

#endif		
		

		ImVec2 textSize = ImGui::CalcTextSize(diplayText);

		

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		const float wrap_pos_x = window->DC.TextWrapPos;
		
		DrawContectionLine(
			{ glm::vec2{ xPosRigthe, yPosTextLine }, Utils::ConvertImVecInGlm(textSize)}
			, { histoGramPos, histoGramScale }
			, lastEllementBottemPos.y
			, lastHeithShow
			, color
		);
		ImVec2 textCurserPos = { xPosRigthe + framePadding.x * 0.0f + framePadding.x * 3.0f, yPosText };
		ImGui::SetCursorScreenPos(textCurserPos);

		ImGui::Text(diplayText);
		textLineHeigth = textLineHeigth + ImGui::GetTextLineHeight();
		return textLineHeigth;
	}

	void ImGuiPerfomeceRendering::DrawContectionLine(const ElementTransform& textTransform, const ElementTransform& histrogramTransform, float baseButtemBarHigth, float histrogramBarHigth, const glm::vec4& color)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImVec2 region = ImGui::GetContentRegionAvail();
		const ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 framePadding = style.FramePadding;

		ImVec2 leftTop = ImVec2{ -1.0f,-1.0f };
		ImVec2 leftBottem = ImVec2{ -1.0f,-1.0f };

		ImVec2 rigthTop = ImVec2{ -1.0f,-1.0f };
		ImVec2 rigthBottem = ImVec2{-1.0f,-1.0f };

		float xPosLeft = histrogramTransform.postion.x + framePadding.x * 4.0f + framePadding.x * 0.0f + (histrogramTransform.scale.x);
		float xPosRigthe = xPosLeft + region.x / 3.0f;
		float offsetValueX = xPosRigthe < 20.0f ? xPosRigthe : 10.0f;

		leftTop.x = xPosLeft;
		leftTop.y = histrogramBarHigth;
		leftBottem.x = xPosLeft;
		leftBottem.y = baseButtemBarHigth;

		leftTop.x += offsetValueX;
		leftBottem.x += offsetValueX;

		rigthTop.x = textTransform.postion.x;
		rigthTop.y = textTransform.postion.y;
		rigthBottem = rigthTop;

		rigthTop.x -= offsetValueX;
		rigthBottem.x -= offsetValueX;

		rigthBottem.y += textTransform.scale.y;
		
#if 0
		ImU32 colorBit = IM_COL32(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f);

		ImVec2 leftTopCopy = leftTop;
		ImVec2 leftBottemCopy = leftBottem;
		ImVec2 rigthTopCopy = rigthTop;
		ImVec2  rigthBottemCopy = rigthBottem;


		ImVec2 leftOffsetTop = leftTop;
		ImVec2 leftOffsetBottem = leftBottem;
		draw_list->AddQuadFilled(leftTopCopy, rigthTopCopy, rigthBottemCopy, leftBottemCopy, colorBit);
		
		leftTop.x -= offsetValueX;
		leftBottem.x -= offsetValueX;
		draw_list->AddQuadFilled(leftTop, leftTopCopy, leftTopCopy, leftBottemCopy, colorBit);



		ImVec2 rigthOffsetTop = rigthTop;
		ImVec2 rigthOffsetBottem = rigthBottem;

		rigthTop.x += offsetValueX;
		rigthBottem.x += offsetValueX;		
		draw_list->AddQuadFilled(rigthTop, rigthTopCopy, rigthBottemCopy, rigthBottem, colorBit);

		for(int i = 0; i < 0; i++)
		{
			

			draw_list->PathLineTo(rigthTopCopy);
			draw_list->PathLineTo(rigthTop);
			draw_list->PathLineTo(rigthBottem);

			draw_list->PathLineTo(rigthBottemCopy);

			draw_list->PathLineTo(leftBottemCopy);
			draw_list->PathLineTo(leftBottem);
			draw_list->PathLineTo(leftTop);

			draw_list->PathLineTo(leftBottem);
			draw_list->PathLineTo(leftBottemCopy);
			draw_list->PathLineTo(leftTopCopy);

			if(i == 0)
			{
				draw_list->PathFillConvex(colorBit);
			}
			else
			{
				draw_list->PathStroke(IM_COL32(color.x * 192.0f, color.y * 128.0f, color.z * 128.0f, 128.0f), ImDrawFlags_Closed, 3.0f);
			
			}
		}
#else

		DrawConection(leftTop,leftBottem,rigthTop,rigthBottem, color, offsetValueX);
#endif
		rigthTop.x += offsetValueX;
		rigthTop.x += offsetValueX;

		ImGui::SetCursorScreenPos(rigthTop);
	}

	void ImGuiPerfomeceRendering::DrawConection(const ImVec2& pLT, const ImVec2& pLB, const ImVec2& pRT, const ImVec2& pRB, const glm::vec4& color, float offset)
	{
		
		ImVec2 leftTop[3] = {
			pLT, 
			{ pLT.x - offset, pLT.y },
			pRT
		};
		ImVec2 leftBottem[3] = {
			pLB,
			{ pLB.x - offset, pLB.y },
			pRB
		};

		ImVec2 rigthTop[3] = {
			pRT,
			pLT,
			{ pRT.x + offset, pRT.y },
			
		};
		
		ImVec2 rigthBottem[3] = {
			pRB,
			pLB,
			{ pRB.x + offset, pRB.y },
		};

		for (int i = 0; i < 3; i++)
		{
			DrawQuad(leftTop[i], leftBottem[i], rigthTop[i], rigthBottem[i], color);
		}
		
	}


	std::tuple<glm::vec2, glm::vec2, float> ImGuiPerfomeceRendering::PlotHistogramGraph(std::vector<float>& dataVec, uint32_t firstIndex, uint32_t endIndex, ImVec2 drawSpace, const glm::vec4& color)
	{
		constexpr double maxGraph = (1.0 / 30.0) * 1000.0;
		constexpr double minGraph = 0.0f;
		constexpr double absGraph = glm::abs(maxGraph - minGraph);
		

		float lastValue = dataVec.at(endIndex - 1);
#if 1
#if 0
		ImVec2 curserPosBefor = ImGui::GetCursorScreenPos();
#else
		glm::vec2 curserPosBefor = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());
#endif

		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Utils::ConvertGlmInImVec(color));
		uint32_t size = endIndex - firstIndex;
		int offset = 0;
		// size += size / 2;
		// size = dataVec.size();
		float* dataPtr = dataVec.data();
		dataPtr += firstIndex;
		ImGui::PlotHistogram("##Frame Histogram", dataPtr, size, offset, nullptr, minGraph, maxGraph, drawSpace);

		ImGui::PopStyleColor();

		const ImGuiStyle& style = ImGui::GetStyle();
#if 1
		glm::vec2 framePadding = Utils::ConvertImVecInGlm(style.FramePadding);
		glm::vec2 itemInnerSpacing = Utils::ConvertImVecInGlm(style.ItemInnerSpacing);

		glm::vec2 curserPosAfter = curserPosBefor + Utils::ConvertImVecInGlm(drawSpace);
		ImGui::SameLine();

		curserPosAfter -= framePadding;
		curserPosBefor += framePadding;


#if 0
		glm::vec2 histoGramScale = curserPosAfter - curserPosBefor;
#else
		glm::vec2 histoGramScale = curserPosAfter - curserPosBefor;

#endif

#elif 1
		glm::vec2 curserPosAfter = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());
		ImGui::SameLine();
		curserPosAfter.x = ImGui::GetCursorScreenPos().x;


		glm::vec2 framePadding = Utils::ConvertImVecInGlm(style.FramePadding);

		glm::vec2 histoGramPos = curserPosBefor - framePadding;
		glm::vec2 histoGramScale = curserPosAfter - curserPosBefor;
		histoGramScale -= 1.0f * framePadding;
#else
		ImVec2 curserPosAfter = ImGui::GetCursorScreenPos();
		ImGui::SameLine();
		curserPosAfter.x = ImGui::GetCursorScreenPos().x;


		glm::vec2 histoGramPos = {
			curserPosBefor.x - style.FramePadding.x, curserPosBefor.y - style.FramePadding.y
		};
		glm::vec2 histoGramScale = {
			(curserPosAfter.x - curserPosBefor.x) - (1.0f * style.FramePadding.x),
			(curserPosAfter.y - curserPosBefor.y) - (1.0f * style.FramePadding.y)
		};
#endif

#if 1
		double absUnitHigth = static_cast<double>(histoGramScale.y) / absGraph;
		double graphHigth = lastValue - minGraph;
		double inveredGraphHigth = maxGraph - graphHigth;

		double minLastHeithShow = absUnitHigth * graphHigth;
		float lastHeithShow = curserPosAfter.y - minLastHeithShow;
#else
		double graphHigth = lastValue - minGraph;
		double inveredGraphHigth = maxGraph - graphHigth;

		double absUnitHigth = static_cast<double>(histoGramScale.y) / absGraph;
		double minLastHeithShow = absUnitHigth * graphHigth;

		float lastHeithShow = curserPosAfter.y - minLastHeithShow;
#endif	 
		const float& lastElementTopPosY = lastHeithShow;
		const glm::vec2& lastEllementBottemPos = curserPosAfter;

		std::tuple<glm::vec2, glm::vec2, float> lastElementBottemRigthPosHistogrammScaleAndHeigth = {
			lastEllementBottemPos, histoGramScale, lastElementTopPosY
		};
#else
		
		std::tuple<glm::vec2, glm::vec2, float> lastElementBottemRigthPosHistogrammScaleAndHeigth = DrawHistogram(dataVec, drawSpace, color);

#endif
		

		return lastElementBottemRigthPosHistogrammScaleAndHeigth;
	}

	std::tuple<glm::vec2, glm::vec2, float> ImGuiPerfomeceRendering::DrawHistogram(std::vector<float>& dataVec, ImVec2 drawSpace, const glm::vec4& color)
	{
		constexpr double maxGraphY = (1.0 / 10.0) * 1000.0;
		constexpr double minGraphY = 0.0f;
		constexpr double absGraphY = glm::abs(maxGraphY - minGraphY);
		constexpr float absGraphYFloat = static_cast<float>(absGraphY);
		constexpr float rectangleRound = 0.0f;
		constexpr float thiknes = 0.0f;
		constexpr float offsetPosBetweenBarsX = 1.0f;

		const ImGuiStyle& style = ImGui::GetStyle();
		glm::vec2 framePadding = Utils::ConvertImVecInGlm(style.FramePadding);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		glm::vec2 curserPos = Utils::ConvertImVecInGlm(ImGui::GetCursorScreenPos());

		const glm::vec2 framePadingGroundMin = curserPos;
		const glm::vec2 framePadingGroundMax = framePadingGroundMin + Utils::ConvertImVecInGlm(drawSpace);
		const glm::vec2 framePadingGroundScale = framePadingGroundMax - framePadingGroundMin;

		draw_list->AddRectFilled(
			Utils::ConvertGlmInImVec(framePadingGroundMin)
			, Utils::ConvertGlmInImVec(framePadingGroundMax)
			, ImGui::GetColorU32(ImGuiCol_FrameBg)
			, style.FrameRounding
		);

		const glm::vec2 frameWorkingGroundMin = framePadingGroundMin + framePadding;
		const glm::vec2 frameWorkingGroundMax = framePadingGroundMax - framePadding;
		const glm::vec2 frameWorkingGroundScale = frameWorkingGroundMax - frameWorkingGroundMin;

		const float& posTopX = frameWorkingGroundMin.x;
		const float& posTopY = frameWorkingGroundMin.y;

		const float posBottemX = frameWorkingGroundMax.x;
		const float posBottemY = frameWorkingGroundMax.y;



		const uint32_t count = dataVec.size();
		const glm::vec2 units = glm::vec2{ static_cast<float>(count), absGraphYFloat };
		
		const float offsetPixelX = offsetPosBetweenBarsX * static_cast<float>(count);
		const float offsetPixelXPerElement = frameWorkingGroundScale.x <= offsetPixelX ? offsetPixelX : 0.0f;
		const float offsetPixelCount = offsetPixelXPerElement * static_cast<float>(count);
		
		const glm::vec2 workingPixelUnits = frameWorkingGroundScale;

		const glm::vec2 pixelPerUinte = (workingPixelUnits / units) - glm::vec2{ offsetPixelXPerElement, 0.0f };
		const float& pixelPerUinteX = pixelPerUinte.x;

		float elementBottem = -11.11111f;
		float elementTop = -22.22222f;
		const ImU32 colorBit[2] = {
			IM_COL32(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f),
			IM_COL32(color.x * 192.0f, color.y * 192.0f, color.z * 192.0f, color.w * 255.0f) 
		};
		
		float posX = posTopX;
	

		for (const float& value : dataVec)
		{
			if (value != value) // Ignore NaN values
			{
				posX += pixelPerUinteX;
			}
			else if (value <= minGraphY)
			{
				elementTop = posBottemY;
				elementBottem = posBottemY;
				posX += pixelPerUinteX;
			} 
			else if (value < maxGraphY)
			{
				float heigth = pixelPerUinte.y * value;
				float topPos = posBottemY - heigth;
				elementTop = topPos;
				glm::vec2 min = { posX, topPos };
				posX += pixelPerUinteX;
				
				elementBottem = posBottemY;
				glm::vec2 max = { posX, posBottemY };

				draw_list->AddRectFilled(
					Utils::ConvertGlmInImVec(min)
					, Utils::ConvertGlmInImVec(max)
					, colorBit[0], rectangleRound);
#if 0
				draw_list->AddRect(
					Utils::ConvertGlmInImVec(min)
					, Utils::ConvertGlmInImVec(max)
					, colorBit[1], rectangleRound, thiknes);
#endif

			} 
			else if (maxGraphY <= value)
			{
				elementTop = posTopY;
				glm::vec2 min = { posX, elementTop };
				posX += pixelPerUinteX;

				elementBottem = posBottemY;
				glm::vec2 max = { posX, posBottemY };

				draw_list->AddRectFilled(
					Utils::ConvertGlmInImVec(min)
					, Utils::ConvertGlmInImVec(max)
					, colorBit[0], rectangleRound);
#if 0
				draw_list->AddRect(
					Utils::ConvertGlmInImVec(min)
					, Utils::ConvertGlmInImVec(max)
					, colorBit[1], rectangleRound, thiknes);
#endif
			}
			posX += offsetPixelXPerElement;
		}

		posX += pixelPerUinteX;
		posX += offsetPixelXPerElement;
		posX += pixelPerUinteX;

		posX += framePadding.x;
		glm::vec2 histogramBarScale = Utils::ConvertImVecInGlm(drawSpace);
		glm::vec2 lastElementPos = { posX , elementBottem };
		
		float lastElementTop = elementTop;

		ImGui::SetCursorScreenPos(Utils::ConvertGlmInImVec(lastElementPos));

		return std::tuple<glm::vec2, glm::vec2, float>{
			lastElementPos, frameWorkingGroundScale, lastElementTop
		};
	}

	void ImGuiPerfomeceRendering::DrawQuad(const ImVec2& pLT, const ImVec2& pLB, const ImVec2& pRT, const ImVec2& pRB, const glm::vec4& color)
	{
		constexpr float offset = 0.5f;
		constexpr float invOffset = -offset;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 colorBit = IM_COL32(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f);
#if 1
#if	1

		draw_list->AddQuadFilled(
			ImVec2{ pRB.x - offset		, pRB.y - offset }
			, ImVec2{ pRT.x - offset	, pRT.y - invOffset }
			, ImVec2{ pLT.x - invOffset	, pLT.y - invOffset }
			, ImVec2{ pLB.x - invOffset	, pLB.y - offset }
			, colorBit
		);

#elif 1
		colorBit = IM_COL32(color.x * 255.0f, color.y * 32.0f, color.z * 32.0f, color.w * 128.0f);
		ImU32 colorBitLine = IM_COL32(color.x * 192.0f, color.y * 16.0f, color.z * 32.0f, color.w * 255.0f);

		draw_list->AddQuadFilled(
			   ImVec2{ pRB.x - offset		, pRB.y - offset }
			 , ImVec2{ pRT.x - offset		, pRT.y - invOffset }
			 , ImVec2{ pLT.x - invOffset	, pLT.y - invOffset }
			 , ImVec2{ pLB.x - invOffset	, pLB.y - offset }
			 , colorBit
		);
		// draw_list->AddQuad(
		// 	ImVec2{ pRB.x - offset		, pRB.y - offset }
		// 	, ImVec2{ pRT.x - offset	, pRT.y - invOffset }
		// 	, ImVec2{ pLT.x - invOffset	, pLT.y - invOffset }
		// 	, ImVec2{ pLB.x - invOffset	, pLB.y - offset }
		// 	, colorBitLine
		// );
#else
		draw_list->AddQuad(
				ImVec2{ pRB.x - offset		, pRB.y - offset }
			, ImVec2{ pRT.x - offset	, pRT.y - invOffset }
			, ImVec2{ pLT.x - invOffset	, pLT.y - invOffset }
			, ImVec2{ pLB.x - invOffset	, pLB.y - offset }
			, colorBit
		);
#endif

		// draw_list->AddQuad(pRB, pRT, pLT, pLB, IM_COL32(color.x * 192.0f, color.y * 128.0f, color.z * 128.0f, color.w * 255.0f), 3.0f);
#else
		draw_list->PathLineTo(pRB);
		draw_list->PathLineTo(pRT);
		draw_list->PathLineTo(pLT);
		draw_list->PathLineTo(pLB);
		draw_list->PathFillConvex(colorBit);
#endif

	}


	

}