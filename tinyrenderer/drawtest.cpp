#include "drawtest.h"




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

void draw_mesh_test()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�
	//����obj�����ݻ�������������������
	TGAImage img(2000, 2000, TGAImage::RGB);
	mesh(model, white, img, 1000);
	img.flip_vertically();
	img.write_tga_file("obj_mesh1.tga", 0);
}

void draw_triangle_test()
{
	TGAImage image(500, 500, TGAImage::RGB);
	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	triangle_simple(t0[0], t0[1], t0[2], red, image);
	//triangle(t1[0], t1[1], t1[2], white, image);
	triangle_barycentric(t1, white, image);
	triangle_simple(t2[0], t2[1], t2[2], green, image);
	triangle_bound(t2[0], t2[1], t2[2], blue, image);
	image.flip_vertically();
	image.write_tga_file("triangle_fill.tga");
}

void draw_mesh_triangle_test()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�
	//����obj�����ݻ�������������������
	int width = 2000;
	int height = 2000;
	int scale = width / 2;
	TGAImage image(width, height, TGAImage::RGB);
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		Vec3f pts[3] = { model.vert(model.face(i)[0]), model.vert(model.face(i)[1]), model.vert(model.face(i)[2]) };
		//ֻ��ȡxyͶӰ
		Vec2f pts2[3] = { Vec2f(pts[0].x, pts[0].y), Vec2f(pts[1].x, pts[1].y), Vec2f(pts[2].x, pts[2].y) };
		//����ƽ��
		Vec2i pts3[3];
		for (int j = 0; j < 3; j++)
		{
			pts3[j] = Vec2i(pts2[j].x * scale + width / 2, pts2[j].y * scale + height / 2);
		}
		//����������(�������RGB��ɫ)
		triangle_barycentric(pts3, TGAColor(rand() % 255, rand() % 255, rand() % 255), image);
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle1.tga");
}

void draw_mesh_shadow_test()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�
	//����obj�����ݻ�������������������
	int width = 2000;
	int height = 2000;
	int scale = width / 2;
	TGAImage image(width, height, TGAImage::RGB);
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		Vec3f pts[3] = { model.vert(model.face(i)[0]), model.vert(model.face(i)[1]), model.vert(model.face(i)[2]) };
		//ֻ��ȡxyͶӰ
		Vec2f pts2[3] = { Vec2f(pts[0].x, pts[0].y), Vec2f(pts[1].x, pts[1].y), Vec2f(pts[2].x, pts[2].y) };
		//����ƽ��
		Vec2i pts3[3];
		for (int j = 0; j < 3; j++)
		{
			pts3[j] = Vec2i(pts2[j].x * scale + width / 2, pts2[j].y * scale + height / 2);
		}
		//��������Ч��-�����淨�����͹��շ������
		Vec3f fn = get_face_normal(pts);//�淨��
		Vec3f light_dir(0, 0, -1);//���շ���-Z�Ḻ����
		float intensity = (fn * light_dir);//����ǿ��(�������)
		if (intensity > 0)	//������ߵ��治����
		{
			//����������
			//triangle_barycentric(pts3, TGAColor(intensity*255, intensity*255, intensity*255), image);
			triangle_barycentric(pts3, TGAColor(rand() % 255, rand() % 255, rand() % 255), image);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_visible.tga");

}

void draw_depth_test_line()
{
	const int width = 800, height = 1;
	TGAImage image(width, height, TGAImage::RGB);
	float yBuffer[width];//��Ȼ���
	//memset(yBuffer, -1, sizeof(yBuffer));

	rasterize_line(Vec2i(20, 34), Vec2i(744, 400), red, image, yBuffer);
	rasterize_line(Vec2i(120, 434), Vec2i(444, 400), green, image, yBuffer);
	rasterize_line(Vec2i(330, 463), Vec2i(594, 200), blue, image, yBuffer);

	image.flip_vertically();
	image.write_tga_file("z_buffer_test_line.tga");
}

