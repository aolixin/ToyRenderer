#pragma once
#include <cstdint>
#include <fstream>
#include <vector>

#include "geometry.h"

namespace TGA
{
#pragma pack(push,1)
	struct TGAHeader
	{
		std::uint8_t idlength = 0;
		std::uint8_t colormaptype = 0;
		std::uint8_t datatypecode = 0;
		std::uint16_t colormaporigin = 0;
		std::uint16_t colormaplength = 0;
		std::uint8_t colormapdepth = 0;
		std::uint16_t x_origin = 0;
		std::uint16_t y_origin = 0;
		std::uint16_t width = 0;
		std::uint16_t height = 0;
		std::uint8_t bitsperpixel = 0;
		std::uint8_t imagedescriptor = 0;
	};
#pragma pack(pop)

	struct TGAColor
	{
		std::uint8_t bgra[4] = {0, 0, 0, 0};
		std::uint8_t bytespp = 4;
		std::uint8_t& operator[](const int i) { return bgra[i]; }
	};

	struct TGAImage
	{
		enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

		inline TGAImage() = default;
		inline TGAImage(const int w, const int h, const int bpp);
		inline bool read_tga_file(const std::string filename);
		inline bool write_tga_file(const std::string filename, const bool vflip = true, const bool rle = true) const;
		inline void flip_horizontally();
		inline void flip_vertically();
		inline std::uint32_t get(const int x, const int y) const;
		inline void set(const int x, const int y, const std::uint32_t&);
		inline int width() const;
		inline int height() const;

		inline std::uint32_t sample2D(const Vec2f& uvf) const;

		inline std::uint32_t SampleBilinear(float x, float y) const;
		inline std::uint32_t BilinearInterp(uint32_t, uint32_t, uint32_t, uint32_t, int32_t, int32_t) const;

		inline std::uint32_t SampleBicubic(float x, float y) const;
		inline std::uint32_t BicubicInterp(float, float) const;
		inline static std::uint32_t ApplyWeight(std::uint32_t color, float weight);
		inline static float CubicWeight(float t);

	private:
		inline bool load_rle_data(std::ifstream& in);
		inline bool unload_rle_data(std::ofstream& out) const;

		std::vector<Vec4f> bicubic_pixels = std::vector<Vec4f>(16);

		int w = 0;
		int h = 0;
		std::uint8_t bpp = 0;
		std::vector<std::uint8_t> data = {};
	};


	TGAImage::TGAImage(const int w, const int h, const int bpp) : w(w), h(h), bpp(bpp), data(w * h * bpp, 0)
	{
	}

	bool TGAImage::read_tga_file(const std::string filename)
	{
		std::ifstream in;
		in.open(filename, std::ios::binary);
		if (!in.is_open())
		{
			std::cerr << "can't open file " << filename << "\n";
			return false;
		}
		TGAHeader header;
		in.read(reinterpret_cast<char*>(&header), sizeof(header));
		if (!in.good())
		{
			std::cerr << "an error occured while reading the header\n";
			return false;
		}
		w = header.width;
		h = header.height;
		bpp = header.bitsperpixel >> 3;
		if (w <= 0 || h <= 0 || (bpp != GRAYSCALE && bpp != RGB && bpp != RGBA))
		{
			std::cerr << "bad bpp (or width/height) value\n";
			return false;
		}
		size_t nbytes = bpp * w * h;
		data = std::vector<std::uint8_t>(nbytes, 0);
		if (3 == header.datatypecode || 2 == header.datatypecode)
		{
			in.read(reinterpret_cast<char*>(data.data()), nbytes);
			if (!in.good())
			{
				std::cerr << "an error occured while reading the data\n";
				return false;
			}
		}
		else if (10 == header.datatypecode || 11 == header.datatypecode)
		{
			if (!load_rle_data(in))
			{
				std::cerr << "an error occured while reading the data\n";
				return false;
			}
		}
		else
		{
			std::cerr << "unknown file format " << (int)header.datatypecode << "\n";
			return false;
		}
		if (!(header.imagedescriptor & 0x20))
			flip_vertically();
		if (header.imagedescriptor & 0x10)
			flip_horizontally();
		std::cerr << w << "x" << h << "/" << bpp * 8 << "\n";
		return true;
	}

