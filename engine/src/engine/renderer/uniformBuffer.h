#pragma once

#include <cstdint>
#include <memory>

namespace Engine {

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() {};
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		
		static std::shared_ptr<UniformBuffer> create(uint32_t size, uint32_t binding);
	};

}
