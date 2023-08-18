
	int	ctrlpt=0, readNum=0;
	int nSpcPoints=0, nSpcLines=0;
	FILE *fppts = fopen(m_P3D_Data_File,"r");

	for(i=0;i<5;i++)
	{
		fgets(tmpchar,512,fppts);	//读入注释行
	}

	fgets(tmpchar,512,fppts);	//读入总点数行
	readNum = sscanf(tmpchar,"%d %d\n",&nSpcPoints,&nSpcLines);

	for(i=0;i<nSpcPoints;i++)
	{
		if(feof(fppts))	break;

		ctrlpt = 0;readNum=0;
		strcpy(tmpchar,"");
		fgets(tmpchar,512,fppts);	//读入空间点行
		readNum = sscanf(tmpchar,"%s %lf %lf %lf %d\n",&Point3D.m_Name,&Point3D.x,&Point3D.y,&Point3D.z,&attrib);
		
		fgets(tmpchar,512,fppts);	//读入空间点对应像点数行
		sscanf(tmpchar,"%d\n",&ImgNum);

		for(j=0;j<ImgNum;j++)
		{
				Point2D.m_Valid = 1;
				Point2D.m_spacePointID = Point3D.m_spacePointID;

				fgets(tmpchar,512,fppts);	//读入像点行
				readNum = sscanf(tmpchar,"%d %lf %lf %d\n",&Point2D.m_imageID,&Point2D.x,&Point2D.y,&Point2D.m_GroupNo);
		}
	}
	fclose(fppts);