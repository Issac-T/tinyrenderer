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
	//ͨ��RGB����
	TGAColor(unsigned char R, unsigned char G, unsigned char B) :b(B), g(G), r(R), a(0), bytespp(3) {}
	//ͨ��RGBA����
	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) :b(B), g(G), r(R), a(A), bytespp(4) {}
	//ͨ����������ֵval����
	TGAColor(int v, int bpp) :val(v), bytespp(bpp) {}
	//�������캯��
	TGAColor(const TGAColor& c) :val(c.val), bytespp(c.bytespp) {}
	//ͨ���ֽ��鹹�� ע��val(0)�����ֵ
	TGAColor(const unsigned char* p, int bpp) :val(0), bytespp(bpp)
	{
		for (int i = 0; i < bpp; i++)
		{
			raw[i] = p[i];
		}
	}
	//����=�����(��������& ��ʵ������=��ֵ����)
	TGAColor& operator=(const TGAColor& c)
	{
		if (this != &c)//�����Ը�ֵ
		{
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}
};

//TGAImage��
class TGAImage
{
protected:
	unsigned char* data;
	int width;
	int height;
	int bytespp; //ÿ�����ص��ֽ���

	bool load_rle_data(std::ifstream &in); //����RLEѹ������
	bool unload_rle_data(std::ofstream& out); //����RLEѹ������

public:
	//ͼ���ʽ(bytespp��Ӧ)
	enum  Format
	{
		GRAYSCALE = 1, //�Ҷ�ͼ
		RGB = 3,	//RGBͼ
		RGBA = 4	//RGBAͼ
	};

	//ͼ���ļ���ʽ
	enum  FileType
	{
		UNCOMPRESSED = 1, //δѹ��color-map
		UNCOMPRESSED_TRUECOLOR = 2, //δѹ�����ɫ
		UNCOMPRESSED_GRAYSCLAE = 3, //δѹ���ڰ׻Ҷ�ͼ
		RLE = 9, //RLEѹ��color-map
		RLE_TRUECOLOR = 10, //RLEѹ�����ɫ
		RLE_GRAYSCLAE = 11 //RLEѹ���ڰ׻Ҷ�ͼ
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage& img);//��������
	bool read_tga_file(const char* filename); //���ļ���ȡtgaͼ��
	bool write_tga_file(const char* filename,bool rle=true); //TGAд�����ļ�
	bool flip_horizontally(); //ˮƽ��ת
	bool flip_vertically(); //��ֱ��ת
	bool scale(int w, int h); //����
	TGAColor get(int x, int y); //��ȡ����ֵ
	bool set(int x, int y, TGAColor c); //��������ֵ
	~TGAImage();
	TGAImage& operator=(const TGAImage& img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char* buffer();
	void clear();
};

#endif //__TGA_IMAGE_H__
