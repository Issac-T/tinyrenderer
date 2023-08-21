// tinyrenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor red2 = TGAColor(255, 0, 0, 100);
const TGAColor green = TGAColor(0, 255, 0, 255); 
const TGAColor blue = TGAColor(0, 0, 255, 255);

int main()
{
    TGAImage img(200,100,TGAImage::RGBA);
    img.set(52,41, red);
    img.set(52,42, red2);
    img.set(20, 20, white);
    img.write_tga_file("tga_image.tga", 0);
    img.write_tga_file("tga_image_rle.tga", 1);

    TGAImage img2(200, 100, 24);
    img2.read_tga_file("tga_image.tga");
    TGAColor color = img2.get(52, 41);
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
