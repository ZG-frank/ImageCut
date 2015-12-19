#include "GDALAlgorithm.h"


int ImageCut(const char* pszSrcFile, const char* pszDstFile, int iStartX, int iStartY, int iSizeX, int iSizeY, const char* pszFormat, CProcessBase* pProcess)
{
	// 前面需要对输入输出的文件路径进行判断，这里不进行判断直接处理
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("开始矩形裁切...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	if (pSrcDS == NULL)
	{
		printf("File: %s不能打开！\n", pszSrcFile);
		return 0;
	}
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();

	//获得图像的大小
	int OriImageWidth = pSrcDS->GetRasterXSize();
	int OriImageHigh = pSrcDS->GetRasterYSize();

	// 确定裁切图像的宽高
	int iDstWidth = 250 / 0.07;
	int iDstHeight = 250 / 0.07;

	//获取原图的仿射变换信息
	double adfGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(adfGeoTransform);

	// 计算裁切后的图像的左上角坐标	
	adfGeoTransform[0] = adfGeoTransform[0] + iStartX*adfGeoTransform[1] + iStartY*adfGeoTransform[2];
	adfGeoTransform[3] = adfGeoTransform[3] + iStartX*adfGeoTransform[4] + iStartY*adfGeoTransform[5];

	// 创建输出文件并设置空间参考和坐标信息
	GDALDriver *poDriver = (GDALDriver *)GDALGetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(adfGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());

	int *pBandMap = new int[iBandCount];
	for (int i = 0; i < iBandCount; i++)
		pBandMap[i] = i + 1;

	// 下面这里需要根据数据类型来判断，申请不同类型的缓存来进行处理，这里仅仅以8bit图像进行演示
	if (eDT == GDT_Byte)	// 如果是8bit图像
	{
		// 申请所有数据所需要的缓存，如果图像太大应该用分块处理
		DT_8U *pDataBuff = new DT_8U[iDstWidth*iDstHeight*iBandCount];

		
		pSrcDS->RasterIO(GF_Read, iStartX, iStartY, iSizeX, iSizeY, pDataBuff, iSizeX, iSizeY, eDT, iBandCount, pBandMap, 0, 0, 0);
		pDstDS->RasterIO(GF_Write, 0, 0, iSizeX, iSizeY, pDataBuff, iSizeX, iSizeY, eDT, iBandCount, pBandMap, 0, 0, 0);
		
		RELEASE(pDataBuff);
	}
	else
	{
		// 其他类型的图像，与8bit类似，就是申请的缓存类型不同而已
	}

	RELEASE(pBandMap);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("裁切完成！");
	//输出TFW文件

	GDALWriteWorldFile(pszDstFile, "tfw", adfGeoTransform);

	return RE_SUCCESS;
}

int ImageCutByAOI(const char* pszSrcFile, const char* pszDstFile, const char* pszAOIWKT, const char* pszFormat, CProcessBase* pProcess)
{
	// 前面需要对输入输出的文件路径进行判断，这里不进行判断直接处理
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("开始AOI裁切...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	// 打开原始图像并计算图像信息
	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();
	int iSrcWidth = pSrcDS->GetRasterXSize();
	int iSrcHeight = pSrcDS->GetRasterYSize();

	double pSrcGeoTransform[6] = { 0 };
	double pDstGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(pSrcGeoTransform);			//图像的仿射变换信息
	memcpy(pDstGeoTransform, pSrcGeoTransform, sizeof(double)* 6);

	// 将传入的AOI的WKT处理为一个OGRGeometry类型，用于后续处理
	char *pszWKT = (char*)pszAOIWKT;
	OGRGeometry* pAOIGeometry = OGRGeometryFactory::createGeometry(wkbPolygon);
	pAOIGeometry->importFromWkt(&pszWKT);

	OGREnvelope eRect;
	pAOIGeometry->getEnvelope(&eRect);

	// 设置输出图像的左上角坐标
	GDALApplyGeoTransform(pSrcGeoTransform, eRect.MinX, eRect.MinY, (&pDstGeoTransform[0]), &(pDstGeoTransform[3]));

	// 根据裁切范围确定裁切后的图像宽高
	int iDstWidth = static_cast<int>(eRect.MaxX - eRect.MinX);
	int iDstHeight = static_cast<int>(eRect.MaxY - eRect.MinY);

	// 创建输出图像
	GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(pDstGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());
	

	// 构造坐标转换关系
	void *hTransformArg = GDALCreateGenImgProjTransformer2((GDALDatasetH)pSrcDS, (GDALDatasetH)pDstDS, NULL);
	GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

	// 构造GDALWarp的变换选项
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

	// 设置裁切AOI，AOI中的坐标必须是图像的行列号坐标，否则不能进行裁切
	psWO->hCutline = (void*)pAOIGeometry;
	// 设置上面的hCutline的值和使用下面的CUTLINE配置项的效果一样，两个选择一个即可
	psWO->papszWarpOptions = CSLSetNameValue(psWO->papszWarpOptions, "CUTLINE", pszAOIWKT);

	// 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// 执行处理
	oWO.ChunkAndWarpImage(0, 0, iDstWidth, iDstHeight);

	// 释放资源和关闭文件
	GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("裁切完成！");

	return RE_SUCCESS;
}

int ImageResample1(const char* pszSrcFile, const char* pszDstFile, double dResX, double dResY,
	const char* pszFormat, CProcessBase* pProcess)
{
	// 前面需要对输入输出的文件路径进行判断，这里不进行判断直接处理
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("开始重采样...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();
	int iSrcWidth = pSrcDS->GetRasterXSize();
	int iSrcHeight = pSrcDS->GetRasterYSize();

	// 根据采样比例计算重采样后的图像宽高
	int iDstWidth = static_cast<int>(iSrcWidth  * dResX + 0.5);
	int iDstHeight = static_cast<int>(iSrcHeight * dResY + 0.5);

	double adfGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(adfGeoTransform);

	// 计算采样后的图像的分辨率	
	adfGeoTransform[1] = adfGeoTransform[1] / dResX;
	adfGeoTransform[5] = adfGeoTransform[5] / dResY;

	// 创建输出文件并设置空间参考和坐标信息
	GDALDriver *poDriver = (GDALDriver *)GDALGetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(adfGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());

	int *pBandMap = new int[iBandCount];
	for (int i = 0; i < iBandCount; i++)
		pBandMap[i] = i + 1;

	// 下面这里需要根据数据类型来判断，申请不同类型的缓存来进行处理，这里仅仅以8bit图像进行演示
	if (eDT == GDT_Byte)	// 如果是8bit图像
	{
		// 申请所有数据所需要的缓存，如果图像太大应该用分块处理
		DT_8U *pDataBuff = new DT_8U[iDstWidth*iDstHeight*iBandCount];

		pSrcDS->RasterIO(GF_Read, 0, 0, iSrcWidth, iSrcHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);
		pDstDS->RasterIO(GF_Write, 0, 0, iDstWidth, iDstHeight, pDataBuff, iDstWidth, iDstHeight, eDT, iBandCount, pBandMap, 0, 0, 0);

		RELEASE(pDataBuff);
	}
	else
	{
		// 其他类型的图像，与8bit类似，就是申请的缓存类型不同而已
	}

	RELEASE(pBandMap);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("重采样完成！");
	

	return RE_SUCCESS;
}

int ImageResample2(const char* pszSrcFile, const char* pszDstFile, double dResX, double dResY,
	GDALResampleAlg eResampleMethod, const char* pszFormat, CProcessBase* pProcess)
{
	// 前面需要对输入输出的文件路径进行判断，这里不进行判断直接处理
	if (pProcess != NULL)
	{
		pProcess->ReSetProcess();
		pProcess->SetMessage("开始重采样...");
		pProcess->SetPosition(1.0);
	}

	GDALAllRegister();

	GDALDataset *pSrcDS = (GDALDataset*)GDALOpen(pszSrcFile, GA_ReadOnly);
	GDALDataType eDT = pSrcDS->GetRasterBand(1)->GetRasterDataType();

	int iBandCount = pSrcDS->GetRasterCount();
	int iSrcWidth = pSrcDS->GetRasterXSize();
	int iSrcHeight = pSrcDS->GetRasterYSize();

	// 根据采样比例计算重采样后的图像宽高
	int iDstWidth = static_cast<int>(iSrcWidth  * dResX + 0.5);
	int iDstHeight = static_cast<int>(iSrcHeight * dResY + 0.5);

	double adfGeoTransform[6] = { 0 };
	pSrcDS->GetGeoTransform(adfGeoTransform);

	// 计算采样后的图像的分辨率	
	adfGeoTransform[1] = adfGeoTransform[1] / dResX;
	adfGeoTransform[5] = adfGeoTransform[5] / dResY;

	// 创建输出文件并设置空间参考和坐标信息
	GDALDriver *poDriver = (GDALDriver *)GDALGetDriverByName(pszFormat);
	GDALDataset *pDstDS = poDriver->Create(pszDstFile, iDstWidth, iDstHeight, iBandCount, eDT, NULL);
	pDstDS->SetGeoTransform(adfGeoTransform);
	pDstDS->SetProjection(pSrcDS->GetProjectionRef());

	// 构造坐标转换关系
	void *hTransformArg = NULL;
	hTransformArg = GDALCreateGenImgProjTransformer2((GDALDatasetH)pSrcDS, (GDALDatasetH)pDstDS, NULL);
	GDALTransformerFunc pfnTransformer = GDALGenImgProjTransform;

	// 构造GDALWarp的变换选项
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

	// 创建GDALWarp执行对象，并使用GDALWarpOptions来进行初始化
	GDALWarpOperation oWO;
	oWO.Initialize(psWO);

	// 执行处理
	oWO.ChunkAndWarpImage(0, 0, iDstWidth, iDstHeight);

	// 释放资源和关闭文件
	GDALDestroyGenImgProjTransformer(psWO->pTransformerArg);
	GDALDestroyWarpOptions(psWO);

	GDALClose((GDALDatasetH)pSrcDS);
	GDALClose((GDALDatasetH)pDstDS);

	if (pProcess != NULL)
		pProcess->SetMessage("重采样完成！");

	return RE_SUCCESS;
}