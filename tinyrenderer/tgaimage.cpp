#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <math.h>
#include "tgaimage.h"

//RLE(�γ̱���)���ݶ�ȡ
bool TGAImage::load_rle_data(std::ifstream& in)
{
	unsigned long pixelcount = width * height;
	unsigned long currentpixel = 0;
	unsigned long currentbyte = 0;
	TGAColor colorbuffer;
	do {
		//����ͷ
		unsigned char chunkheader = 0;
		chunkheader = in.get();
		if (!in.good()) 
		{
			std::cerr << "an error occured while reading the data\n";
			return false;
		}
		
		if (chunkheader < 128) //����Ϊchunkheader+1����RLE����
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
		else //����Ϊchunkheader-127��RLE����
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
	const unsigned char max_chunk_length = 128; //���鳤��
	unsigned long npixels = width * height;
	unsigned long curpix = 0;
	while (curpix < npixels) 
	{
		unsigned long chunkstart = curpix * bytespp;
		unsigned long curbyte = curpix * bytespp;
		unsigned char run_length = 1;//��ǰ�Ѵ���鳤��(������Ȼ���������)
		bool raw = true;//true��ʾraw����(�����������أ���ѹ��ԭʼ)�� false��ʾʹ��RLE���ݣ�������ȿ�ѹ����
		//����һ��������(������Ȼ���������)
		while (curpix + run_length < npixels && run_length < max_chunk_length) 
		{
			//�ж���һ������ֵ�Ƿ��뵱ǰ����ֵ���
			bool succ_eq = true;
			for (int t = 0; succ_eq && t < bytespp; t++) 
			{
				succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);
			}
			curbyte += bytespp;//������һ������

			//�����һ����ʱ����ȷ�ϱ�������Ƿ�raw����
			if (1 == run_length) 
			{
				raw = !succ_eq;
			}
			//raw����ʱ������������˳�
			if (raw && succ_eq) 
			{
				run_length--;
				break;
			}
			//��raw������������� ���˳�
			if (!raw && !succ_eq) 
			{
				break;
			}
			run_length++;
		}
		curpix += run_length;
		//д������ݣ��鿪ͷx=chunk[0] 0~127�������Ϊ������x��raw���ݣ�128~255�������x-127������chunk[1]���ݣ�
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
	memset(data, 0, nbytes); //��ʼ��Ϊ0
}

TGAImage::TGAImage(const TGAImage& img)
{
	width = img.width;
	height = img.height;
	bytespp = img.bytespp;
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	memcpy(data, img.data, nbytes); //��������
}

bool TGAImage::read_tga_file(const char* filename)
{
	if (data)	delete[] data;
	data = NULL;
	std::ifstream in;
	//�Զ����Ʒ�ʽ���ļ�
	in.open(filename, std::ios::binary);
	if (!in.is_open())
	{
		std::cerr << "can't open file" << filename << std::endl;
		in.close();
		return false;
	}
	//��ȡͷ����Ϣ
	TGA_Header header;
	in.read((char*)&header, sizeof(header));
	if (!in.good())
	{
		std::cerr << "an error occured while read the header" << std::endl;
		return false;
	}
	//��ÿ�ߡ�λ����Ϣ
	width = header.width;
	height = header.height;
	bytespp = header.bitsperpixel >> 3;//����8(ע����bits��bytes)
	//����ߡ�λ����Ϣ
	if (width <= 0 || height <= 0 || (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA))
	{
		in.close();
		std::cerr << "bad bpp(or width/height) value" << std::endl;
		return false;
	}
	//����ͼ�����ݴ�С
	unsigned long nbytes = bytespp * width * height;
	//�����ڴ�
	data = new unsigned char[nbytes];
	//��ȡͼ������
	if (UNCOMPRESSED_GRAYSCLAE == header.datatypecode || UNCOMPRESSED_TRUECOLOR == header.datatypecode)
	{
		//δѹ�����ݣ�ֱ�Ӷ�ȡ
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
		//RLEѹ������
		if (!load_rle_data(in))
		{
			in.close();
			std::cerr << "an error occured while read the data" << std::endl;
			return false;
		}
	}
	else
	{
		//�ݲ�֧��color-mapͼ
		in.close();
		std::cerr << "unknown file format:" <<(int)header.datatypecode<< std::endl;
		return false;
	}
	//����Ƿ���Ҫ��ת��Ĭ�϶�תΪ���ϵ���-�����Ҹ�ʽ��
	if (header.imagedescriptor & 0x01) //bit4=1Ϊ���ҵ���
		flip_horizontally();
	if(!(header.imagedescriptor & 0x20)) //bit5=1Ϊ���ϵ���
		flip_vertically();
	std::cout<<width<<"x"<<height<<"/"<<bytespp*8<<std::endl;
	in.close();
	return true;
}

bool TGAImage::write_tga_file(const char* filename, bool rle)
{
	//���ļ�
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if (!out.is_open())
	{
		std::cerr << "can't open file" << filename << std::endl;
		out.close();
		return false;
	}
	//д��ͷ����Ϣ
	TGA_Header header;
	memset(&header, 0, sizeof(header));
	header.width = width;
	header.height = height;
	header.bitsperpixel = bytespp << 3; //bytespp*8
	header.imagedescriptor = 0x20;//ԭ��Ϊ���Ͻ�
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
	//д��ͼ������
	if (!rle)
	{
		//δѹ������
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
		//RLEѹ������
		if (!unload_rle_data(out))
		{
			out.close();
			std::cerr << "can't write rle data" << std::endl;
			return false;
		}
	}
	//д��developer area
	unsigned char developer_area_ref[4] = { 0,0,0,0 };
	out.write((char*)developer_area_ref, sizeof(developer_area_ref));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	//д����չ����
	unsigned char extension_area_ref[4] = { 0,0,0,0 };
	out.write((char*)extension_area_ref, sizeof(extension_area_ref));
	if (!out.good())
	{
		std::cerr << "can't dump the tga file" << std::endl;
		out.close();
		return false;
	}
	//д��footer
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
	//������������
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
		return TGAColor();

	return TGAColor(data+(x+y*width)*bytespp,bytespp);
}

bool TGAImage::set(int x, int y, TGAColor c)
{
	//������������
	if (!data || x < 0 || y < 0 || x >= width || y >= height || c.bytespp < bytespp)
		return false;

	memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);
	return true;
}

TGAImage::~TGAImage()
{
	if (data)	//ע��ָ�뱣��
		delete[] data;
}

TGAImage& TGAImage::operator=(const TGAImage& img)
{
	if (this != &img)
	{
		if (data != nullptr)//ɾ��ԭ������
			delete[] data;
		width = img.width;
		height = img.height;
		bytespp = img.bytespp;
		unsigned long nbytes = width * height * bytespp;
		data = new unsigned char[nbytes];
		memcpy(data,img.data, nbytes);//��������
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
	return data;	//����ȫ,���ܱ��ⲿ�޸�
}

void TGAImage::clear()
{
	memset(data, 0, width * height * bytespp);
}
