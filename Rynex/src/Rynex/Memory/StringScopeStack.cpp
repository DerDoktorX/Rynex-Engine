#include "rypch.h"
#include "StringScopeStack.h"

namespace Rynex {
	namespace Memory {

		StringScopeStack::StringScopeStack()
			: m_Scope()
			, m_CountVec()
		{
		}

		StringScopeStack::~StringScopeStack()
		{
		}

		const std::string& StringScopeStack::GetScope() const
		{
			return m_Scope;
		}

		void StringScopeStack::PushScope(const std::string& name)
		{
			uint32_t scopeCharEnd = static_cast<uint32_t>(m_Scope.size());

			if (0u != scopeCharEnd)
				m_Scope += ".";
			m_Scope += name;

			m_CountVec.emplace_back(scopeCharEnd);
		}

		void StringScopeStack::PopScope()
		{
			RY_CORE_ASSERT(!m_CountVec.empty(), "No scopse to pop");

			uint32_t scopeCharEnd = m_CountVec.back();
			m_CountVec.pop_back();
			std::string::const_iterator itPos = m_Scope.begin() + scopeCharEnd;
			std::string::const_iterator itEnd = m_Scope.end();

			m_Scope.erase(itPos, itEnd);
		}

		void StringScopeStack::PushScope(const std::string& name, AssetHandle handle)
		{
			uint64_t value = handle;
			PushScope(name, value);
		}

		void StringScopeStack::PushScope(const std::string& name, uint32_t value)
		{
			uint64_t value64 = static_cast<uint64_t>(value);
			PushScope(name, value64);
		}

		void StringScopeStack::PushScope(const std::string& name, const std::string& valueName)
		{
			std::string strValueName = name + '[' + valueName + ']';
			PushScope(strValueName);
		}


		void StringScopeStack::PushScope(const std::string& name, uint64_t value)
		{
			std::string strValue = std::to_string(value);
			PushScope(name, strValue);
		}

		void StringScopeStack::PrintScope() const
		{
			RY_CORE_TRACE("DrawContext Scope: {}", m_Scope);
		}

		std::string&& StringScopeStack::GetScopeToName(const std::string& name) const
		{
			if (m_Scope.empty())
			{
				RY_CORE_WARN("Scope string is empty!");
				return std::string();
			}

			size_t pos = m_Scope.rfind(name);
			const char* posFirst = m_Scope.c_str();
			const char* posEnd = posFirst + pos;
			return std::string(posFirst, posEnd);
		}

		std::string&& StringScopeStack::GetScopeNamespaces() const
		{
			std::string scopeNamespaces;
			scopeNamespaces.reserve(m_Scope.size());
			int8_t openBrakets = 0;
			for (const char& c : m_Scope)
			{

				switch (c)
				{
				case '[':
					openBrakets++;
					break;
				case ']':
					openBrakets--;
					break;

				default:
					if (openBrakets == 0)
						scopeNamespaces += c;
					break;
				}
				RY_CORE_ASSERT(0 <= openBrakets, "negativ openbarkets shoud never happen!");
			}
			return std::move(scopeNamespaces);
		}

		std::string&& StringScopeStack::GetScopePath() const
		{
			return std::string(m_Scope);
		}

		void StringScopeStack::Clear()
		{
			m_CountVec.clear();
			m_Scope.clear();
		}
	}
}