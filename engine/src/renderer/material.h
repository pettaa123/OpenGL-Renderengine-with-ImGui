#pragma once

#include "renderer/shader.h"
#include "renderer/texture.h"

#include <unordered_set>

#include "renderer/shader.h"

namespace Engine {

	enum class MaterialFlag
	{
		None       = BIT(0),
		DepthTest  = BIT(1),
		Blend      = BIT(2),
		TwoSided   = BIT(3)
	};

	class Material : public RefCounted
	{
		friend class Material;
	public:
		static std::shared_ptr<Material> create(const std::shared_ptr<Shader>& shader, const std::string& name = "");
		static std::shared_ptr<Material> copy(const std::shared_ptr<Material>& other, const std::string& name = "");
		virtual ~Material() {}

		virtual void invalidate() = 0;
		
		virtual void set(const std::string& name, float value) = 0;
		virtual void set(const std::string& name, int value) = 0;
		virtual void set(const std::string& name, uint32_t value) = 0;
		virtual void set(const std::string& name, bool value) = 0;
		virtual void set(const std::string& name, const glm::vec2& value) = 0;
		virtual void set(const std::string& name, const glm::vec3& value) = 0;
		virtual void set(const std::string& name, const glm::vec4& value) = 0;
		virtual void set(const std::string& name, const glm::ivec2& value) = 0;
		virtual void set(const std::string& name, const glm::ivec3& value) = 0;
		virtual void set(const std::string& name, const glm::ivec4& value) = 0;

		virtual void set(const std::string& name, const glm::mat3& value) = 0;
		virtual void set(const std::string& name, const glm::mat4& value) = 0;

		virtual void set(const std::string& name, const std::shared_ptr<Texture2D>& texture) = 0;
		virtual void set(const std::string& name, const std::shared_ptr<Texture2D>& texture, uint32_t arrayIndex) = 0;
		virtual void set(const std::string& name, const std::shared_ptr<TextureCube>& texture) = 0;
		virtual void set(const std::string& name, const std::shared_ptr<Image2D>& image) = 0;

		virtual float& getFloat(const std::string& name) = 0;
		virtual int32_t& getInt(const std::string& name) = 0;
		virtual uint32_t& getUInt(const std::string& name) = 0;
		virtual bool& getBool(const std::string& name) = 0;
		virtual glm::vec2& getVector2(const std::string& name) = 0;
		virtual glm::vec3& getVector3(const std::string& name) = 0;
		virtual glm::vec4& getVector4(const std::string& name) = 0;
		virtual glm::mat3& getMatrix3(const std::string& name) = 0;
		virtual glm::mat4& getMatrix4(const std::string& name) = 0;

		virtual std::shared_ptr<Texture2D> GetTexture2D(const std::string& name) = 0;
		virtual std::shared_ptr<TextureCube> GetTextureCube(const std::string& name) = 0;

		virtual std::shared_ptr<Texture2D> TryGetTexture2D(const std::string& name) = 0;
		virtual std::shared_ptr<TextureCube> TryGetTextureCube(const std::string& name) = 0;

		virtual uint32_t getFlags() const = 0;
		virtual bool getFlag(MaterialFlag flag) const = 0;
		virtual void setFlag(MaterialFlag flag, bool value = true) = 0;

		virtual std::shared_ptr<Shader> getShader() = 0;
		virtual const std::string& getName() const = 0;
	};

}
