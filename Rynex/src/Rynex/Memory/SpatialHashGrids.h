#pragma once
#include<rypch.h>

namespace Rynex {

	namespace Memory {
		struct Client 
		{
			glm::vec3 postion;
			glm::vec3 dimension;
			size_t indices;
		};
		class SpatialHashGrids 
		{
		public:
			using ValueType = int;
			using Key = size_t;
			using Container = robin_hood::unordered_flat_map<Key, ValueType>;
		public:
			SpatialHashGrids()
			{

			}

		private:
			void Insert(const Client& client)
			{
				auto [x, y, z] = client.postion;
				std::pair<float , float> ()
			}



		private:
			Container m_Cells;

		};
	}

}