void draw_depth_test_triangle()
{


	Model model("obj/african_head.obj");//��ȡobj�ļ�

	int width = 2000;
	int height = 2000;
	int scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);
	//��ʼ��zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//����obj�����ݻ�������������������
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		Vec3f pts[3] = { model.vert(model.face(i)[0]), model.vert(model.face(i)[1]), model.vert(model.face(i)[2]) };
		//����ƽ��
		Vec3f pts_scaled[3];
		for (int j = 0; j < 3; j++)
		{
			pts_scaled[j] = pts[j] * scale + Vec3f(width / 2, height / 2, 0);
		}
		//��������Ч��-�����淨�����͹��շ������
		Vec3f fn = get_face_normal(pts);//�淨��
		Vec3f light_dir(0, 0, -1);//���շ���-Z�Ḻ����
		float intensity = (fn * light_dir);//����ǿ��(�������)
		if (intensity > 0)	//������ߵ��治����
		{
			//����������
			rasterize_triangle(pts_scaled, zBuffer, TGAColor(intensity * 255, intensity * 255, intensity * 255), image);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_visible_zBuffer.tga");

	//todo ����zBuffer���ͼ
}

void draw_mesh_texture_test()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�

	int width = 2000;
	int height = 2000;
	int scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);

	//��ʼ��zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//����obj�����ݻ�������������������
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		std::vector<int> v_index = model.face(i);
		Vec3f pts[3] = { model.vert(v_index[0]), model.vert(v_index[1]), model.vert(v_index[2]) };

		//����ƽ��
		Vec3f pts_scaled[3];
		for (int j = 0; j < 3; j++)
		{
			pts_scaled[j] = pts[j] * scale + Vec3f(width / 2, height / 2, 0);
		}
		//��������Ч��-�����淨�����͹��շ������
		Vec3f fn = get_face_normal(pts);//�淨��
		Vec3f light_dir(0, 0, -1);//���շ���-Z�Ḻ����
		float intensity = (fn * light_dir);//����ǿ��(�������)
		if (intensity > 0)	//������ߵ��治����
		{
			//����������
			//rasterize_triangle_texture(pts_scaled, pts_texture,zBuffer,image,texture);
			rasterize_triangle_texture(pts_scaled, zBuffer, i, model, image, intensity);
		}
	}

	image.flip_vertically();
	//image.write_tga_file("mesh_triangle_texture.tga");
	image.write_tga_file("mesh_triangle_texture_intensity.tga");

	//todo ����zBuffer���ͼ
}

void draw_central_projection_test()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�

	int width = 2000;
	int height = 2000;
	int scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);

	//Vec3f camera_pos(0,0,2000);//���Z��λ��
	Vec3f camera_pos(width / 2, height / 2, 3000);//���Z��λ��

	//��ʼ��zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//����obj�����ݻ�������������������
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		std::vector<int> v_index = model.face(i);

		Vec3f pts[3] = { model.vert(v_index[0]), model.vert(v_index[1]), model.vert(v_index[2]) };

		//��������Ч��-�����淨�����͹��շ������
		Vec3f fn = get_face_normal(pts);//�淨��
		Vec3f light_dir(0, 0, -1);//���շ���-Z�Ḻ����
		float intensity = (fn * light_dir);//����ǿ��(�������)

		if (intensity > 0)	//������ߵ��治����
		{
			//���� ԭ��ƽ������������
			Vec3f pts_scaled[3];
			for (int j = 0; j < 3; j++)
			{
				pts_scaled[j] = pts[j] * scale + Vec3f(width / 2, height / 2, 0);
			}

			//�Ը���������͸�ӱ任
			Vec3f pts_projected[3];
			for (int k = 0; k < 3; k++)
				pts_projected[k] = central_projection(pts_scaled[k], camera_pos);

			//����������
			rasterize_triangle_texture(pts_projected, zBuffer, i, model, image, intensity);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_texture_intensity_projection1.tga");

	//todo ����zBuffer���ͼ
}

