// tinyrenderer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <math.h>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor red2 = TGAColor(255, 0, 0, 100);
const TGAColor green = TGAColor(0, 255, 0, 255); 
const TGAColor blue = TGAColor(0, 0, 255, 255);

using std::abs;

void line(int x0, int y0, int x1, int y1, const TGAColor& color, TGAImage& img)
{
    float delta_x = (x1 - x0);
    float delta_y = (y1 - y0);
    int counts = std::max(abs(delta_x), abs(delta_y));
    float step_x = delta_x / counts;
    float step_y = delta_y / counts;
    for (int i = 0; i < counts; i++)
    {
        int x = x0+i*step_x;
		int y = y0 + i * step_y;
		img.set(x, y, color);
    }
}

void mesh(Model& model, const TGAColor& color, TGAImage& img,int scale)
{
    int nf = model.nfaces();
    //原点移至img中心
    int x0 = img.get_width() / 2;
    int y0 = img.get_height() / 2;
    //绘制每个面
    for (int i = 0; i < nf; i++)
    {
        std::vector<int> cur_face = model.face(i);
        Vec3f v1, v2;
        //依次连接该面的每条边
        for (int j = 1; j < cur_face.size(); j++)
        {
            v1 = model.vert(cur_face[j - 1])* scale;
            v2 = model .vert(cur_face[j])* scale;
            line(v1.x + x0, v1.y + y0, v2.x + x0, v2.y + y0, color, img);
        }
        v1 = model.vert(cur_face[cur_face.size()-1]) * scale;
        v2 = model.vert(cur_face[0]) * scale;
        line(v1.x+ x0, v1.y+ y0, v2.x+ x0, v2.y+y0, color, img);
    }
    return;
}

void draw_line_test()
{
    TGAImage img(200, 100, TGAImage::RGB);
    img.set(52, 41, red);
    img.set(52, 42, red2);
    img.set(20, 20, white);
    line(10, 10, 88, 55, blue, img);
    line(50, 68, 185, 20, red, img);
    line(13, 20, 80, 40, white, img);
    line(20, 13, 40, 80, red, img);
    line(80, 40, 13, 20, red, img);
    img.write_tga_file("tga_image.tga", 0);
    img.write_tga_file("tga_image_rle.tga", 1);

    TGAImage img2(200, 100, 24);
    img2.read_tga_file("tga_image.tga");
    TGAColor color = img2.get(52, 41);
    std::cout << "Hello World!\n";
}

int main()
{
    Model model("obj/african_head.obj");//读取obj文件
    //根据obj面数据绘制所有三角面网格线
    TGAImage img(2000, 2000, TGAImage::RGB);
    mesh(model, white, img,1000);
    img.flip_vertically();
    img.write_tga_file("obj_mesh.tga", 0);
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
