#pragma once

#include <glm/vec4.hpp>
#include <string>
#include <cstddef>

namespace TextureMapping {

	/// <summary>
	/// Represents a color with 4 floating-point components (R, G, B, A).
	/// </summary>
	struct Color4 {

	public:

		float r;
		float g;
		float b;
		float a;

		/// <summary>
		/// Initializes a new instance of the <see cref="Color4"/> struct.
		/// </summary>
		/// <param name="r">The red component of the new Color4 structure.</param>
		/// <param name="g">The green component of the new Color4 structure.</param>
		/// <param name="b">The blue component of the new Color4 structure.</param>
		/// <param name="a">The alpha component of the new Color4 structure.</param>
		constexpr Color4() :
			r(1.0f),
			g(1.0f),
			b(1.0f),
			a(1.0f)
		{}

		/// <summary>
		/// Initializes a new instance of the <see cref="Color4"/> struct.
		/// </summary>
		/// <param name="r">The red component of the new Color4 structure.</param>
		/// <param name="g">The green component of the new Color4 structure.</param>
		/// <param name="b">The blue component of the new Color4 structure.</param>
		/// <param name="a">The alpha component of the new Color4 structure.</param>
		constexpr Color4(std::byte r, std::byte g, std::byte b, std::byte a) :
			r((float)r / 255.0f),
			g((float)g / 255.0f),
			b((float)b / 255.0f),
			a((float)a / 255.0f)
		{}

		/// <summary>
		/// Initializes a new instance of the <see cref="Color4"/> struct.
		/// </summary>
		/// <param name="r">The red component of the new Color4 structure.</param>
		/// <param name="g">The green component of the new Color4 structure.</param>
		/// <param name="b">The blue component of the new Color4 structure.</param>
		/// <param name="a">The alpha component of the new Color4 structure.</param>
		constexpr Color4(float r, float g, float b, float a = 1.0f) :
			r(r),
			g(g),
			b(b),
			a(a)
		{}

		bool equals(Color4 other)
		{
			return
				r == other.r &&
				g == other.g &&
				b == other.b &&
				a == other.a;
		}

		/// <summary>
		/// Compares the specified Color4 structures for equality.
		/// </summary>
		/// <param name="left">The left-hand side of the comparison.</param>
		/// <param name="right">The right-hand side of the comparison.</param>
		/// <returns>True if left is equal to right; false otherwise.</returns>
		bool operator==(const Color4& right)
		{
			return equals(right);
		}

		glm::vec4 vec4()
		{
			return glm::vec4{ r,g,b,a };
		}

		/// <summary>
		/// Compares the specified Color4 structures for inequality.
		/// </summary>
		/// <param name="left">The left-hand side of the comparison.</param>
		/// <param name="right">The right-hand side of the comparison.</param>
		/// <returns>True if left is not equal to right; false otherwise.</returns>
		//[Pure]
		//static constexpr bool operator !=(Color4 left, Color4 right)
		//{
		//	return !left.Equals(right);
		//}