	bool TGAImage::load_rle_data(std::ifstream& in)
	{
		size_t pixelcount = w * h;
		size_t currentpixel = 0;
		size_t currentbyte = 0;
		TGAColor colorbuffer;
		do
		{
			std::uint8_t chunkheader = 0;
			chunkheader = in.get();
			if (!in.good())
			{
				std::cerr << "an error occured while reading the data\n";
				return false;
			}
			if (chunkheader < 128)
			{
				chunkheader++;
				for (int i = 0; i < chunkheader; i++)
				{
					in.read(reinterpret_cast<char*>(colorbuffer.bgra), bpp);
					if (!in.good())
					{
						std::cerr << "an error occured while reading the header\n";
						return false;
					}
					for (int t = 0; t < bpp; t++)
						data[currentbyte++] = colorbuffer.bgra[t];
					currentpixel++;
					if (currentpixel > pixelcount)
					{
						std::cerr << "Too many pixels read\n";
						return false;
					}
				}
			}
			else
			{
				chunkheader -= 127;
				in.read(reinterpret_cast<char*>(colorbuffer.bgra), bpp);
				if (!in.good())
				{
					std::cerr << "an error occured while reading the header\n";
					return false;
				}
				for (int i = 0; i < chunkheader; i++)
				{
					for (int t = 0; t < bpp; t++)
						data[currentbyte++] = colorbuffer.bgra[t];
					currentpixel++;
					if (currentpixel > pixelcount)
					{
						std::cerr << "Too many pixels read\n";
						return false;
					}
				}
			}
		}
		while (currentpixel < pixelcount);
		return true;
	}

