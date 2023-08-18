#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <math.h>
//Eigen/LU�����˾������桢��������ʽ�Ȳ���
#include <Eigen/LU>
//Eigen/Core�����˾����������
#include <Eigen/Core>


using namespace Eigen;
using namespace std;


void ForwardIntersection(const char* path, const char* path_para, int point_number)
{
	ofstream output;
	output.open("output.txt");

	//����ͼƬ����
	int i, j, m;
	int photo_num;
	FILE* para = fopen(path_para, "r");
	char temp[512];
	for (i = 0; i < 3; i++) 
	{
		fgets(temp, 512, para);//����ע����
	}
	fgets(temp, 512, para);
	sscanf(temp, "%d\n", &photo_num);
	VectorXi photo_No(photo_num);
	MatrixXd photo_para(photo_num, 6);
	for (i = 0; i < photo_num; i++) {
		fgets(temp, 512, para);
		sscanf(temp, "%d %lf %lf %lf %lf %lf %lf\n", &photo_No[i], &photo_para(i, 0), &photo_para(i, 1), &photo_para(i, 2), &photo_para(i, 3), &photo_para(i, 4), \
			& photo_para(i, 5));
	}
	fclose(para);

	//�������
	int	ctrlpt = 0, readNum = 0;
	int nSpcPoints = 0, nSpcLines = 0;
	FILE* fppts = fopen(path, "r");
	char tmpchar[512];
	for (i = 0; i < 5; i++)
	{
		fgets(tmpchar, 512, fppts);	//����ע����
	}
	fgets(tmpchar, 512, fppts);	//�����ܵ�����
	readNum = sscanf(tmpchar, "%d %d\n", &nSpcPoints, &nSpcLines);

	//��ÿ���ռ��еĵ����ƽ��
	if (point_number > 0) {
		nSpcPoints = point_number;//����Ҫ����ĵ���
	}
	int round = 10000000;//���ÿռ�����Χ
	for (m = 1; m <= nSpcPoints; m++)
	{
		//��������ļ�ĩβ�ͽ���
		if (feof(fppts))
			break;

		ctrlpt = 0; readNum = 0;
		strcpy(tmpchar, "");
		fgets(tmpchar, 512, fppts);	//����ռ����
		int ptnum;
		double X, Y, Z, attrib;
		readNum = sscanf(tmpchar, "%d %lf %lf %lf %d\n", &ptnum, &X, &Y, &Z, &attrib);//����ռ�����ֵ

		fgets(tmpchar, 512, fppts);	//����ռ���Ӧ�������

		int ImgNum;
		sscanf(tmpchar, "%d\n", &ImgNum);//���뺬�д˿ռ�����Ƭ��

		//���ڿռ��е�ÿһ����
		VectorXd x(ImgNum * 2);//�������
		MatrixXd Img(ImgNum, 6);//��Ƭ�ķ�λԪ��
		VectorXd X0(3);//�ռ�������ֵ
		Vector3d Xreal;//�ռ��������ֵ
		IOFormat OctaveFmt(FullPrecision, 0, ", ", ";\n", "", "", "[", "]");//��ʽ�����

		output << "��" << m << "�������ֵ:" << endl;
		Xreal << X, Y, Z;
		output << Xreal.transpose().format(OctaveFmt) << endl;
		X0 << double(int(X) / round * round), double(int(Y) / round * round), double(int(Z) / round * round);
		output << "��" << m << "����ĳ�ʼ����ֵ:" << endl;
		output << X0.transpose().format(OctaveFmt) << endl;


		for (j = 0; j < ImgNum; j++)
		{
			int num;
			double gnum;
			fgets(tmpchar, 512, fppts);	//���������
			sscanf(tmpchar, "%d %lf %lf %d\n", &num, &x[2 * j], &x[2 * j + 1], &gnum);
			for (int k = 0; k < photo_num; k++) {
				if (num == photo_No[k]) {//�ҳ���Ӧ��Ƭ�ķ�λԪ��
					Img.row(j) = photo_para.row(k);
				}
			}
		}


		MatrixXd B(ImgNum * 2, 3);
		VectorXd l(ImgNum * 2);
		VectorXd Xbar(3);//Xbar��Ybar��Zbar
		MatrixXd P(ImgNum * 2, ImgNum * 2);
		MatrixXd R(3, 3);
		VectorXd xhat(3);
		MatrixXd Nbb;
		MatrixXd W;
		VectorXd V(12);
		MatrixXd DXX;
		double sigma0;
		P.setIdentity();
		int turn;


		for (turn = 1; turn <= 10; turn++) {
			for (i = 0; i < ImgNum * 2; i++) {//����ÿ������ת���󣬽���ȷ���õ���B�����еĲ���

				R << cos(Img(i / 2, 3)) * cos(Img(i / 2, 5)) - sin(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * sin(Img(i / 2, 5)),
					-cos(Img(i / 2, 3)) * sin(Img(i / 2, 5)) - sin(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * cos(Img(i / 2, 5)),
					-sin(Img(i / 2, 3)) * cos(Img(i / 2, 4)),
					cos(Img(i / 2, 4))* sin(Img(i / 2, 5)),
					cos(Img(i / 2, 4))* cos(Img(i / 2, 5)),
					-sin(Img(i / 2, 4)),
					sin(Img(i / 2, 3))* cos(Img(i / 2, 5)) + cos(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * sin(Img(i / 2, 5)),
					-sin(Img(i / 2, 3)) * sin(Img(i / 2, 5)) + cos(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * cos(Img(i / 2, 5)),
					cos(Img(i / 2, 3))* cos(Img(i / 2, 4));

				//����Xbar��Ybar��Zbar
				for (j = 0; j < 3; j++) {
					Xbar[j] = R(0, j) * (X0[0] - Img(i / 2, 0)) + R(1, j) * (X0[1] - Img(i / 2, 1)) + R(2, j) * (X0[2] - Img(i / 2, 2));
				}

				//ȷ��B��l
				double f = 105.2;
				B(i, 0) = -(1 / Xbar[2]) * (R(0, i % 2) * f + R(0, 2) * x[i]);
				B(i, 1) = -(1 / Xbar[2]) * (R(1, i % 2) * f + R(1, 2) * x[i]);
				B(i, 2) = -(1 / Xbar[2]) * (R(2, i % 2) * f + R(2, 2) * x[i]);
				l[i] = x[i] + f * Xbar[i % 2] / Xbar[2];
			}
			//����xhat
			Nbb = B.transpose() * P * B;
			W = B.transpose() * P * l;
			xhat = Nbb.inverse() * W;

			V = B * xhat - l;
			sigma0 = sqrt((V.transpose() * P * V)(0, 0) / ((ImgNum * 2) - 3));
			DXX = sigma0 * sigma0 * Nbb.inverse();
			X0 = xhat + X0;
			output << endl;
			output << "��" << m << "�����ڵ�" << turn << "��ƽ���ĸ�������" << endl;
			output << xhat.transpose().format(OctaveFmt) << endl;
			output << "��" << m << "�����ڵ�" << turn << "��ƽ����ƽ��ֵ��" << endl;
			output << X0.transpose().format(OctaveFmt) << endl;
			output << "��" << m << "�����ڵ�" << turn << "��ƽ���ĵ�λȨ����" << endl;
			output << sigma0 << endl;
			output << "��" << m << "�����ڵ�" << turn << "��ƽ����δ֪������" << endl;
			output << DXX.format(OctaveFmt) << endl;
			if (abs(xhat[0]) < 0.5 && abs(xhat[1]) < 0.5 && abs(xhat[2]) < 0.5)
				break;
		}
		output << "*********************************************************************************************" << endl;

	}
	output.close();
	fclose(fppts);

}

void main()
{
	const char* path_para = "Data.pht";//������Ƭ��λԪ���ļ�·��
	const char* path = "Data.pts";//������ļ�·��
	int point_number = -1;//������Ҫ�����ĸ�����Ĭ�ϴ������е�
	ForwardIntersection(path, path_para, point_number);
}