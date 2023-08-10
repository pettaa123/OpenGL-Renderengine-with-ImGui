#include "renderer/renderer2D.h"

#include "renderer/vertexArray.h"
#include "renderer/shader.h"
#include "renderer/uniformBuffer.h"
#include "renderer/renderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include "MSDFData.h"

#include <array>

namespace Engine {

	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float tilingFactor;
		
		// Editor-only
		int entityID;
	};

	struct CircleVertex
	{
		glm::vec3 worldPosition;
		glm::vec3 localPosition;
		glm::vec4 color;
		float thickness;
		float fade;

		// Editor-only
		int entityID;
	};

	struct LineVertex
	{
		glm::vec3 position;
		glm::vec4 color;

		// Editor-only
		int entityID;
	};

	struct TextVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;

		// TODO: bg color for outline/bg

		// Editor-only
		int entityID;
	};

	struct Renderer2DData
	{
		static const uint32_t maxQuads = 20000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32; // TODO: RenderCaps

		std::shared_ptr<VertexArray> quadVertexArray;
		std::shared_ptr<VertexBuffer> quadVertexBuffer;
		std::shared_ptr<Shader> quadShader;
		std::shared_ptr<Texture2D> whiteTexture;

		std::shared_ptr<VertexArray> circleVertexArray;
		std::shared_ptr<VertexBuffer> circleVertexBuffer;
		std::shared_ptr<Shader> circleShader;

		std::shared_ptr<VertexArray> lineVertexArray;
		std::shared_ptr<VertexBuffer> lineVertexBuffer;
		std::shared_ptr<Shader> lineShader;	
		
		std::shared_ptr<VertexArray> textVertexArray;
		std::shared_ptr<VertexBuffer> textVertexBuffer;
		std::shared_ptr<Shader> textShader;

		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		uint32_t lineVertexCount = 0;
		LineVertex* lineVertexBufferBase = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;

		uint32_t textIndexCount = 0;
		TextVertex* textVertexBufferBase = nullptr;
		TextVertex* textVertexBufferPtr = nullptr;

		float lineWidth = 2.0f;

		std::array<std::shared_ptr<Texture2D>,maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1; // 0 = white texture
		
		std::shared_ptr<Texture2D> fontAtlasTexture;

		glm::vec4 quadVertexPositions[4];

		Renderer2D::Statistics stats;

		struct CameraData
		{
			glm::mat4 viewProjection;
		};
		CameraData cameraBuffer;
		std::shared_ptr<UniformBuffer> cameraUniformBuffer;
	};

	static Renderer2DData s_data;

	void Renderer2D::init()
	{
		s_data.quadVertexArray = VertexArray::create();

		s_data.quadVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(QuadVertex));
		s_data.quadVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
		});
		s_data.quadVertexArray->addVertexBuffer(s_data.quadVertexBuffer);

		s_data.quadVertexBufferBase = new QuadVertex[s_data.maxVertices];

		uint32_t* quadIndices = new uint32_t[s_data.maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.maxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::create(quadIndices, s_data.maxIndices);
		s_data.quadVertexArray->setIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circles
		s_data.circleVertexArray = VertexArray::create();

		s_data.circleVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(CircleVertex));
		s_data.circleVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"         },
			{ ShaderDataType::Float,  "a_Thickness"     },
			{ ShaderDataType::Float,  "a_Fade"          },
			{ ShaderDataType::Int,    "a_EntityID"      }
		});
		s_data.circleVertexArray->addVertexBuffer(s_data.circleVertexBuffer);
		s_data.circleVertexArray->setIndexBuffer(quadIB); // Use quad IB
		s_data.circleVertexBufferBase = new CircleVertex[s_data.maxVertices];

		// Lines
		s_data.lineVertexArray = VertexArray::create();

		s_data.lineVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(LineVertex));
		s_data.lineVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    },
			{ ShaderDataType::Int,    "a_EntityID" }
		});
		s_data.lineVertexArray->addVertexBuffer(s_data.lineVertexBuffer);
		s_data.lineVertexBufferBase = new LineVertex[s_data.maxVertices];

		// Text
		s_data.textVertexArray = VertexArray::create();

		s_data.textVertexBuffer = VertexBuffer::create(s_data.maxVertices * sizeof(TextVertex));
		s_data.textVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Int,    "a_EntityID"     }
		});
		s_data.textVertexArray->addVertexBuffer(s_data.textVertexBuffer);
		s_data.textVertexArray->setIndexBuffer(quadIB);
		s_data.textVertexBufferBase = new TextVertex[s_data.maxVertices];

		s_data.whiteTexture = Texture2D::create(TextureSpecification());
		uint32_t whiteTextureData = 0xffffffff;
		s_data.whiteTexture->setData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_data.maxTextureSlots];
		for (uint32_t i = 0; i < s_data.maxTextureSlots; i++)
			samplers[i] = i;

		s_data.quadShader = Shader::create("assets/shaders/Renderer2D_Quad.glsl");
		s_data.circleShader = Shader::create("assets/shaders/Renderer2D_Circle.glsl");
		s_data.lineShader = Shader::create("assets/shaders/Renderer2D_Line.glsl");
		s_data.textShader = Shader::create("assets/shaders/Renderer2D_Text.glsl");

		// Set first texture slot to 0
		s_data.textureSlots[0] = s_data.whiteTexture;

		s_data.quadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.quadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_data.cameraUniformBuffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::shutdown()
	{
		delete[] s_data.quadVertexBufferBase;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		s_data.cameraBuffer.viewProjection = camera.getViewProjectionMatrix();
		s_data.cameraUniformBuffer->setData(&s_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		startBatch();
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_data.cameraBuffer.viewProjection = camera.getProjection() * glm::inverse(transform);
		s_data.cameraUniformBuffer->setData(&s_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		startBatch();
	}

	void Renderer2D::beginScene(const EditorCamera& camera)
	{

		s_data.cameraBuffer.viewProjection = camera.getViewProjection();
		s_data.cameraUniformBuffer->setData(&s_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		startBatch();
	}

	void Renderer2D::endScene()
	{
		flush();
	}

	void Renderer2D::startBatch()
	{
		s_data.quadIndexCount = 0;
		s_data.quadVertexBufferPtr = s_data.quadVertexBufferBase;

		s_data.circleIndexCount = 0;
		s_data.circleVertexBufferPtr = s_data.circleVertexBufferBase;

		s_data.lineVertexCount = 0;
		s_data.lineVertexBufferPtr = s_data.lineVertexBufferBase;	
		
		s_data.textIndexCount = 0;
		s_data.textVertexBufferPtr = s_data.textVertexBufferBase;

		s_data.textureSlotIndex = 1;
	}

	void Renderer2D::flush()
	{
		if (s_data.quadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.quadVertexBufferPtr - (uint8_t*)s_data.quadVertexBufferBase);
			s_data.quadVertexBuffer->setData(s_data.quadVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_data.textureSlotIndex; i++)
				s_data.textureSlots[i]->bind(i);

			s_data.quadShader->bind();
			RenderCommand::drawIndexed(s_data.quadVertexArray, s_data.quadIndexCount);
			s_data.stats.drawCalls++;
		}

		if (s_data.circleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.circleVertexBufferPtr - (uint8_t*)s_data.circleVertexBufferBase);
			s_data.circleVertexBuffer->setData(s_data.circleVertexBufferBase, dataSize);

			s_data.circleShader->bind();
			RenderCommand::drawIndexed(s_data.circleVertexArray, s_data.circleIndexCount);
			s_data.stats.drawCalls++;
		}

		if (s_data.lineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.lineVertexBufferPtr - (uint8_t*)s_data.lineVertexBufferBase);
			s_data.lineVertexBuffer->setData(s_data.lineVertexBufferBase, dataSize);

			s_data.lineShader->bind();
			RenderCommand::setLineWidth(s_data.lineWidth);
			RenderCommand::drawLines(s_data.lineVertexArray, s_data.lineVertexCount);
			s_data.stats.drawCalls++;
		}
		
		if (s_data.textIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_data.textVertexBufferPtr - (uint8_t*)s_data.textVertexBufferBase);
			s_data.textVertexBuffer->setData(s_data.textVertexBufferBase, dataSize);

			auto buf = s_data.textVertexBufferBase;
			//s_data.FontAtlasTexture->bind(0);

			s_data.textShader->bind();
			RenderCommand::drawIndexed(s_data.textVertexArray, s_data.textIndexCount);
			s_data.stats.drawCalls++;
		}
	}

	void Renderer2D::nextBatch()
	{
		flush();
		startBatch();
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		drawQuad(transform, color);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		drawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
			nextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color = color;
			s_data.quadVertexBufferPtr->texCoord = textureCoords[i];
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityID = entityID;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		s_data.stats.quadCount++;
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_data.quadIndexCount >= Renderer2DData::maxIndices)
			nextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_data.textureSlotIndex; i++)
		{
			if (*s_data.textureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_data.textureSlotIndex >= Renderer2DData::maxTextureSlots)
				nextBatch();

			textureIndex = (float)s_data.textureSlotIndex;
			s_data.textureSlots[s_data.textureSlotIndex] = texture;
			s_data.textureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_data.quadVertexBufferPtr->position = transform * s_data.quadVertexPositions[i];
			s_data.quadVertexBufferPtr->color = tintColor;
			s_data.quadVertexBufferPtr->texCoord = textureCoords[i];
			s_data.quadVertexBufferPtr->texIndex = textureIndex;
			s_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			s_data.quadVertexBufferPtr->entityID = entityID;
			s_data.quadVertexBufferPtr++;
		}

		s_data.quadIndexCount += 6;

		s_data.stats.quadCount++;
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		drawQuad(transform, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		drawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness /*= 1.0f*/, float fade /*= 0.005f*/, int entityID /*= -1*/)
	{

		// TODO: implement for circles
		// if (s_data.quadIndexCount >= Renderer2DData::MaxIndices)
		// 	NextBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_data.circleVertexBufferPtr->worldPosition = transform * s_data.quadVertexPositions[i];
			s_data.circleVertexBufferPtr->localPosition = s_data.quadVertexPositions[i] * 2.0f;
			s_data.circleVertexBufferPtr->color = color;
			s_data.circleVertexBufferPtr->thickness = thickness;
			s_data.circleVertexBufferPtr->fade = fade;
			s_data.circleVertexBufferPtr->entityID = entityID;
			s_data.circleVertexBufferPtr++;
		}

		s_data.circleIndexCount += 6;

		s_data.stats.quadCount++;
	}

	void Renderer2D::drawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color, int entityID)
	{
		s_data.lineVertexBufferPtr->position = p0;
		s_data.lineVertexBufferPtr->color = color;
		s_data.lineVertexBufferPtr->entityID = entityID;
		s_data.lineVertexBufferPtr++;

		s_data.lineVertexBufferPtr->position = p1;
		s_data.lineVertexBufferPtr->color = color;
		s_data.lineVertexBufferPtr->entityID = entityID;
		s_data.lineVertexBufferPtr++;

		s_data.lineVertexCount += 2;
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		drawLine(p0, p1, color, entityID);
		drawLine(p1, p2, color, entityID);
		drawLine(p2, p3, color, entityID);
		drawLine(p3, p0, color, entityID);
	}

	void Renderer2D::drawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * s_data.quadVertexPositions[i];

		drawLine(lineVertices[0], lineVertices[1], color, entityID);
		drawLine(lineVertices[1], lineVertices[2], color, entityID);
		drawLine(lineVertices[2], lineVertices[3], color, entityID);
		drawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	//void Renderer2D::drawSprite(const glm::mat4& transform, spriteRendererComponent& src, int entityID)
	//{
	//	if (src.texture)
	//		drawQuad(transform, src.texture, src.TilingFactor, src.color, entityID);
	//	else
	//		drawQuad(transform, src.color, entityID);
	//}

	//void Renderer2D::drawString(const std::string& string, std::shared_ptr<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID)
	//{
	//	const auto& fontGeometry = font->GetMSDFData()->fontGeometry;
	//	const auto& metrics = fontGeometry.getMetrics();
	//	std::shared_ptr<Texture2D> fontAtlas = font->GetAtlasTexture();
	//
	//	s_data.fontAtlasTexture = fontAtlas;
	//
	//	double x = 0.0;
	//	double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
	//	double y = 0.0;
	//
	//	const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();
	//	
	//	for (size_t i = 0; i < string.size(); i++)
	//	{
	//		char character = string[i];
	//		if (character == '\r')
	//			continue;
	//
	//		if (character == '\n')
	//		{
	//			x = 0;
	//			y -= fsScale * metrics.lineHeight + textParams.lineSpacing;
	//			continue;
	//		}
	//
	//		if (character == ' ')
	//		{
	//			float advance = spaceGlyphAdvance;
	//			if (i < string.size() - 1)
	//			{
	//				char nextCharacter = string[i + 1];
	//				double dAdvance;
	//				fontGeometry.getAdvance(dAdvance, character, nextCharacter);
	//				advance = (float)dAdvance;
	//			}
	//
	//			x += fsScale * advance + textParams.Kerning;
	//			continue;
	//		}
	//
	//		if (character == '\t')
	//		{
	//			// NOTE(Yan): is this right?
	//			x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
	//			continue;
	//		}
	//
	//		auto glyph = fontGeometry.getGlyph(character);
	//		if (!glyph)
	//			glyph = fontGeometry.getGlyph('?');
	//		if (!glyph)
	//			return;
	//
	//		double al, ab, ar, at;
	//		glyph->getQuadAtlasBounds(al, ab, ar, at);
	//		glm::vec2 texCoordMin((float)al, (float)ab);
	//		glm::vec2 texCoordMax((float)ar, (float)at);
	//
	//		double pl, pb, pr, pt;
	//		glyph->getQuadPlaneBounds(pl, pb, pr, pt);
	//		glm::vec2 quadMin((float)pl, (float)pb);
	//		glm::vec2 quadMax((float)pr, (float)pt);
	//
	//		quadMin *= fsScale, quadMax *= fsScale;
	//		quadMin += glm::vec2(x, y);
	//		quadMax += glm::vec2(x, y);
	//
	//		float texelWidth = 1.0f / fontAtlas->getWidth();
	//		float texelHeight = 1.0f / fontAtlas->getHeight();
	//		texCoordMin *= glm::vec2(texelWidth, texelHeight);
	//		texCoordMax *= glm::vec2(texelWidth, texelHeight);
	//
	//		// render here
	//		s_data.textVertexBufferPtr->position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
	//		s_data.textVertexBufferPtr->color = textParams.color;
	//		s_data.textVertexBufferPtr->texCoord = texCoordMin;
	//		s_data.textVertexBufferPtr->entityID = entityID;
	//		s_data.textVertexBufferPtr++;
	//
	//		s_data.textVertexBufferPtr->position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
	//		s_data.textVertexBufferPtr->color = textParams.color;
	//		s_data.textVertexBufferPtr->texCoord = { texCoordMin.x, texCoordMax.y };
	//		s_data.textVertexBufferPtr->entityID = entityID;
	//		s_data.textVertexBufferPtr++;
	//
	//		s_data.textVertexBufferPtr->position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
	//		s_data.textVertexBufferPtr->color = textParams.color;
	//		s_data.textVertexBufferPtr->texCoord = texCoordMax;
	//		s_data.textVertexBufferPtr->entityID = entityID;
	//		s_data.textVertexBufferPtr++;
	//
	//		s_data.textVertexBufferPtr->position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
	//		s_data.textVertexBufferPtr->color = textParams.color;
	//		s_data.textVertexBufferPtr->texCoord = { texCoordMax.x, texCoordMin.y };
	//		s_data.textVertexBufferPtr->entityID = entityID;
	//		s_data.textVertexBufferPtr++;
	//
	//		s_data.textIndexCount += 6;
	//		s_data.stats.quadCount++;
	//
	//		if (i < string.size() - 1)
	//		{
	//			double advance = glyph->getAdvance();
	//			char nextCharacter = string[i + 1];
	//			fontGeometry.getAdvance(advance, character, nextCharacter);
	//
	//			x += fsScale * advance + textParams.Kerning;
	//		}
	//	}
	//}

	//void Renderer2D::drawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID)
	//{
	//	drawString(string, component.FontAsset, transform, { component.color, component.Kerning, component.lineSpacing }, entityID);
	//}

	float Renderer2D::getLineWidth()
	{
		return s_data.lineWidth;
	}

	void Renderer2D::setLineWidth(float width)
	{
		s_data.lineWidth = width;
	}

	void Renderer2D::resetStats()
	{
		memset(&s_data.stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::getStats()
	{
		return s_data.stats;
	}

}
