#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <math.h>
#include "tgaimage.h"

//RLE(游程编码)数据读取
bool TGAImage::load_rle_data(std::ifstream& in)
{
	unsigned long pixelcount = width * height;
	unsigned long currentpixel = 0;
	unsigned long currentbyte = 0;
	TGAColor colorbuffer;
	do {
		//读块头
		unsigned char chunkheader = 0;
		chunkheader = in.get();
		if (!in.good()) 
		{
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
		
		if (chunkheader < 128) //后续为chunkheader+1个非RLE数据
		{
			chunkheader++;
			for (int i = 0; i < chunkheader; i++) 
			{
				in.read((char*)colorbuffer.raw, bytespp);
				if (!in.good()) 
				{
					std::cerr << "an error occured while reading the header\n";
					return false;
				}
				for (int t = 0; t < bytespp; t++)
					data[currentbyte++] = colorbuffer.raw[t];
				currentpixel++;
				if (currentpixel > pixelcount) 
				{
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
		else //后续为chunkheader-127个RLE数据
		{
			chunkheader -= 127;
			in.read((char*)colorbuffer.raw, bytespp);
			if (!in.good()) 
			{
				std::cerr << "an error occured while reading the header\n";
				return false;
			}
			for (int i = 0; i < chunkheader; i++) 
			{
				for (int t = 0; t < bytespp; t++)
					data[currentbyte++] = colorbuffer.raw[t];
				currentpixel++;
				if (currentpixel > pixelcount) 
				{
					std::cerr << "Too many pixels read\n";
					return false;
				}
			}
		}
	} while (currentpixel < pixelcount);
	return true;
}

bool TGAImage::unload_rle_data(std::ofstream& out)
{
	const unsigned char max_chunk_length = 128; //最大块长度
	unsigned long npixels = width * height;
	unsigned long curpix = 0;
	while (curpix < npixels) 
	{
		unsigned long chunkstart = curpix * bytespp;
		unsigned long curbyte = curpix * bytespp;
		unsigned char run_length = 1;//当前已处理块长度(连续相等或连续不等)
		bool raw = true;//true表示raw数据(连续不等像素，不压缩原始)， false表示使用RLE数据（连续相等可压缩）
		//生成一个块数据(连续相等或连续不等)
		while (curpix + run_length < npixels && run_length < max_chunk_length) 
		{
			//判断下一个像素值是否与当前像素值相等
			bool succ_eq = true;
			for (int t = 0; succ_eq && t < bytespp; t++) 
			{
				succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);
			}
			curbyte += bytespp;//移至下一个像素

			//处理第一像素时即可确认本块后续是否按raw处理
			if (1 == run_length) 
			{
				raw = !succ_eq;
			}
			//raw处理时，遇到相等则退出
			if (raw && succ_eq) 
			{
				run_length--;
				break;
			}
			//非raw处理，遇到不相等 则退出
			if (!raw && !succ_eq) 
			{
				break;
			}
			run_length++;
		}
		curpix += run_length;
		//写入块数据（块开头x=chunk[0] 0~127代表后续为不连续x个raw数据，128~255代表后续x-127个连续chunk[1]数据）
		out.put(raw ? run_length - 1 : run_length + 127);
		if (!out.good()) 
		{
			std::cerr << "can't dump the tga file\n";
			return false;
		}
		out.write((char*)(data + chunkstart), (raw ? run_length * bytespp : bytespp));
		if (!out.good()) 
		{
			std::cerr << "can't dump the tga file\n";
			return false;
		}
	}
	return true;
}

TGAImage::TGAImage(): data(NULL),width(0),height(0),bytespp(0)
{
}

TGAImage::TGAImage(int w, int h, int bpp)
{
	width = w;
	height = h;
	bytespp = bpp;
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	memset(data, 0, nbytes); //初始化为0
}

TGAImage::TGAImage(const TGAImage& img)
{
	width = img.width;
	height = img.height;
	bytespp = img.bytespp;
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	memcpy(data, img.data, nbytes); //拷贝数据
}

bool TGAImage::read_tga_file(const char* filename)
{
	if (data)	delete[] data;
	data = NULL;
	std::ifstream in;
	//以二进制方式打开文件
	in.open(filename, std::ios::binary);
	if (!in.is_open())
	{
		std::cerr << "can't open file" << filename << std::endl;
		in.close();
		return false;
	}
	//读取头部信息
	TGA_Header header;
	in.read((char*)&header, sizeof(header));
	if (!in.good())
	{
		std::cerr << "an error occured while read the header" << std::endl;
		return false;
	}
	//获得宽高、位深信息
	width = header.width;
	height = header.height;
	bytespp = header.bitsperpixel >> 3;//除以8(注意是bits到bytes)
	//检查宽高、位深信息
	if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA))
	{
		in.close();
		std::cerr << "bad bpp(or width/height) value" << std::endl;
		return false;
	}
	//计算图像数据大小
	unsigned long nbytes = bytespp * width * height;
	//分配内存
	data = new unsigned char[nbytes];
	//读取图像数据
	if (UNCOMPRESSED_GRAYSCLAE == header.datatypecode || UNCOMPRESSED_TRUECOLOR == header.datatypecode)
	{
		//未压缩数据，直接读取
		in.read((char*)data, nbytes);
		if (!in.good())
		{
			in.close();
			std::cerr << "an error occured while read the data" << std::endl;
			return false;
		}
	}
	else if (RLE_TRUECOLOR == header.datatypecode || RLE_GRAYSCLAE == header.datatypecode)
	{
		//RLE压缩数据
		if (!load_rle_data(in))
		{
			in.close();
			std::cerr << "an error occured while read the data" << std::endl;
			return false;
		}
	}
	else
	{
		//暂不支持color-map图
		in.close();
		std::cerr << "unknown file format:" <<(int)header.datatypecode<< std::endl;
		return false;
	}
	//检查是否需要翻转（默认都转为从上到下-从左到右格式）
	if (header.imagedescriptor & 0x01) //bit4=1为从右到左
		flip_horizontally();
	if(!(header.imagedescriptor & 0x20)) //bit5=1为从上到下
		flip_vertically();
	std::cout<<width<<"x"<<height<<"/"<<bytespp*8<<std::endl;
	in.close();
	return true;
}

bool TGAImage::write_tga_file(const char* filename, bool rle)
{
	//打开文件
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out.is_open())
	{
		std::cerr << "can't open file" << filename << std::endl;
		out.close();
		return false;
	}
	//写入头部信息
	TGA_Header header;
	memset(&header, 0, sizeof(header));
	header.width = width;
	header.height = height;
	header.bitsperpixel = bytespp << 3; //bytespp*8
	header.imagedescriptor = 0x20;//原点为左上角
	if (bytespp == GRAYSCALE)
	{
		header.datatypecode = (rle ? RLE_GRAYSCLAE : UNCOMPRESSED_GRAYSCLAE);
	}
	else
	{
		header.datatypecode = (rle ? RLE_TRUECOLOR : UNCOMPRESSED_TRUECOLOR);
	}
	out.write((char*)&header, sizeof(header));
	if (!out.good())
	{
		out.close();
		std::cerr << "can't dump the tga file" << std::endl;
		return false;
	}
	//写入图像数据
	if (!rle)
	{
		//未压缩数据
		out.write((char*)data, width * height * bytespp);
		if (!out.good())
		{
			std::cerr << "can't write raw data" << std::endl;
			out.close();
			return false;
		}
	}
	else
	{
		//RLE压缩数据
		if (!unload_rle_data(out))
		{
			out.close();
			std::cerr << "can't write rle data" << std::endl;
			return false;
		}
	}
	//写入developer area
	unsigned char developer_area_ref[4] = { 0,0,0,0 };
	out.write((char*)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	//写入扩展区域
	unsigned char extension_area_ref[4] = { 0,0,0,0 };
	out.write((char*)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	//写入footer
	unsigned char footer[18] = { 'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0' };
	out.write((char*)footer, sizeof(footer));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}

	out.close();
	return true;
}

bool TGAImage::flip_horizontally()
{
	if (!data) return false;
	int half = width >> 1;
	for (int i = 0; i < half; i++)
	{
		for (int j = 0; j < height; j++)
		{
			TGAColor c1 = get(i, j);
			TGAColor c2 = get(width - 1 - i, j);
			set(i, j, c2);
			set(width - 1 - i, j, c1);		
		}
	}
	return true;
}

bool TGAImage::flip_vertically()
{
	if (!data) return false;
	int half = height >> 1;
	for (int i = 0; i < half; i++)
	{
		for (int j = 0; j < width; j++)
		{
			TGAColor c1 = get(j, i);
			TGAColor c2 = get(j, height-1-i);
			set(j, i, c2);
			set(j, height - 1 - i, c1);
		}
	}
	return true;
}

bool TGAImage::scale(int w, int h)
{
	return false;
}

TGAColor TGAImage::get(int x, int y)
{
	//输入条件保护
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
		return TGAColor();

	return TGAColor(data+(x+y*width)*bytespp,bytespp);
}

bool TGAImage::set(int x, int y, TGAColor c)
{
	//输入条件保护
	if (!data || x < 0 || y < 0 || x >= width || y >= height || c.bytespp < bytespp)
		return false;

	memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);
	return true;
}

TGAImage::~TGAImage()
{
	if (data)	//注意指针保护
		delete[] data;
}

TGAImage& TGAImage::operator=(const TGAImage& img)
{
	if (this != &img)
	{
		if (data != nullptr)//删除原有数据
			delete[] data;
		width = img.width;
		height = img.height;
		bytespp = img.bytespp;
		unsigned long nbytes = width * height * bytespp;
		data = new unsigned char[nbytes];
		memcpy(data,img.data, nbytes);//拷贝数据
	}
	return *this;
}

int TGAImage::get_width() const
{
	return width;
}

int TGAImage::get_height() const
{
	return height;
}

int TGAImage::get_bytespp()
{
	return bytespp;
}

unsigned char* TGAImage::buffer()
{
	return data;	//不安全,可能被外部修改
}

void TGAImage::clear()
{
	memset(data, 0, width * height * bytespp);
}
