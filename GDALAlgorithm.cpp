#include "GDALAlgorithm.h"


int ImageCut(const char* pszSrcFile, const char* pszDstFile, int iStartX, int iStartY, int iSizeX, int iSizeY, const char* pszFormat, CProcessBase* pProcess)
{
	// ǰ����Ҫ������������ļ�·�������жϣ����ﲻ�����ж�ֱ�Ӵ���
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("��ʼ���β���...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	if (pSrcDS == NULL)
	{
		printf("File: %s���ܴ򿪣�\n", pszSrcFile);
		return 0;
	}
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();

	//���ͼ��Ĵ�С
	int OriImageWidth = pSrcDS->GetRasterXSize();
	int OriImageHigh = pSrcDS->GetRasterYSize();

	// ȷ������ͼ��Ŀ��
	int iDstWidth = 250 / 0.07;
	int iDstHeight = 250 / 0.07;

	//��ȡԭͼ�ķ���任��Ϣ
	double adfGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(adfGeoTransform);

	// ������к��ͼ������Ͻ�����	
	adfGeoTransform[0] = adfGeoTransform[0] + iStartX*adfGeoTransform[1] + iStartY*adfGeoTransform[2];
	adfGeoTransform[3] = adfGeoTransform[3] + iStartX*adfGeoTransform[4] + iStartY*adfGeoTransform[5];

	// ��������ļ������ÿռ�ο���������Ϣ
	GDALDriver *poDriver = (GDALDriver *)GDALGetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(adfGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());

	int *pBandMap = new int[iBandCount];
	for (int i = 0; i < iBandCount; i++)
		pBandMap[i] = i + 1;

	// ����������Ҫ���������������жϣ����벻ͬ���͵Ļ��������д������������8bitͼ�������ʾ
	if (eDT == GDT_Byte)	// �����8bitͼ��
	{
		// ����������������Ҫ�Ļ��棬���ͼ��̫��Ӧ���÷ֿ鴦��
		DT_8U *pDataBuff = new DT_8U[iDstWidth*iDstHeight*iBandCount];

		
		pSrcDS->RasterIO(GF_Read, iStartX, iStartY, iSizeX, iSizeY, pDataBuff, iSizeX, iSizeY, eDT, iBandCount, pBandMap, 0, 0, 0);
		pDstDS->RasterIO(GF_Write, 0, 0, iSizeX, iSizeY, pDataBuff, iSizeX, iSizeY, eDT, iBandCount, pBandMap, 0, 0, 0);
		
		RELEASE(pDataBuff);
	}
	else
	{
		// �������͵�ͼ����8bit���ƣ���������Ļ������Ͳ�ͬ����
	}

	RELEASE(pBandMap);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("������ɣ�");
	//���TFW�ļ�

	GDALWriteWorldFile(pszDstFile, "tfw", adfGeoTransform);

	return RE_SUCCESS;
}

int ImageCutByAOI(const char* pszSrcFile, const char* pszDstFile, const char* pszAOIWKT, const char* pszFormat, CProcessBase* pProcess)
{
	// ǰ����Ҫ������������ļ�·�������жϣ����ﲻ�����ж�ֱ�Ӵ���
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("��ʼAOI����...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	// ��ԭʼͼ�񲢼���ͼ����Ϣ
	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();
	int iSrcWidth = pSrcDS->GetRasterXSize();
	int iSrcHeight = pSrcDS->GetRasterYSize();

	double pSrcGeoTransform[6] = { 0 };
	double pDstGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(pSrcGeoTransform);			//ͼ��ķ���任��Ϣ
	memcpy(pDstGeoTransform, pSrcGeoTransform, sizeof(double)* 6);

	// �������AOI��WKT����Ϊһ��OGRGeometry���ͣ����ں�������
	char *pszWKT = (char*)pszAOIWKT;
	OGRGeometry* pAOIGeometry = OGRGeometryFactory::createGeometry(wkbPolygon);
	pAOIGeometry->importFromWkt(&pszWKT);

	OGREnvelope eRect;
	pAOIGeometry->getEnvelope(&eRect);

	// �������ͼ������Ͻ�����
	GDALApplyGeoTransform(pSrcGeoTransform, eRect.MinX, eRect.MinY, (&pDstGeoTransform[0]), &(pDstGeoTransform[3]));

	// ���ݲ��з�Χȷ�����к��ͼ����
	int iDstWidth = static_cast<int>(eRect.MaxX - eRect.MinX);
	int iDstHeight = static_cast<int>(eRect.MaxY - eRect.MinY);

	// �������ͼ��
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(pDstGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());
	

	// ��������ת����ϵ
	void *hTransformArg = GDALCreateGenImgProjTransformer2((GDALDatasetH)pSrcDS, (GDALDatasetH)pDstDS, NULL);
	GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

	// ����GDALWarp�ı任ѡ��
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = eDT;
	psWO->eResampleAlg = GRA_NearestNeighbour;

	psWO->hSrcDS = (GDALDatasetH)pSrcDS;
	psWO->hDstDS = (GDALDatasetH)pDstDS;

	psWO->pfnTransformer = pfnTransformer;
	psWO->pTransformerArg = hTransformArg;

	
	psWO->pProgressArg = pProcess;

	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	for (int i = 0; i < iBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	// ���ò���AOI��AOI�е����������ͼ������к����꣬�����ܽ��в���
	psWO->hCutline = (void*)pAOIGeometry;
	// ���������hCutline��ֵ��ʹ�������CUTLINE�������Ч��һ��������ѡ��һ������
	psWO->papszWarpOptions = CSLSetNameValue(psWO->papszWarpOptions, "CUTLINE", pszAOIWKT);

	// ����GDALWarpִ�ж��󣬲�ʹ��GDALWarpOptions�����г�ʼ��
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// ִ�д���
	oWO.ChunkAndWarpImage(0, 0, iDstWidth, iDstHeight);

	// �ͷ���Դ�͹ر��ļ�
	GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("������ɣ�");

	return RE_SUCCESS;
}

int ImageResample1(const char* pszSrcFile, const char* pszDstFile, double dResX, double dResY,
	const char* pszFormat, CProcessBase* pProcess)
{
	// ǰ����Ҫ������������ļ�·�������жϣ����ﲻ�����ж�ֱ�Ӵ���
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("��ʼ�ز���...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();
	int iSrcWidth = pSrcDS->GetRasterXSize();
	int iSrcHeight = pSrcDS->GetRasterYSize();

	// ���ݲ������������ز������ͼ����
	int iDstWidth = static_cast<int>(iSrcWidth  * dResX + 0.5);
	int iDstHeight = static_cast<int>(iSrcHeight * dResY + 0.5);

	double adfGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(adfGeoTransform);

	// ����������ͼ��ķֱ���	
	adfGeoTransform[1] = adfGeoTransform[1] / dResX;
	adfGeoTransform[5] = adfGeoTransform[5] / dResY;

	// ��������ļ������ÿռ�ο���������Ϣ
	GDALDriver *poDriver = (GDALDriver *)GDALGetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(adfGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());

	int *pBandMap = new int[iBandCount];
	for (int i = 0; i < iBandCount; i++)
		pBandMap[i] = i + 1;

	// ����������Ҫ���������������жϣ����벻ͬ���͵Ļ��������д������������8bitͼ�������ʾ
	if (eDT == GDT_Byte)	// �����8bitͼ��
	{
		// ����������������Ҫ�Ļ��棬���ͼ��̫��Ӧ���÷ֿ鴦��
		DT_8U *pDataBuff = new DT_8U[iDstWidth*iDstHeight*iBandCount];

		pSrcDS->RasterIO(GF_Read, 0, 0, iSrcWidth, iSrcHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		pDstDS->RasterIO(GF_Write, 0, 0, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);

		RELEASE(pDataBuff);
	}
	else
	{
		// �������͵�ͼ����8bit���ƣ���������Ļ������Ͳ�ͬ����
	}

	RELEASE(pBandMap);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("�ز�����ɣ�");
	

	return RE_SUCCESS;
}

int ImageResample2(const char* pszSrcFile, const char* pszDstFile, double dResX, double dResY,
	GDALResampleAlg eResampleMethod, const char* pszFormat, CProcessBase* pProcess)
{
	// ǰ����Ҫ������������ļ�·�������жϣ����ﲻ�����ж�ֱ�Ӵ���
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("��ʼ�ز���...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();
	int iSrcWidth = pSrcDS->GetRasterXSize();
	int iSrcHeight = pSrcDS->GetRasterYSize();

	// ���ݲ������������ز������ͼ����
	int iDstWidth = static_cast<int>(iSrcWidth  * dResX + 0.5);
	int iDstHeight = static_cast<int>(iSrcHeight * dResY + 0.5);

	double adfGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(adfGeoTransform);

	// ����������ͼ��ķֱ���	
	adfGeoTransform[1] = adfGeoTransform[1] / dResX;
	adfGeoTransform[5] = adfGeoTransform[5] / dResY;

	// ��������ļ������ÿռ�ο���������Ϣ
	GDALDriver *poDriver = (GDALDriver *)GDALGetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(adfGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());

	// ��������ת����ϵ
	void *hTransformArg = NULL;
	hTransformArg = GDALCreateGenImgProjTransformer2((GDALDatasetH)pSrcDS, (GDALDatasetH)pDstDS, NULL);
	GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

	// ����GDALWarp�ı任ѡ��
	GDALWarpOptions *psWO = GDALCreateWarpOptions();

	psWO->papszWarpOptions = CSLDuplicate(NULL);
	psWO->eWorkingDataType = eDT;
	psWO->eResampleAlg = eResampleMethod;

	psWO->hSrcDS = (GDALDatasetH)pSrcDS;
	psWO->hDstDS = (GDALDatasetH)pDstDS;

	psWO->pfnTransformer = pfnTransformer;
	psWO->pTransformerArg = hTransformArg;

	psWO->pProgressArg = pProcess;

	psWO->nBandCount = iBandCount;
	psWO->panSrcBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	psWO->panDstBands = (int *)CPLMalloc(psWO->nBandCount*sizeof(int));
	for (int i = 0; i < iBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	// ����GDALWarpִ�ж��󣬲�ʹ��GDALWarpOptions�����г�ʼ��
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// ִ�д���
	oWO.ChunkAndWarpImage(0, 0, iDstWidth, iDstHeight);

	// �ͷ���Դ�͹ر��ļ�
	GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("�ز�����ɣ�");

	return RE_SUCCESS;
}