		/// <inheritdoc cref="ToString(string, IFormatProvider)"/>
		std::string toString()
		{
			std::string var = "R: " + std::to_string(r) + "G: " + std::to_string(g) + "B: " + std::to_string(b) + "A: " + std::to_string(a);
			return var;
		}



		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 255, 255, 0).
		/// </summary>
		static constexpr Color4 transparent() { return Color4(255, 255, 255, 0); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (240, 248, 255, 255).
		/// </summary>
		static constexpr Color4 aliceBlue() { return Color4(240, 248, 255, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (250, 235, 215, 255).
		/// </summary>
		static constexpr Color4 antiqueWhite() { return Color4(250, 235, 215, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 255, 255, 255).
		/// </summary>
		static constexpr Color4 aqua() { return Color4(0, 255, 255, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (127, 255, 212, 255).
		/// </summary>
		static constexpr Color4 Aquamarine() { return Color4(127, 255, 212, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (240, 255, 255, 255).
		/// </summary>
		static constexpr Color4 Azure() { return Color4(240, 255, 255, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (245, 245, 220, 255).
		/// </summary>
		static constexpr Color4 Beige() { return Color4(245, 245, 220, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 228, 196, 255).
		/// </summary>
		static constexpr Color4 Bisque() { return Color4(255, 228, 196, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 0, 0, 255).
		/// </summary>
		static constexpr Color4 Black() { return Color4(0, 0, 0, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 235, 205, 255).
		/// </summary>
		static constexpr Color4 BlanchedAlmond() { return Color4(255, 235, 205, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 0, 255, 255).
		/// </summary>
		static constexpr Color4 Blue() { return Color4(0, 0, 255, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (138, 43, 226, 255).
		/// </summary>
		static constexpr Color4 BlueViolet() { return Color4(138, 43, 226, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (165, 42, 42, 255).
		/// </summary>
		static constexpr Color4 Brown() { return Color4(165, 42, 42, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (222, 184, 135, 255).
		/// </summary>
		static constexpr Color4 BurlyWood() { return Color4(222, 184, 135, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (95, 158, 160, 255).
		/// </summary>
		static constexpr Color4 CadetBlue() { return Color4(95, 158, 160, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (127, 255, 0, 255).
		/// </summary>
		static constexpr Color4 Chartreuse() { return Color4(127, 255, 0, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (210, 105, 30, 255).
		/// </summary>
		static constexpr Color4 Chocolate() { return Color4(210, 105, 30, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 127, 80, 255).
		/// </summary>
		static constexpr Color4 Coral() { return Color4(255, 127, 80, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (100, 149, 237, 255).
		/// </summary>
		static constexpr Color4 CornflowerBlue() { return Color4(100, 149, 237, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 248, 220, 255).
		/// </summary>
		static constexpr Color4 Cornsilk() { return Color4(255, 248, 220, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (220, 20, 60, 255).
		/// </summary>
		static constexpr Color4 Crimson() { return Color4(220, 20, 60, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 255, 255, 255).
		/// </summary>
		static constexpr Color4 Cyan() { return Color4(0, 255, 255, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 0, 139, 255).
		/// </summary>
		static constexpr Color4 DarkBlue() { return Color4(0, 0, 139, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 139, 139, 255).
		/// </summary>
		static constexpr Color4 DarkCyan() { return Color4(0, 139, 139, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (184, 134, 11, 255).
		/// </summary>
		static constexpr Color4 DarkGoldenrod() { return Color4(184, 134, 11, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (169, 169, 169, 255).
		/// </summary>
		static constexpr Color4 DarkGray() { return Color4(169, 169, 169, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 100, 0, 255).
		/// </summary>
		static constexpr Color4 DarkGreen() { return Color4(0, 100, 0, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (189, 183, 107, 255).
		/// </summary>
		static constexpr Color4 DarkKhaki() { return Color4(189, 183, 107, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (139, 0, 139, 255).
		/// </summary>
		static constexpr Color4 DarkMagenta() { return Color4(139, 0, 139, 255); }

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (85, 107, 47, 255).
		/// </summary>
		static constexpr Color4 DarkOliveGreen() {
			return Color4(85, 107, 47, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 140, 0, 255).
		/// </summary>
		static constexpr Color4 DarkOrange() {
			return Color4(255, 140, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (153, 50, 204, 255).
		/// </summary>
		static constexpr Color4 DarkOrchid() {
			return Color4(153, 50, 204, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (139, 0, 0, 255).
		/// </summary>
		static constexpr Color4 DarkRed() {
			return Color4(139, 0, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (233, 150, 122, 255).
		/// </summary>
		static constexpr Color4 DarkSalmon() {
			return Color4(233, 150, 122, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (143, 188, 139, 255).
		/// </summary>
		static constexpr Color4 DarkSeaGreen() {
			return Color4(143, 188, 139, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (72, 61, 139, 255).
		/// </summary>
		static constexpr Color4 DarkSlateBlue() {
			return Color4(72, 61, 139, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (47, 79, 79, 255).
		/// </summary>
		static constexpr Color4 DarkSlateGray() {
			return Color4(47, 79, 79, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 206, 209, 255).
		/// </summary>
		static constexpr Color4 DarkTurquoise() {
			return Color4(0, 206, 209, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (148, 0, 211, 255).
		/// </summary>
		static constexpr Color4 DarkViolet() {
			return Color4(148, 0, 211, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 20, 147, 255).
		/// </summary>
		static constexpr Color4 DeepPink() {
			return Color4(255, 20, 147, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 191, 255, 255).
		/// </summary>
		static constexpr Color4 DeepSkyBlue() {
			return Color4(0, 191, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (105, 105, 105, 255).
		/// </summary>
		static constexpr Color4 DimGray() {
			return Color4(105, 105, 105, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (30, 144, 255, 255).
		/// </summary>
		static constexpr Color4 DodgerBlue() {
			return Color4(30, 144, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (178, 34, 34, 255).
		/// </summary>
		static constexpr Color4 Firebrick() {
			return Color4(178, 34, 34, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 250, 240, 255).
		/// </summary>
		static constexpr Color4 FloralWhite() {
			return Color4(255, 250, 240, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (34, 139, 34, 255).
		/// </summary>
		static constexpr Color4 ForestGreen() {
			return Color4(34, 139, 34, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 0, 255, 255).
		/// </summary>
		static constexpr Color4 Fuchsia() {
			return Color4(255, 0, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (220, 220, 220, 255).
		/// </summary>
		static constexpr Color4 Gainsboro() {
			return Color4(220, 220, 220, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (248, 248, 255, 255).
		/// </summary>
		static constexpr Color4 GhostWhite() {
			return Color4(248, 248, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 215, 0, 255).
		/// </summary>
		static constexpr Color4 Gold() {
			return Color4(255, 215, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (218, 165, 32, 255).
		/// </summary>
		static constexpr Color4 Goldenrod() {
			return Color4(218, 165, 32, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (128, 128, 128, 255).
		/// </summary>
		static constexpr Color4 Gray() {
			return Color4(128, 128, 128, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 128, 0, 255).
		/// </summary>
		static constexpr Color4 Green() {
			return Color4(0, 128, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (173, 255, 47, 255).
		/// </summary>
		static constexpr Color4 GreenYellow() {
			return Color4(173, 255, 47, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (240, 255, 240, 255).
		/// </summary>
		static constexpr Color4 Honeydew() {
			return Color4(240, 255, 240, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 105, 180, 255).
		/// </summary>
		static constexpr Color4 HotPink() {
			return Color4(255, 105, 180, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (205, 92, 92, 255).
		/// </summary>
		static constexpr Color4 IndianRed() {
			return Color4(205, 92, 92, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (75, 0, 130, 255).
		/// </summary>
		static constexpr Color4 Indigo() {
			return Color4(75, 0, 130, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 255, 240, 255).
		/// </summary>
		static constexpr Color4 Ivory() {
			return Color4(255, 255, 240, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (240, 230, 140, 255).
		/// </summary>
		static constexpr Color4 Khaki() {
			return Color4(240, 230, 140, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (230, 230, 250, 255).
		/// </summary>
		static constexpr Color4 Lavender() {
			return Color4(230, 230, 250, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 240, 245, 255).
		/// </summary>
		static constexpr Color4 LavenderBlush() {
			return Color4(255, 240, 245, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (124, 252, 0, 255).
		/// </summary>
		static constexpr Color4 LawnGreen() {
			return Color4(124, 252, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 250, 205, 255).
		/// </summary>
		static constexpr Color4 LemonChiffon() {
			return Color4(255, 250, 205, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (173, 216, 230, 255).
		/// </summary>
		static constexpr Color4 LightBlue() {
			return Color4(173, 216, 230, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (240, 128, 128, 255).
		/// </summary>
		static constexpr Color4 LightCoral() {
			return Color4(240, 128, 128, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (224, 255, 255, 255).
		/// </summary>
		static constexpr Color4 LightCyan() {
			return Color4(224, 255, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (250, 250, 210, 255).
		/// </summary>
		static constexpr Color4 LightGoldenrodYellow() {
			return Color4(250, 250, 210, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (144, 238, 144, 255).
		/// </summary>
		static constexpr Color4 LightGreen() {
			return Color4(144, 238, 144, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (211, 211, 211, 255).
		/// </summary>
		static constexpr Color4 LightGray() {
			return Color4(211, 211, 211, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 182, 193, 255).
		/// </summary>
		static constexpr Color4 LightPink() {
			return Color4(255, 182, 193, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 160, 122, 255).
		/// </summary>
		static constexpr Color4 LightSalmon() {
			return Color4(255, 160, 122, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (32, 178, 170, 255).
		/// </summary>
		static constexpr Color4 LightSeaGreen() {
			return Color4(32, 178, 170, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (135, 206, 250, 255).
		/// </summary>
		static constexpr Color4 LightSkyBlue() {
			return Color4(135, 206, 250, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (119, 136, 153, 255).
		/// </summary>
		static constexpr Color4 LightSlateGray() {
			return Color4(119, 136, 153, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (176, 196, 222, 255).
		/// </summary>
		static constexpr Color4 LightSteelBlue() {
			return Color4(176, 196, 222, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 255, 224, 255).
		/// </summary>
		static constexpr Color4 LightYellow() {
			return Color4(255, 255, 224, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 255, 0, 255).
		/// </summary>
		static constexpr Color4 Lime() {
			return Color4(0, 255, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (50, 205, 50, 255).
		/// </summary>
		static constexpr Color4 LimeGreen() {
			return Color4(50, 205, 50, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (250, 240, 230, 255).
		/// </summary>
		static constexpr Color4 Linen() {
			return Color4(250, 240, 230, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 0, 255, 255).
		/// </summary>
		static constexpr Color4 Magenta() {
			return Color4(255, 0, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (128, 0, 0, 255).
		/// </summary>
		static constexpr Color4 Maroon() {
			return Color4(128, 0, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (102, 205, 170, 255).
		/// </summary>
		static constexpr Color4 MediumAquamarine() {
			return Color4(102, 205, 170, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 0, 205, 255).
		/// </summary>
		static constexpr Color4 MediumBlue() {
			return Color4(0, 0, 205, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (186, 85, 211, 255).
		/// </summary>
		static constexpr Color4 MediumOrchid() {
			return Color4(186, 85, 211, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (147, 112, 219, 255).
		/// </summary>
		static constexpr Color4 MediumPurple() {
			return Color4(147, 112, 219, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (60, 179, 113, 255).
		/// </summary>
		static constexpr Color4 MediumSeaGreen() {
			return Color4(60, 179, 113, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (123, 104, 238, 255).
		/// </summary>
		static constexpr Color4 MediumSlateBlue() {
			return Color4(123, 104, 238, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 250, 154, 255).
		/// </summary>
		static constexpr Color4 MediumSpringGreen() {
			return Color4(0, 250, 154, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (72, 209, 204, 255).
		/// </summary>
		static constexpr Color4 MediumTurquoise() {
			return Color4(72, 209, 204, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (199, 21, 133, 255).
		/// </summary>
		static constexpr Color4 MediumVioletRed() {
			return Color4(199, 21, 133, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (25, 25, 112, 255).
		/// </summary>
		static constexpr Color4 MidnightBlue() {
			return Color4(25, 25, 112, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (245, 255, 250, 255).
		/// </summary>
		static constexpr Color4 MintCream() {
			return Color4(245, 255, 250, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 228, 225, 255).
		/// </summary>
		static constexpr Color4 MistyRose() {
			return Color4(255, 228, 225, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 228, 181, 255).
		/// </summary>
		static constexpr Color4 Moccasin() {
			return Color4(255, 228, 181, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 222, 173, 255).
		/// </summary>
		static constexpr Color4 NavajoWhite() {
			return Color4(255, 222, 173, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 0, 128, 255).
		/// </summary>
		static constexpr Color4 Navy() {
			return Color4(0, 0, 128, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (253, 245, 230, 255).
		/// </summary>
		static constexpr Color4 OldLace() {
			return Color4(253, 245, 230, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (128, 128, 0, 255).
		/// </summary>
		static constexpr Color4 Olive() {
			return Color4(128, 128, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (107, 142, 35, 255).
		/// </summary>
		static constexpr Color4 OliveDrab() {
			return Color4(107, 142, 35, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 165, 0, 255).
		/// </summary>
		static constexpr Color4 Orange() {
			return Color4(255, 165, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 69, 0, 255).
		/// </summary>
		static constexpr Color4 OrangeRed() {
			return Color4(255, 69, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (218, 112, 214, 255).
		/// </summary>
		static constexpr Color4 Orchid() {
			return Color4(218, 112, 214, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (238, 232, 170, 255).
		/// </summary>
		static constexpr Color4 PaleGoldenrod() {
			return Color4(238, 232, 170, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (152, 251, 152, 255).
		/// </summary>
		static constexpr Color4 PaleGreen() {
			return Color4(152, 251, 152, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (175, 238, 238, 255).
		/// </summary>
		static constexpr Color4 PaleTurquoise() {
			return Color4(175, 238, 238, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (219, 112, 147, 255).
		/// </summary>
		static constexpr Color4 PaleVioletRed() {
			return Color4(219, 112, 147, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 239, 213, 255).
		/// </summary>
		static constexpr Color4 PapayaWhip() {
			return Color4(255, 239, 213, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 218, 185, 255).
		/// </summary>
		static constexpr Color4 PeachPuff() {
			return Color4(255, 218, 185, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (205, 133, 63, 255).
		/// </summary>
		static constexpr Color4 Peru() {
			return Color4(205, 133, 63, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 192, 203, 255).
		/// </summary>
		static constexpr Color4 Pink() {
			return Color4(255, 192, 203, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (221, 160, 221, 255).
		/// </summary>
		static constexpr Color4 Plum() {
			return Color4(221, 160, 221, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (176, 224, 230, 255).
		/// </summary>
		static constexpr Color4 PowderBlue() {
			return Color4(176, 224, 230, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (128, 0, 128, 255).
		/// </summary>
		static constexpr Color4 Purple() {
			return Color4(128, 0, 128, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 0, 0, 255).
		/// </summary>
		static constexpr Color4 Red() {
			return Color4(255, 0, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (188, 143, 143, 255).
		/// </summary>
		static constexpr Color4 RosyBrown() {
			return Color4(188, 143, 143, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (65, 105, 225, 255).
		/// </summary>
		static constexpr Color4 RoyalBlue() {
			return Color4(65, 105, 225, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (139, 69, 19, 255).
		/// </summary>
		static constexpr Color4 SaddleBrown() {
			return Color4(139, 69, 19, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (250, 128, 114, 255).
		/// </summary>
		static constexpr Color4 Salmon() {
			return Color4(250, 128, 114, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (244, 164, 96, 255).
		/// </summary>
		static constexpr Color4 SandyBrown() {
			return Color4(244, 164, 96, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (46, 139, 87, 255).
		/// </summary>
		static constexpr Color4 SeaGreen() {
			return Color4(46, 139, 87, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 245, 238, 255).
		/// </summary>
		static constexpr Color4 SeaShell() {
			return Color4(255, 245, 238, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (160, 82, 45, 255).
		/// </summary>
		static constexpr Color4 Sienna() {
			return Color4(160, 82, 45, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (192, 192, 192, 255).
		/// </summary>
		static constexpr Color4 Silver() {
			return Color4(192, 192, 192, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (135, 206, 235, 255).
		/// </summary>
		static constexpr Color4 SkyBlue() {
			return Color4(135, 206, 235, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (106, 90, 205, 255).
		/// </summary>
		static constexpr Color4 SlateBlue() {
			return Color4(106, 90, 205, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (112, 128, 144, 255).
		/// </summary>
		static constexpr Color4 SlateGray() {
			return Color4(112, 128, 144, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 250, 250, 255).
		/// </summary>
		static constexpr Color4 Snow() {
			return Color4(255, 250, 250, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 255, 127, 255).
		/// </summary>
		static constexpr Color4 SpringGreen() {
			return Color4(0, 255, 127, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (70, 130, 180, 255).
		/// </summary>
		static constexpr Color4 SteelBlue() {
			return Color4(70, 130, 180, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (210, 180, 140, 255).
		/// </summary>
		static constexpr Color4 Tan() {
			return Color4(210, 180, 140, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (0, 128, 128, 255).
		/// </summary>
		static constexpr Color4 Teal() {
			return Color4(0, 128, 128, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (216, 191, 216, 255).
		/// </summary>
		static constexpr Color4 Thistle() {
			return Color4(216, 191, 216, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 99, 71, 255).
		/// </summary>
		static constexpr Color4 Tomato() {
			return Color4(255, 99, 71, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (64, 224, 208, 255).
		/// </summary>
		static constexpr Color4 Turquoise() {
			return Color4(64, 224, 208, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (238, 130, 238, 255).
		/// </summary>
		static constexpr Color4 Violet() {
			return Color4(238, 130, 238, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (245, 222, 179, 255).
		/// </summary>
		static constexpr Color4 Wheat() {
			return Color4(245, 222, 179, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 255, 255, 255).
		/// </summary>
		static constexpr Color4 White() {
			return Color4(255, 255, 255, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (245, 245, 245, 255).
		/// </summary>
		static constexpr Color4 WhiteSmoke() {
			return Color4(245, 245, 245, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (255, 255, 0, 255).
		/// </summary>
		static constexpr Color4 Yellow() {
			return Color4(255, 255, 0, 255);
		}

		/// <summary>
		/// Gets the system color with (R, G, B, A) = (154, 205, 50, 255).
		/// </summary>
		static constexpr Color4 YellowGreen() {
			return Color4(154, 205, 50, 255);
		}
	};
}