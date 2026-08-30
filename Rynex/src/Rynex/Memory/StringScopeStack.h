#pragma once
#include <Rynex/Asset/Base/Asset.h>

namespace Rynex{
	namespace Memory {

		class StringScopeStack
		{
		public:
			StringScopeStack();
			~StringScopeStack();

			const std::string& GetScope() const;

			void PushScope(const std::string& name);
			void PopScope();
			void PushScope(const std::string& name, AssetHandle handle);
			void PushScope(const std::string& name, uint64_t value);
			void PushScope(const std::string& name, uint32_t value);
			void PushScope(const std::string& name, const std::string& valueName);

			void PrintScope()const;

			std::string&& GetScopeToName(const std::string& name) const;
			std::string&& GetScopeNamespaces()const;
			std::string&& GetScopePath()const;
			void Clear();
		private:
			std::string m_Scope;
			std::vector<uint32_t> m_CountVec;
		};
	}
}
