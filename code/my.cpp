#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <math.h>
//Eigen/LU包含了矩阵求逆、计算行列式等操作
#include <Eigen/LU>
//Eigen/Core包含了矩阵基础操作
#include <Eigen/Core>


using namespace Eigen;
using namespace std;


void ForwardIntersection(const char* path, const char* path_para, int point_number)
{
	ofstream output;
	output.open("output.txt");

	//读入图片参数
	int i, j, m;
	int photo_num;
	FILE* para = fopen(path_para, "r");
	char temp[512];
	for (i = 0; i < 3; i++) 
	{
		fgets(temp, 512, para);//读入注释行
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

	//读入点数
	int	ctrlpt = 0, readNum = 0;
	int nSpcPoints = 0, nSpcLines = 0;
	FILE* fppts = fopen(path, "r");
	char tmpchar[512];
	for (i = 0; i < 5; i++)
	{
		fgets(tmpchar, 512, fppts);	//读入注释行
	}
	fgets(tmpchar, 512, fppts);	//读入总点数行
	readNum = sscanf(tmpchar, "%d %d\n", &nSpcPoints, &nSpcLines);

	//对每个空间中的点进行平差
	if (point_number > 0) {
		nSpcPoints = point_number;//设置要处理的点数
	}
	int round = 10000000;//设置空间点的误差范围
	for (m = 1; m <= nSpcPoints; m++)
	{
		//如果读到文件末尾就结束
		if (feof(fppts))
			break;

		ctrlpt = 0; readNum = 0;
		strcpy(tmpchar, "");
		fgets(tmpchar, 512, fppts);	//读入空间点行
		int ptnum;
		double X, Y, Z, attrib;
		readNum = sscanf(tmpchar, "%d %lf %lf %lf %d\n", &ptnum, &X, &Y, &Z, &attrib);//读入空间点的真值

		fgets(tmpchar, 512, fppts);	//读入空间点对应像点数行

		int ImgNum;
		sscanf(tmpchar, "%d\n", &ImgNum);//读入含有此空间点的相片数

		//对于空间中的每一个点
		VectorXd x(ImgNum * 2);//像点坐标
		MatrixXd Img(ImgNum, 6);//相片的方位元素
		VectorXd X0(3);//空间点坐标初值
		Vector3d Xreal;//空间点坐标真值
		IOFormat OctaveFmt(FullPrecision, 0, ", ", ";\n", "", "", "[", "]");//格式化输出

		output << "第" << m << "个点的真值:" << endl;
		Xreal << X, Y, Z;
		output << Xreal.transpose().format(OctaveFmt) << endl;
		X0 << double(int(X) / round * round), double(int(Y) / round * round), double(int(Z) / round * round);
		output << "第" << m << "个点的初始近似值:" << endl;
		output << X0.transpose().format(OctaveFmt) << endl;


		for (j = 0; j < ImgNum; j++)
		{
			int num;
			double gnum;
			fgets(tmpchar, 512, fppts);	//读入像点行
			sscanf(tmpchar, "%d %lf %lf %d\n", &num, &x[2 * j], &x[2 * j + 1], &gnum);
			for (int k = 0; k < photo_num; k++) {
				if (num == photo_No[k]) {//找出对应相片的方位元素
					Img.row(j) = photo_para.row(k);
				}
			}
		}


		MatrixXd B(ImgNum * 2, 3);
		VectorXd l(ImgNum * 2);
		VectorXd Xbar(3);//Xbar，Ybar，Zbar
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
			for (i = 0; i < ImgNum * 2; i++) {//计算每个点旋转矩阵，进而确定该点在B矩阵中的参数

				R << cos(Img(i / 2, 3)) * cos(Img(i / 2, 5)) - sin(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * sin(Img(i / 2, 5)),
					-cos(Img(i / 2, 3)) * sin(Img(i / 2, 5)) - sin(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * cos(Img(i / 2, 5)),
					-sin(Img(i / 2, 3)) * cos(Img(i / 2, 4)),
					cos(Img(i / 2, 4))* sin(Img(i / 2, 5)),
					cos(Img(i / 2, 4))* cos(Img(i / 2, 5)),
					-sin(Img(i / 2, 4)),
					sin(Img(i / 2, 3))* cos(Img(i / 2, 5)) + cos(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * sin(Img(i / 2, 5)),
					-sin(Img(i / 2, 3)) * sin(Img(i / 2, 5)) + cos(Img(i / 2, 3)) * sin(Img(i / 2, 4)) * cos(Img(i / 2, 5)),
					cos(Img(i / 2, 3))* cos(Img(i / 2, 4));

				//计算Xbar，Ybar，Zbar
				for (j = 0; j < 3; j++) {
					Xbar[j] = R(0, j) * (X0[0] - Img(i / 2, 0)) + R(1, j) * (X0[1] - Img(i / 2, 1)) + R(2, j) * (X0[2] - Img(i / 2, 2));
				}

				//确定B和l
				double f = 105.2;
				B(i, 0) = -(1 / Xbar[2]) * (R(0, i % 2) * f + R(0, 2) * x[i]);
				B(i, 1) = -(1 / Xbar[2]) * (R(1, i % 2) * f + R(1, 2) * x[i]);
				B(i, 2) = -(1 / Xbar[2]) * (R(2, i % 2) * f + R(2, 2) * x[i]);
				l[i] = x[i] + f * Xbar[i % 2] / Xbar[2];
			}
			//计算xhat
			Nbb = B.transpose() * P * B;
			W = B.transpose() * P * l;
			xhat = Nbb.inverse() * W;

			V = B * xhat - l;
			sigma0 = sqrt((V.transpose() * P * V)(0, 0) / ((ImgNum * 2) - 3));
			DXX = sigma0 * sigma0 * Nbb.inverse();
			X0 = xhat + X0;
			output << endl;
			output << "第" << m << "个点在第" << turn << "轮平差后的改正数：" << endl;
			output << xhat.transpose().format(OctaveFmt) << endl;
			output << "第" << m << "个点在第" << turn << "轮平差后的平差值：" << endl;
			output << X0.transpose().format(OctaveFmt) << endl;
			output << "第" << m << "个点在第" << turn << "轮平差后的单位权中误差：" << endl;
			output << sigma0 << endl;
			output << "第" << m << "个点在第" << turn << "轮平差后的未知数中误差：" << endl;
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
	const char* path_para = "Data.pht";//输入相片方位元素文件路径
	const char* path = "Data.pts";//输入点文件路径
	int point_number = -1;//设置需要处理点的个数，默认处理所有点
	ForwardIntersection(path, path_para, point_number);
}