	bool TGAImage::write_tga_file(const std::string filename, const bool vflip, const bool rle) const
	{
		constexpr std::uint8_t developer_area_ref[4] = {0, 0, 0, 0};
		constexpr std::uint8_t extension_area_ref[4] = {0, 0, 0, 0};
		constexpr std::uint8_t footer[18] = {
			'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O', 'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'
		};
		std::ofstream out;
		out.open(filename, std::ios::binary);
		if (!out.is_open())
		{
			std::cerr << "can't open file " << filename << "\n";
			return false;
		}
		TGAHeader header = {};
		header.bitsperpixel = bpp << 3;
		header.width = w;
		header.height = h;
		header.datatypecode = (bpp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
		header.imagedescriptor = vflip ? 0x00 : 0x20; // top-left or bottom-left origin
		out.write(reinterpret_cast<const char*>(&header), sizeof(header));
		if (!out.good())
		{
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		if (!rle)
		{
			out.write(reinterpret_cast<const char*>(data.data()), w * h * bpp);
			if (!out.good())
			{
				std::cerr << "can't unload raw data\n";
				return false;
			}
		}
		else if (!unload_rle_data(out))
		{
			std::cerr << "can't unload rle data\n";
			return false;
		}
		out.write(reinterpret_cast<const char*>(developer_area_ref), sizeof(developer_area_ref));
		if (!out.good())
		{
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write(reinterpret_cast<const char*>(extension_area_ref), sizeof(extension_area_ref));
		if (!out.good())
		{
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write(reinterpret_cast<const char*>(footer), sizeof(footer));
		if (!out.good())
		{
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		return true;
	}

	// TODO: it is not necessary to break a raw chunk for two equal pixels (for the matter of the resulting size)
	bool TGAImage::unload_rle_data(std::ofstream& out) const
	{
		const std::uint8_t max_chunk_length = 128;
		size_t npixels = w * h;
		size_t curpix = 0;
		while (curpix < npixels)
		{
			size_t chunkstart = curpix * bpp;
			size_t curbyte = curpix * bpp;
			std::uint8_t run_length = 1;
			bool raw = true;
			while (curpix + run_length < npixels && run_length < max_chunk_length)
			{
				bool succ_eq = true;
				for (int t = 0; succ_eq && t < bpp; t++)
					succ_eq = (data[curbyte + t] == data[curbyte + t + bpp]);
				curbyte += bpp;
				if (1 == run_length)
					raw = !succ_eq;
				if (raw && succ_eq)
				{
					run_length--;
					break;
				}
				if (!raw && !succ_eq)
					break;
				run_length++;
			}
			curpix += run_length;
			out.put(raw ? run_length - 1 : run_length + 127);
			if (!out.good())
			{
				std::cerr << "can't dump the tga file\n";
				return false;
			}
			out.write(reinterpret_cast<const char*>(data.data() + chunkstart), (raw ? run_length * bpp : bpp));
			if (!out.good())
			{
				std::cerr << "can't dump the tga file\n";
				return false;
			}
		}
		return true;
	}

	std::uint32_t TGAImage::get(const int x, const int y) const
	{
		if (!data.size() || x < 0 || y < 0 || x >= w || y >= h)
			return {};
		const std::uint8_t* p = data.data() + (x + y * w) * bpp;
		return p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
	}


	void TGAImage::set(int x, int y, const std::uint32_t& c)
	{
		if (!data.size() || x < 0 || y < 0 || x >= w || y >= h) return;
		std::uint8_t* p = data.data() + (x + y * w) * RGBA;
		p[0] = c & 0xff;
		p[1] = (c >> 8) & 0xff;
		p[2] = (c >> 16) & 0xff;
		p[3] = (c >> 24) & 0xff;
	}

	void TGAImage::flip_horizontally()
	{
		int half = w >> 1;
		for (int i = 0; i < half; i++)
			for (int j = 0; j < h; j++)
				for (int b = 0; b < bpp; b++)
					std::swap(data[(i + j * w) * bpp + b], data[(w - 1 - i + j * w) * bpp + b]);
	}

	void TGAImage::flip_vertically()
	{
		int half = h >> 1;
		for (int i = 0; i < w; i++)
			for (int j = 0; j < half; j++)
				for (int b = 0; b < bpp; b++)
					std::swap(data[(i + j * w) * bpp + b], data[(i + (h - 1 - j) * w) * bpp + b]);
	}

	int TGAImage::width() const
	{
		return w;
	}

	int TGAImage::height() const
	{
		return h;
	}

	std::uint32_t TGAImage::sample2D(const Vec2f& uvf) const
	{
		return get(uvf.x * w, uvf.y * h);
		//return SampleBilinear(uvf.x * w, uvf.y * h);
		return SampleBicubic(uvf.x * w, uvf.y * h);
	}


	// 双线性插值
	std::uint32_t TGAImage::SampleBilinear(float x, float y) const
	{
		int32_t fx = (int32_t)(x * 0x10000);
		int32_t fy = (int32_t)(y * 0x10000);

		int32_t x1 = Between(0, width() - 1, (fx >> 16) - 1);
		int32_t y1 = Between(0, height() - 1, (fy >> 16) - 1);
		int32_t x2 = Between(0, width() - 1, fx >> 16);
		int32_t y2 = Between(0, height() - 1, fy >> 16);

		int32_t dx = (fx >> 8) & 0xff;
		int32_t dy = (fy >> 8) & 0xff;
		if (width() <= 0 || height() <= 0) return 0;
		uint32_t c00 = get(x1, y1);
		uint32_t c01 = get(x2, y1);
		uint32_t c10 = get(x1, y2);
		uint32_t c11 = get(x2, y2);
		return BilinearInterp(c00, c01, c10, c11, dx, dy);
	}


	// 双线性插值计算：给出四个点的颜色，以及坐标偏移，计算结果
	std::uint32_t TGAImage::BilinearInterp(uint32_t tl, uint32_t tr, uint32_t bl, uint32_t br, int32_t distx,
	                                       int32_t disty) const
	{
		uint32_t f, r;
		int32_t distxy = distx * disty;
		int32_t distxiy = (distx << 8) - distxy; /* distx * (256 - disty) */
		int32_t distixy = (disty << 8) - distxy; /* disty * (256 - distx) */
		int32_t distixiy = 256 * 256 - (disty << 8) - (distx << 8) + distxy;
		r = (tl & 0x000000ff) * distixiy + (tr & 0x000000ff) * distxiy
			+ (bl & 0x000000ff) * distixy + (br & 0x000000ff) * distxy;
		f = (tl & 0x0000ff00) * distixiy + (tr & 0x0000ff00) * distxiy
			+ (bl & 0x0000ff00) * distixy + (br & 0x0000ff00) * distxy;
		r |= f & 0xff000000;
		tl >>= 16;
		tr >>= 16;
		bl >>= 16;
		br >>= 16;
		r >>= 16;
		f = (tl & 0x000000ff) * distixiy + (tr & 0x000000ff) * distxiy
			+ (bl & 0x000000ff) * distixy + (br & 0x000000ff) * distxy;
		r |= f & 0x00ff0000;
		f = (tl & 0x0000ff00) * distixiy + (tr & 0x0000ff00) * distxiy
			+ (bl & 0x0000ff00) * distixy + (br & 0x0000ff00) * distxy;
		r |= f & 0xff000000;
		return r;
	}


	std::uint32_t TGAImage::SampleBicubic(float x, float y) const
	{
		int32_t x_int = static_cast<int32_t>(x);
		int32_t y_int = static_cast<int32_t>(y);

		auto& pixels = const_cast<std::vector<Vec4f>&>(bicubic_pixels);

		for (int32_t j = 0; j < 4; ++j)
		{
			for (int32_t i = 0; i < 4; ++i)
			{
				std::int32_t cx = Between(0, width() - 1, x_int - 1 + i);
				std::int32_t cy = Between(0, height() - 1, y_int - 1 + j);
				pixels[j * 4 + i] = vector_from_color(get(cx, cy));
			}
		}
		float weight_x = x - x_int;
		float weight_y = y - y_int;

		//return pixels[0];
		uint32_t res = BicubicInterp(weight_x, weight_y);

		return res;
	}

	std::uint32_t TGAImage::BicubicInterp(float weight_x, float weight_y) const
	{
		Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};
		for (int32_t j = 0; j < 4; ++j)
		{
			for (int32_t i = 0; i < 4; ++i)
			{
				float weight = CubicWeight(weight_x - i + 1.0f) * CubicWeight(weight_y - j + 1.0f);
				color += bicubic_pixels[j * 4 + i] * weight;
			}
		}

		return vector_to_color(color);
	}

	// 应用权重并转换颜色
	std::uint32_t TGAImage::ApplyWeight(std::uint32_t color, float weight)
	{
		uint32_t res = 0;
		res |= (uint32_t)(((color & 0x000000ff) * weight)) & 0x000000ff;
		res |= (uint32_t)(((color & 0x0000ff00) * weight)) & 0x0000ff00;
		res |= (uint32_t)(((color & 0x00ff0000) * weight)) & 0x00ff0000;
		res |= (uint32_t)(((color & 0xff000000) * weight)) & 0xff000000;

		return res;
	}

	// 三次插值权重函数
	float TGAImage::CubicWeight(float x)
	{
		/*float abs_x = abs(x);
		float a = -0.5;
		if (abs_x <= 1.0)
		{
			return (a + 2) * pow(abs_x, 3) - (a + 3) * pow(abs_x, 2) + 1;
		}
		else if (abs_x < 2.0)
		{
			return a * pow(abs_x, 3) - 5 * a * pow(abs_x, 2) + 8 * a * abs_x - 4 * a;
		}
		else
			return 0.0;*/

		float abs_x = abs(x);
		if (abs_x < 1.0)
		{
			return 1 - 2 * abs_x * abs_x + abs_x * abs_x * abs_x;
		}
		else if (abs_x < 2.0)
		{
			return 4 - 8 * abs_x + 5 * abs_x * abs_x - abs_x * abs_x * abs_x;
		}
		else
			return 0.0;
	}
}