void draw_central_projection_test2()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�

	int width = 2000;
	int height = 2000;
	float scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);

	//Vec3f camera_pos(0,0,2000);//���Z��λ��
	Vec3f camera_pos(width / 2, height / 2, 3000);//���Z��λ��
	//���ɱ任����
	Matrix M_trans_scale = trans_scale_matrix(width, height, scale);
	Matrix M_project = camera_matrix(camera_pos);
	Matrix M_total = M_project * M_trans_scale;
	std::cout << "M_trans_scale:\n" << M_trans_scale;
	std::cout << "M_project:\n" << M_project;

	//��ʼ��zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//����obj�����ݻ�������������������
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		std::vector<int> v_index = model.face(i);

		Vec3f pts[3] = { model.vert(v_index[0]), model.vert(v_index[1]), model.vert(v_index[2]) };

		//��������Ч��-�����淨�����͹��շ������
		Vec3f fn = get_face_normal(pts);//�淨��
		Vec3f light_dir(0, 0, -1);//���շ���-Z�Ḻ����
		float intensity = (fn * light_dir);//����ǿ��(�������)

		if (intensity > 0)	//ֻ����������ߵ���
		{
			//�任���������ϵ
			Vec3f pts_transed[3];
			for (int j = 0; j < 3; j++)
			{
				pts_transed[j] = proj<3>(M_total * embed<4>(pts[j]));
			}

			//����������
			rasterize_triangle_texture(pts_transed, zBuffer, i, model, image, intensity);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_triangle_texture_intensity_projection2.tga");

	//todo ����zBuffer���ͼ
}

void draw_move_camera_test()
{
	Model model("obj/african_head.obj");//��ȡobj�ļ�

	int width = 2000;
	int height = 2000;
	float scale = width / 2.1;
	TGAImage image(width, height, TGAImage::RGB);

	Vec3f camera_pos(2, 2, 3);//���λ��
	Vec3f camera_center(0, 0, 0);//����ӵ�(�������ϵԭ��)
	Vec3f camera_up(0, 1, 0);//����������Ϸ���
	Vec3f view_dir = camera_pos - camera_center;
	view_dir.normalize();
	//���ɱ任����
	Matrix M_model = Matrix::identity();	//mesh��ʼ�����ƶ�����
	Matrix M_view = u_lookat(camera_pos, camera_center, camera_up);	//�任������ռ�
	Matrix M_project = camera_matrix((camera_pos - camera_center).norm());//͸��ͶӰ�任
	Matrix M_viewport = viewport_matrix(0, 0, width, height);//������������
	Matrix M_total = M_viewport * M_project * M_view * M_model;
	std::cout << "M_model:\n" << M_model;
	std::cout << "M_view:\n" << M_view;
	std::cout << "M_project:\n" << M_project;
	std::cout << "M_viewport:\n" << M_viewport;

	//��ʼ��zBuffer
	float* zBuffer = new float[width * height];
	for (int i = 0; i < width * height; i++)
	{
		zBuffer[i] = -std::numeric_limits<float>::max();
	}

	//����obj�����ݻ�������������������
	for (int i = 0; i < model.nfaces(); i++)
	{
		//�������������
		std::vector<int> v_index = model.face(i);

		Vec3f pts[3] = { model.vert(v_index[0]), model.vert(v_index[1]), model.vert(v_index[2]) };

		//��������Ч��-�����淨�����͹��շ������
		Vec3f fn = get_face_normal(pts);//�淨��
		//Vec3f light_dir(0, 0, -1);//���շ���-Z�Ḻ����
		Vec3f light_dir = view_dir * (-1);//���շ���������߷���
		float intensity = (fn * light_dir);//����ǿ��(�������)

		if (intensity > 0)	//ֻ����������ߵ���
		{
			//�任���������ϵ
			Vec3f pts_transed[3];
			for (int j = 0; j < 3; j++)
			{
				pts_transed[j] = proj<3>(M_total * embed<4>(pts[j]));
			}

			//����������
			rasterize_triangle_texture(pts_transed, zBuffer, i, model, image, intensity);
		}
	}

	image.flip_vertically();
	image.write_tga_file("mesh_camera_move_rebuild.tga");

	//todo ����zBuffer���ͼ
	TGAImage depthmap(width, height, TGAImage::RGB);
	for (int i = 0; i < width * height; i++)
		depthmap.set(i % width, i / height, TGAColor(zBuffer[i], zBuffer[i], zBuffer[i]));
	depthmap.flip_vertically();
	depthmap.write_tga_file("mesh_camera_move_depthmap_rebuild.tga");
}

