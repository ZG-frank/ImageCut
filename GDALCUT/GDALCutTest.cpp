#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "GDALAlgorithm.h"

using namespace std;

void fileopen(const char *filename)
{
	ifstream infile(filename, ios::in);
	if (!infile)
	{
		cout << "Can not open file" << endl;
		exit(1);
	}
}

int main()
{
	cout << "输入1 矩形裁剪"<<endl;
	cout << "输入2 标准自动裁剪" << endl;
	cout << "输入3 AOI裁剪" << endl;
	cout << "输入4 连续重采样（使用RasterIO方法）" << endl;
	cout << "输入5 图像重采样（使用GDALWarp方法）" << endl;
	cout << "输入6 单张重采样（使用RasterIO方法）" << endl;
	cout << "输入7 连续切割加重采样（从左往右）" << endl;
	cout << "输入8 单列连续切割重采样并自动命名" << endl;
	int iCode;
	cout << "输入代码： ";
	cin >> iCode;
	int iRev = RE_SUCCESS;  //0
	CConsoleProcess *pPro = new CConsoleProcess();

	switch (iCode)
	{
	case 1: // 图像矩形裁切
	{
				cout << "请输入文件位置： " << endl;
				string fn;
				cin >> fn;

				fileopen(fn.c_str());
				const char* pszSrcFile = fn.c_str();

				GDALAllRegister();
				GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
				if (pSrcDS == NULL)
				{
					printf("File: %s不能打开！\n", pszSrcFile);
					return 0;
				}
				//输出图像的格式信息  
				printf("Driver : %s/%s\n",
					pSrcDS->GetDriver()->GetDescription(),
					pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

				//输出图像的大小和波段个数  
				printf("Size is %d x %d\n",
					pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize());
				//输出图像的左上角坐标和像素大小
				double adfGeoTransform[6];
				if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
				{
					printf("Origin =(%.6f,%.6f)\n",
						adfGeoTransform[0], adfGeoTransform[3]);
					printf("PixelSize = (%.6f,%.6f)\n",
						adfGeoTransform[1], adfGeoTransform[5]);
				}

				cout << "输入裁切矩形的左上角像素行列号(x, y): ";
				int m_nx = 0;
				int m_ny = 0;
				cin >> m_nx >> m_ny;
				cout << "输入裁切矩形的宽度和高度: ";
				int m_width = 0;
				int m_high = 0;
				cin >> m_width >> m_high;
				iRev = ImageCut(fn.c_str(), "F:\\latest_rectcut.bmp", m_nx, m_ny, m_width, m_high, "BMP", pPro);
				
	}
		break;
	case 2:	// 标准自动裁切
	{
				cout << "请输入文件位置： " << endl;
				string fn1;
				cin >> fn1;
				// 判断是否能打开文件
				fileopen(fn1.c_str());
				const char* pszSrcFile = fn1.c_str();

				GDALAllRegister();
				GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
				if (pSrcDS == NULL)
				{
					printf("File: %s不能打开！\n", pszSrcFile);
					return 0;
				}

				// 输出图像的格式信息  
				printf("Driver : %s/%s\n",
					pSrcDS->GetDriver()->GetDescription(),
					pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

				// 输出图像的大小和波段个数  
				printf("Size is %d x %d x %d\n",
					pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize(),
					pSrcDS->GetRasterCount());
				// 输出图像的左上角坐标
				double adfGeoTransform[6];
				if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
				{
					printf("Origin =(%.6f,%.6f)\n",
						adfGeoTransform[0], adfGeoTransform[3]);
					printf("PixelSize = (%.6f,%.6f)\n",
						adfGeoTransform[1], adfGeoTransform[5]);
				}

				//获得图像的大小
				int OriImageWidth = 0;
				int OriImageHigh = 0;
				OriImageWidth = pSrcDS->GetRasterXSize();
				OriImageHigh = pSrcDS->GetRasterYSize();

				//裁切图像起点
				int iStartX = 0;
				int iStartY = 0;
				//裁切图像大小
				cout << "请输入裁切图像大小： " << endl;
				int iSizeX;
				int iSizeY;

				cin >> iSizeX;
				cin >> iSizeY;

				//初始化输出路径
				string filehead;
				string filetail;
				string sImageNum;
				filehead = "F:\\latest_autocut256-";
				//filehead = "F:\\s3c-stc-dom_cut";
				filetail = ".bmp";
				int nImageNum = 1;
				ostringstream osstemp;
				osstemp << nImageNum;
				sImageNum = osstemp.str();
				string outputfileNa = filehead + sImageNum + filetail;

				//以原始图像宽高循环切割
				for (; iStartY < OriImageHigh; iStartY += iSizeY)
				{
					//判断最后一列切割出的图幅是否超出图像范围
					int YSize = 0;
					if (OriImageHigh - iStartY < iSizeY) // 多余的列块
						YSize = OriImageHigh - iStartY;
					else
						YSize = iSizeY;
					for (; iStartX < OriImageWidth; iStartX += iSizeX)
					{
						//判断最后一行切割出的图幅是否超出图像范围
						int XSize = 0;
						if (OriImageWidth - iStartX < iSizeX) // 多余的行块
							XSize = OriImageWidth - iStartX;
						else
							XSize = iSizeX;
						ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, XSize, YSize, "BMP", pPro);
						++nImageNum;

						//重置输出路径
						ostringstream osstemp;
						osstemp << nImageNum;
						sImageNum = osstemp.str();
						string outputfileNatemp = filehead + sImageNum + filetail;
						outputfileNa = outputfileNatemp;
					}
					iStartX = 0;
				}

				iRev = RE_SUCCESS;
	}
		break;
	case 3: // 图像AOI裁切
	{
				const char* pszAOI = "POLYGON((400 500,900 300,1400 200,2000 1500,700 1200,400 500))";
				char* pszWkt = (char*)pszAOI;
				printf("Before:%p\n", (void*)pszWkt);
				OGRGeometry* oGeom = NULL;
				OGRErr err = OGRGeometryFactory::createFromWkt(&pszWkt, NULL, &oGeom);
				printf("After:%p\n", (void*)pszWkt);
				iRev = ImageCutByAOI("F:\\s3c-stc-dom.tif", "F:\\s3c-stc-dom_aoicut.tif", pszAOI, "GTiff", pPro);
	}
	case 4:	// RasterIO重采样
	{
				//初始化输入路径
				string openfilehead;
				string openfiletail;
				string sImageNum;
				openfilehead = "F:\\latest_autocut256-";
				openfiletail = ".bmp";
				string outfilehead = "F:\\";
				string outfiletail = ".jpg";
				int nImageNum = 1;
				ostringstream osstemp;
				osstemp << nImageNum;
				sImageNum = osstemp.str();
				string inputfileNa = openfilehead + sImageNum + openfiletail;
				string outputfileNa = outfilehead + sImageNum + outfiletail;
				for (int i = 0; i < 256; i++)
				{
					iRev = ImageResample1(inputfileNa.c_str(), outputfileNa.c_str(), 2, 2, "BMP", pPro);
					++nImageNum;
					//重置路径
					ostringstream osstemp;
					osstemp << nImageNum;
					sImageNum = osstemp.str();
					string inputfileNatemp = openfilehead + sImageNum + openfiletail;
					string outputfileNatemp = outfilehead + sImageNum + outfiletail;
					inputfileNa = inputfileNatemp;
					outputfileNa = outputfileNatemp;
				}
	}
		break;
	case 5:	// GDALWarp重采样
	{
				//初始化输入路径
				string openfilehead;
				string openfiletail;
				string sImageNum;
				openfilehead = "F:\\latest_autocut64-";
				openfiletail = ".bmp";
				string outfilehead = "F:\\";
				string outfiletail = ".jpg";
				int nImageNum = 1;
				ostringstream osstemp;
				osstemp << nImageNum;
				sImageNum = osstemp.str();
				string inputfileNa = openfilehead + sImageNum + openfiletail;
				string outputfileNa = outfilehead + sImageNum + outfiletail;
				for (int i = 0; i < 64; i++)
				{
					iRev = ImageResample2(inputfileNa.c_str(), outputfileNa.c_str(), 2, 2, GRA_NearestNeighbour, "BMP", pPro);
					++nImageNum;
					//重置路径
					ostringstream osstemp;
					osstemp << nImageNum;
					sImageNum = osstemp.str();
					string inputfileNatemp = openfilehead + sImageNum + openfiletail;
					string outputfileNatemp = outfilehead + sImageNum + outfiletail;
					inputfileNa = inputfileNatemp;
					outputfileNa = outputfileNatemp;
				}
				//iRev = ImageResample2("F:\\spot5_test.tif", "F:\\spot5_res2.tif", 0.5, 0.5, GRA_NearestNeighbour, "GTiff", pPro);
	}
		break;
	case 6:
	{
			  cout << "请输入文件位置： " << endl;
			  string fn;
			  cin >> fn;

			  fileopen(fn.c_str());
			  const char* pszSrcFile = fn.c_str();

			  GDALAllRegister();
			  GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
			  if (pSrcDS == NULL)
			  {
				  printf("File: %s不能打开！\n", pszSrcFile);
				  return 0;
			  }
			  //输出图像的格式信息  
			  printf("Driver : %s/%s\n",
				  pSrcDS->GetDriver()->GetDescription(),
				  pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

			  //输出图像的大小和波段个数  
			  printf("Size is %d x %d\n",
				  pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize());
			  //输出图像的左上角坐标和像素大小
			  double adfGeoTransform[6];
			  if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
			  {
				  printf("Origin =(%.6f,%.6f)\n",
					  adfGeoTransform[0], adfGeoTransform[3]);
				  printf("PixelSize = (%.6f,%.6f)\n",
					  adfGeoTransform[1], adfGeoTransform[5]);
			  }
			  iRev = ImageResample1(fn.c_str(), "F:\\0.jpg", 2, 2, "BMP", pPro);
	}
		break;
	case 7:
	{
			  cout << "请输入文件位置： " << endl;
			  string fn1;
			  cin >> fn1;
			  // 判断是否能打开文件
			  fileopen(fn1.c_str());
			  const char* pszSrcFile = fn1.c_str();

			  GDALAllRegister();
			  GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
			  if (pSrcDS == NULL)
			  {
				  printf("File: %s不能打开！\n", pszSrcFile);
				  return 0;
			  }

			  // 输出图像的格式信息  
			  printf("Driver : %s/%s\n",
				  pSrcDS->GetDriver()->GetDescription(),
				  pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

			  // 输出图像的大小和波段个数  
			  printf("Size is %d x %d x %d\n",
				  pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize(),
				  pSrcDS->GetRasterCount());
			  // 输出图像的左上角坐标
			  double adfGeoTransform[6];
			  if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
			  {
				  printf("Origin =(%.6f,%.6f)\n",
					  adfGeoTransform[0], adfGeoTransform[3]);
				  printf("PixelSize = (%.6f,%.6f)\n",
					  adfGeoTransform[1], adfGeoTransform[5]);
			  }

			  //获得图像的大小
			  int OriImageWidth = 0;
			  int OriImageHigh = 0;
			  OriImageWidth = pSrcDS->GetRasterXSize();
			  OriImageHigh = pSrcDS->GetRasterYSize();

			  //裁切图像起点
			  int iStartX = 0;
			  int iStartY = 0;

			  //裁切图像大小
			  cout << "请输入裁切图像大小： " << endl;
			  int iSizeX;
			  int iSizeY;

			  cin >> iSizeX;
			  cin >> iSizeY;

			  //初始化输出路径
			  string filehead;
			  string filetail;
			  string sImageNum;
			  filehead = "F:\\latest_autocut256-";
			  //filehead = "F:\\s3c-stc-dom_cut";
			  filetail = ".bmp";
			  int nImageNum = 1;
			  ostringstream osstemp;
			  osstemp << nImageNum;
			  sImageNum = osstemp.str();
			  string outputfileNa = filehead + sImageNum + filetail;

			  string refilehead = "F:\\";
			  string refiletail = ".jpg";
			  int reImgNum = 0;
			  ostringstream osstemp1;
			  osstemp1 << reImgNum;
			  string sImageNum1 = osstemp1.str();
			  string resamplefile = refilehead + sImageNum1 + refiletail;

			  //以原始图像宽高循环切割
			  for (; iStartY < OriImageHigh; iStartY += iSizeY)
			  {
				  //判断最后一列切割出的图幅是否超出图像范围
				  int YSize = 0;
				  if (OriImageHigh - iStartY < iSizeY) // 多余的列块
					  YSize = OriImageHigh - iStartY;
				  else
					  YSize = iSizeY;
				  for (; iStartX < OriImageWidth; iStartX += iSizeX)
				  {
					  //判断最后一行切割出的图幅是否超出图像范围
					  int XSize = 0;
					  if (OriImageWidth - iStartX < iSizeX) // 多余的行块
						  XSize = OriImageWidth - iStartX;
					  else
						  XSize = iSizeX;
					  ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, XSize, YSize, "BMP", pPro);
					  ++nImageNum;

					  ImageResample1(outputfileNa.c_str(), resamplefile.c_str(), 2, 2, "BMP", pPro);
					  ++reImgNum;
					  //重置输出路径
					  ostringstream osstemp;
					  osstemp << nImageNum;
					  sImageNum = osstemp.str();
					  string outputfileNatemp = filehead + sImageNum + filetail;
					  outputfileNa = outputfileNatemp;

					  ostringstream osstemp1;
					  osstemp1 << reImgNum;
					  sImageNum1 = osstemp1.str();
					  string resamplefileNatemp = refilehead + sImageNum1 + refiletail;
					  resamplefile = resamplefileNatemp;
				  }
				  iStartX = 0;
			  }

			  iRev = RE_SUCCESS;
	}
		break;
	case 8:
	{
			  cout << "请输入文件位置： " << endl;
			  string fn1;
			  cin >> fn1;

			  // 判断是否能打开文件
			  fileopen(fn1.c_str());
			  const char* pszSrcFile = fn1.c_str();
			  GDALAllRegister();
			  GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
			  if (pSrcDS == NULL)
			  {
				  printf("File: %s不能打开！\n", pszSrcFile);
				  return 0;
			  }

			  // 获得图像的大小
			  int OriImageWidth = pSrcDS->GetRasterXSize();
			  int OriImageHigh = pSrcDS->GetRasterYSize();

			  // 裁切图像起点
			  int iStartX = 0;
			  int iStartY = 0;

			  // 输入裁切图像大小和列数
			  cout << "请输入裁切图像大小： " << endl;
			  int iSizeX;
			  int iSizeY;
			  cin >> iSizeX;
			  cin >> iSizeY;
			  cout << "请输入裁切列数： " << endl;
			  int iColumn;
			  cin >> iColumn;
			  // 裁剪起点改变
			  iStartX = iSizeX * (iColumn - 1);

			  // 初始化切图输出路径
			  string outImghead;
			  string outImgtail;
			  string sImageNum;
			  outImghead = "F:\\latest_autocut-";
			  //filehead = "F:\\s3c-stc-dom_cut";
			  outImgtail = ".bmp";
			  int nImageNum = 1;
			  // 转化成string
			  ostringstream osImageNumtemp;
			  osImageNumtemp << nImageNum;
			  sImageNum = osImageNumtemp.str();

			  string outputfileNa = outImghead + sImageNum + outImgtail;

			  // 初始化重采样输出路径(路径自己设定)
			  string refilehead = "G:\\v1.3\\tiles\\6\\";
			  // 文件夹编号
			  int refile = iColumn - 1;
			  ostringstream osrefiletemp;
			  osrefiletemp << refile;
			  string reFileNum = osrefiletemp.str();
			  string refiletail = ".jpg";
			  // 按照百度地图的方式命名
			  int reImgNum = (OriImageHigh / iSizeY) - 1; //15
			  ostringstream osreImgNumtemp;
			  osreImgNumtemp << reImgNum;
			  string sImageNum1 = osreImgNumtemp.str();

			  string resamplefile = refilehead + reFileNum + "\\" +sImageNum1 + refiletail;

			  for (; iStartX < OriImageWidth; iStartX += iSizeX)
			  {
				  //重置重采样路径
				  resamplefile = refilehead + reFileNum + "\\" + sImageNum1 + refiletail;
				  //判断最后一行切割出的图幅是否超出图像范围
				  int Xsize = 0;
				  if (OriImageWidth - iStartX < iSizeX) // 多余的行块
				  Xsize = OriImageWidth - iStartX;
				  else
				  Xsize = iSizeX;

				  for (; iStartY < OriImageHigh; iStartY += iSizeY)
				  {
					  //判断最后一列切割出的图幅是否超出图像范围
					  int YSize = 0;
					  if (OriImageHigh - iStartY < iSizeY) // 多余的列块
						  YSize = OriImageHigh - iStartY;
					  else
						  YSize = iSizeY;
					  
					  // 先裁剪再重采样
					  ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, Xsize, YSize, "BMP", pPro);
					  nImageNum = nImageNum + 1;

					  ImageResample1(outputfileNa.c_str(), resamplefile.c_str(), 2, 2, "BMP", pPro);
					  reImgNum = reImgNum - 1;

					  //重置输出路径
					  ostringstream osstemp;
					  osstemp << nImageNum;
					  sImageNum = osstemp.str();
					  string outputfileNatemp = outImghead + sImageNum + outImgtail;
					  outputfileNa = outputfileNatemp;

					  ostringstream osstemp1;
					  osstemp1 << reImgNum;
					  sImageNum1 = osstemp1.str();
					  string resamplefileNatemp = refilehead + reFileNum + "\\" + sImageNum1 + refiletail;
					  resamplefile = resamplefileNatemp;
				  }
				  // 重置Y起点
				  iStartY = 0;
				  // 文件夹+1
				  ++refile;

				  ostringstream osstemp2;
				  osstemp2 << refile;
				  reFileNum = osstemp2.str();

				  reImgNum = (OriImageHigh / iSizeY) - 1;;
				  ostringstream osstemp3;
				  osstemp3 << reImgNum;
				  sImageNum1 = osstemp3.str();
			  }
			  iRev = RE_SUCCESS;
	}
		break;
	default:
		iRev = RE_FAILED;
		break;
	}

	if (iRev == RE_SUCCESS)
		printf("成功 success!\n");
	else
		printf("失败 failed!\n");

	delete pPro;
	::system("pause");
	return 0;
}