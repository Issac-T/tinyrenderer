#ifndef __TGA_IMAGE_H__
#define __TGA_IMAGE_H__

#include <fstream>

#pragma pack(push, 1)
struct TGA_Header
{
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char bitsperpixel;
	char imagedescriptor;
};
#pragma pack(pop)

struct TGAColor
{
	union
	{
		struct
		{
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	TGAColor() :val(0), bytespp(1) {}
	//通过RGB构造
	TGAColor(unsigned char R, unsigned char G, unsigned char B) :b(B), g(G), r(R), a(0), bytespp(3) {}
	//通过RGBA构造
	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) :b(B), g(G), r(R), a(A), bytespp(4) {}
	//通过整体像素值val构造
	TGAColor(int v, int bpp) :val(v), bytespp(bpp) {}
	//拷贝构造函数
	TGAColor(const TGAColor& c) :val(c.val), bytespp(c.bytespp) {}
	//通过字节组构造 注意val(0)先清空值
	TGAColor(const unsigned char* p, int bpp) :val(0), bytespp(bpp)
	{
		for (int i = 0; i < bpp; i++)
		{
			raw[i] = p[i];
		}
	}
	//重载=运算符(返回引用& 可实现连续=赋值运算)
	TGAColor& operator=(const TGAColor& c)
	{
		if (this != &c)//避免自赋值
		{
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}
};

//TGAImage类
class TGAImage
{
protected:
	unsigned char* data;
	int width;
	int height;
	int bytespp; //每个像素的字节数

	bool load_rle_data(std::ifstream &in); //加载RLE压缩数据
	bool unload_rle_data(std::ofstream& out); //保存RLE压缩数据

public:
	//图像格式(bytespp对应)
	enum  Format
	{
		GRAYSCALE = 1, //灰度图
		RGB = 3,	//RGB图
		RGBA = 4	//RGBA图
	};

	//图像文件格式
	enum  FileType
	{
		UNCOMPRESSED = 1, //未压缩color-map
		UNCOMPRESSED_TRUECOLOR = 2, //未压缩真彩色
		UNCOMPRESSED_GRAYSCLAE = 3, //未压缩黑白灰度图
		RLE = 9, //RLE压缩color-map
		RLE_TRUECOLOR = 10, //RLE压缩真彩色
		RLE_GRAYSCLAE = 11 //RLE压缩黑白灰度图
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage& img);//拷贝构造
	bool read_tga_file(const char* filename); //从文件读取tga图像
	bool write_tga_file(const char* filename,bool rle=true); //TGA写入至文件
	bool flip_horizontally(); //水平翻转
	bool flip_vertically(); //垂直翻转
	bool scale(int w, int h); //缩放
	TGAColor get(int x, int y); //获取像素值
	bool set(int x, int y, TGAColor c); //设置像素值
	~TGAImage();
	TGAImage& operator=(const TGAImage& img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char* buffer();
	void clear();
};

#endif //__TGA_IMAGE_H__
