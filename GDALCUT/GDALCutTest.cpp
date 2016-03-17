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
	cout << "����1 ���βü�"<<endl;
	cout << "����2 ��׼�Զ��ü�" << endl;
	cout << "����3 AOI�ü�" << endl;
	cout << "����4 �����ز�����ʹ��RasterIO������" << endl;
	cout << "����5 ͼ���ز�����ʹ��GDALWarp������" << endl;
	cout << "����6 �����ز�����ʹ��RasterIO������" << endl;
	cout << "����7 �����и���ز������������ң�" << endl;
	cout << "����8 ���������и��ز������Զ�����" << endl;
	int iCode;
	cout << "������룺 ";
	cin >> iCode;
	int iRev = RE_SUCCESS;  //0
	CConsoleProcess *pPro = new CConsoleProcess();

	switch (iCode)
	{
	case 1: // ͼ����β���
	{
				cout << "�������ļ�λ�ã� " << endl;
				string fn;
				cin >> fn;

				fileopen(fn.c_str());
				const char* pszSrcFile = fn.c_str();

				GDALAllRegister();
				GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
				if (pSrcDS == NULL)
				{
					printf("File: %s���ܴ򿪣�\n", pszSrcFile);
					return 0;
				}
				//���ͼ��ĸ�ʽ��Ϣ  
				printf("Driver : %s/%s\n",
					pSrcDS->GetDriver()->GetDescription(),
					pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

				//���ͼ��Ĵ�С�Ͳ��θ���  
				printf("Size is %d x %d\n",
					pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize());
				//���ͼ������Ͻ���������ش�С
				double adfGeoTransform[6];
				if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
				{
					printf("Origin =(%.6f,%.6f)\n",
						adfGeoTransform[0], adfGeoTransform[3]);
					printf("PixelSize = (%.6f,%.6f)\n",
						adfGeoTransform[1], adfGeoTransform[5]);
				}

				cout << "������о��ε����Ͻ��������к�(x, y): ";
				int m_nx = 0;
				int m_ny = 0;
				cin >> m_nx >> m_ny;
				cout << "������о��εĿ�Ⱥ͸߶�: ";
				int m_width = 0;
				int m_high = 0;
				cin >> m_width >> m_high;
				iRev = ImageCut(fn.c_str(), "F:\\latest_rectcut.bmp", m_nx, m_ny, m_width, m_high, "BMP", pPro);
				
	}
		break;
	case 2:	// ��׼�Զ�����
	{
				cout << "�������ļ�λ�ã� " << endl;
				string fn1;
				cin >> fn1;
				// �ж��Ƿ��ܴ��ļ�
				fileopen(fn1.c_str());
				const char* pszSrcFile = fn1.c_str();

				GDALAllRegister();
				GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
				if (pSrcDS == NULL)
				{
					printf("File: %s���ܴ򿪣�\n", pszSrcFile);
					return 0;
				}

				// ���ͼ��ĸ�ʽ��Ϣ  
				printf("Driver : %s/%s\n",
					pSrcDS->GetDriver()->GetDescription(),
					pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

				// ���ͼ��Ĵ�С�Ͳ��θ���  
				printf("Size is %d x %d x %d\n",
					pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize(),
					pSrcDS->GetRasterCount());
				// ���ͼ������Ͻ�����
				double adfGeoTransform[6];
				if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
				{
					printf("Origin =(%.6f,%.6f)\n",
						adfGeoTransform[0], adfGeoTransform[3]);
					printf("PixelSize = (%.6f,%.6f)\n",
						adfGeoTransform[1], adfGeoTransform[5]);
				}

				//���ͼ��Ĵ�С
				int OriImageWidth = 0;
				int OriImageHigh = 0;
				OriImageWidth = pSrcDS->GetRasterXSize();
				OriImageHigh = pSrcDS->GetRasterYSize();

				//����ͼ�����
				int iStartX = 0;
				int iStartY = 0;
				//����ͼ���С
				cout << "���������ͼ���С�� " << endl;
				int iSizeX;
				int iSizeY;

				cin >> iSizeX;
				cin >> iSizeY;

				//��ʼ�����·��
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

				//��ԭʼͼ����ѭ���и�
				for (; iStartY < OriImageHigh; iStartY += iSizeY)
				{
					//�ж����һ���и����ͼ���Ƿ񳬳�ͼ��Χ
					int YSize = 0;
					if (OriImageHigh - iStartY < iSizeY) // ������п�
						YSize = OriImageHigh - iStartY;
					else
						YSize = iSizeY;
					for (; iStartX < OriImageWidth; iStartX += iSizeX)
					{
						//�ж����һ���и����ͼ���Ƿ񳬳�ͼ��Χ
						int XSize = 0;
						if (OriImageWidth - iStartX < iSizeX) // ������п�
							XSize = OriImageWidth - iStartX;
						else
							XSize = iSizeX;
						ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, XSize, YSize, "BMP", pPro);
						++nImageNum;

						//�������·��
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
	case 3: // ͼ��AOI����
	{
				const char* pszAOI = "POLYGON((400 500,900 300,1400 200,2000 1500,700 1200,400 500))";
				char* pszWkt = (char*)pszAOI;
				printf("Before:%p\n", (void*)pszWkt);
				OGRGeometry* oGeom = NULL;
				OGRErr err = OGRGeometryFactory::createFromWkt(&pszWkt, NULL, &oGeom);
				printf("After:%p\n", (void*)pszWkt);
				iRev = ImageCutByAOI("F:\\s3c-stc-dom.tif", "F:\\s3c-stc-dom_aoicut.tif", pszAOI, "GTiff", pPro);
	}
	case 4:	// RasterIO�ز���
	{
				//��ʼ������·��
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
					//����·��
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
	case 5:	// GDALWarp�ز���
	{
				//��ʼ������·��
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
					//����·��
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
			  cout << "�������ļ�λ�ã� " << endl;
			  string fn;
			  cin >> fn;

			  fileopen(fn.c_str());
			  const char* pszSrcFile = fn.c_str();

			  GDALAllRegister();
			  GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
			  if (pSrcDS == NULL)
			  {
				  printf("File: %s���ܴ򿪣�\n", pszSrcFile);
				  return 0;
			  }
			  //���ͼ��ĸ�ʽ��Ϣ  
			  printf("Driver : %s/%s\n",
				  pSrcDS->GetDriver()->GetDescription(),
				  pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

			  //���ͼ��Ĵ�С�Ͳ��θ���  
			  printf("Size is %d x %d\n",
				  pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize());
			  //���ͼ������Ͻ���������ش�С
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
			  cout << "�������ļ�λ�ã� " << endl;
			  string fn1;
			  cin >> fn1;
			  // �ж��Ƿ��ܴ��ļ�
			  fileopen(fn1.c_str());
			  const char* pszSrcFile = fn1.c_str();

			  GDALAllRegister();
			  GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
			  if (pSrcDS == NULL)
			  {
				  printf("File: %s���ܴ򿪣�\n", pszSrcFile);
				  return 0;
			  }

			  // ���ͼ��ĸ�ʽ��Ϣ  
			  printf("Driver : %s/%s\n",
				  pSrcDS->GetDriver()->GetDescription(),
				  pSrcDS->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME));

			  // ���ͼ��Ĵ�С�Ͳ��θ���  
			  printf("Size is %d x %d x %d\n",
				  pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize(),
				  pSrcDS->GetRasterCount());
			  // ���ͼ������Ͻ�����
			  double adfGeoTransform[6];
			  if (pSrcDS->GetGeoTransform(adfGeoTransform) == CE_None)
			  {
				  printf("Origin =(%.6f,%.6f)\n",
					  adfGeoTransform[0], adfGeoTransform[3]);
				  printf("PixelSize = (%.6f,%.6f)\n",
					  adfGeoTransform[1], adfGeoTransform[5]);
			  }

			  //���ͼ��Ĵ�С
			  int OriImageWidth = 0;
			  int OriImageHigh = 0;
			  OriImageWidth = pSrcDS->GetRasterXSize();
			  OriImageHigh = pSrcDS->GetRasterYSize();

			  //����ͼ�����
			  int iStartX = 0;
			  int iStartY = 0;

			  //����ͼ���С
			  cout << "���������ͼ���С�� " << endl;
			  int iSizeX;
			  int iSizeY;

			  cin >> iSizeX;
			  cin >> iSizeY;

			  //��ʼ�����·��
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

			  //��ԭʼͼ����ѭ���и�
			  for (; iStartY < OriImageHigh; iStartY += iSizeY)
			  {
				  //�ж����һ���и����ͼ���Ƿ񳬳�ͼ��Χ
				  int YSize = 0;
				  if (OriImageHigh - iStartY < iSizeY) // ������п�
					  YSize = OriImageHigh - iStartY;
				  else
					  YSize = iSizeY;
				  for (; iStartX < OriImageWidth; iStartX += iSizeX)
				  {
					  //�ж����һ���и����ͼ���Ƿ񳬳�ͼ��Χ
					  int XSize = 0;
					  if (OriImageWidth - iStartX < iSizeX) // ������п�
						  XSize = OriImageWidth - iStartX;
					  else
						  XSize = iSizeX;
					  ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, XSize, YSize, "BMP", pPro);
					  ++nImageNum;

					  ImageResample1(outputfileNa.c_str(), resamplefile.c_str(), 2, 2, "BMP", pPro);
					  ++reImgNum;
					  //�������·��
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
			  cout << "�������ļ�λ�ã� " << endl;
			  string fn1;
			  cin >> fn1;

			  // �ж��Ƿ��ܴ��ļ�
			  fileopen(fn1.c_str());
			  const char* pszSrcFile = fn1.c_str();
			  GDALAllRegister();
			  GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
			  if (pSrcDS == NULL)
			  {
				  printf("File: %s���ܴ򿪣�\n", pszSrcFile);
				  return 0;
			  }

			  // ���ͼ��Ĵ�С
			  int OriImageWidth = pSrcDS->GetRasterXSize();
			  int OriImageHigh = pSrcDS->GetRasterYSize();

			  // ����ͼ�����
			  int iStartX = 0;
			  int iStartY = 0;

			  // �������ͼ���С������
			  cout << "���������ͼ���С�� " << endl;
			  int iSizeX;
			  int iSizeY;
			  cin >> iSizeX;
			  cin >> iSizeY;
			  cout << "��������������� " << endl;
			  int iColumn;
			  cin >> iColumn;
			  // �ü����ı�
			  iStartX = iSizeX * (iColumn - 1);

			  // ��ʼ����ͼ���·��
			  string outImghead;
			  string outImgtail;
			  string sImageNum;
			  outImghead = "F:\\latest_autocut-";
			  //filehead = "F:\\s3c-stc-dom_cut";
			  outImgtail = ".bmp";
			  int nImageNum = 1;
			  // ת����string
			  ostringstream osImageNumtemp;
			  osImageNumtemp << nImageNum;
			  sImageNum = osImageNumtemp.str();

			  string outputfileNa = outImghead + sImageNum + outImgtail;

			  // ��ʼ���ز������·��(·���Լ��趨)
			  string refilehead = "G:\\v1.3\\tiles\\6\\";
			  // �ļ��б��
			  int refile = iColumn - 1;
			  ostringstream osrefiletemp;
			  osrefiletemp << refile;
			  string reFileNum = osrefiletemp.str();
			  string refiletail = ".jpg";
			  // ���հٶȵ�ͼ�ķ�ʽ����
			  int reImgNum = (OriImageHigh / iSizeY) - 1; //15
			  ostringstream osreImgNumtemp;
			  osreImgNumtemp << reImgNum;
			  string sImageNum1 = osreImgNumtemp.str();

			  string resamplefile = refilehead + reFileNum + "\\" +sImageNum1 + refiletail;

			  for (; iStartX < OriImageWidth; iStartX += iSizeX)
			  {
				  //�����ز���·��
				  resamplefile = refilehead + reFileNum + "\\" + sImageNum1 + refiletail;
				  //�ж����һ���и����ͼ���Ƿ񳬳�ͼ��Χ
				  int Xsize = 0;
				  if (OriImageWidth - iStartX < iSizeX) // ������п�
				  Xsize = OriImageWidth - iStartX;
				  else
				  Xsize = iSizeX;

				  for (; iStartY < OriImageHigh; iStartY += iSizeY)
				  {
					  //�ж����һ���и����ͼ���Ƿ񳬳�ͼ��Χ
					  int YSize = 0;
					  if (OriImageHigh - iStartY < iSizeY) // ������п�
						  YSize = OriImageHigh - iStartY;
					  else
						  YSize = iSizeY;
					  
					  // �Ȳü����ز���
					  ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, Xsize, YSize, "BMP", pPro);
					  nImageNum = nImageNum + 1;

					  ImageResample1(outputfileNa.c_str(), resamplefile.c_str(), 2, 2, "BMP", pPro);
					  reImgNum = reImgNum - 1;

					  //�������·��
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
				  // ����Y���
				  iStartY = 0;
				  // �ļ���+1
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
		printf("�ɹ� success!\n");
	else
		printf("ʧ�� failed!\n");

	delete pPro;
	::system("pause");
	return 0;
}