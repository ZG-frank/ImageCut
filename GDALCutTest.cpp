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
	cout << "����4 ͼ���ز�����ʹ��RasterIO������" << endl;
	cout << "����5 ͼ���ز�����ʹ��GDALWarp������" << endl;
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
				iRev = ImageCut(fn.c_str(), "F:\\s3c-stc-dom_rectcut.tif", m_nx, m_ny, m_width, m_high, "GTiff", pPro);
				
	}
		break;
	case 2:	// ��׼�Զ�����
	{
				cout << "�������ļ�λ�ã� " << endl;
				string fn1;
				cin >> fn1;
				//�ж��Ƿ��ܴ��ļ�
				fileopen(fn1.c_str());
				const char* pszSrcFile = fn1.c_str();

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
				printf("Size is %d x %d x %d\n",
					pSrcDS->GetRasterXSize(), pSrcDS->GetRasterYSize(),
					pSrcDS->GetRasterCount());
				//���ͼ������Ͻ�����
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

				//X�ϲ���ͼ����Ŀ(����ȡ��)
				int nXnum = 0;
				nXnum = ceil((OriImageWidth / (250 / 0.07)));

				//Y�ϲ���ͼ����Ŀ(����ȡ��)
				int nYnum = 0;
				nYnum = ceil((OriImageHigh / (250 / 0.07)));

				//����ͼ�����
				int iStartX = 0;
				int iStartY = 0;

				//����ͼ���С
				int iSizeX = (250 / 0.07);
				int iSizeY = (250 / 0.07);

				//��ʼ�����·��
				string filehead;
				string filetail;
				string sImageNum;
				filehead = "F:\\s3c-stc-dom_cut";
				filetail = ".tif";
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
						ImageCut(fn1.c_str(), outputfileNa.c_str(), iStartX, iStartY, XSize, YSize, "GTiff", pPro);
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
				iRev = ImageResample1("F:\\spot5_test.tif", "F:\\spot5_res1.tif", 2, 2, "GTiff", pPro);
	}
		break;
	case 5:	// GDALWarp�ز���
	{
				iRev = ImageResample2("F:\\spot5_test.tif", "F:\\spot5_res2.tif", 0.5, 0.5, GRA_NearestNeighbour, "GTiff", pPro);
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