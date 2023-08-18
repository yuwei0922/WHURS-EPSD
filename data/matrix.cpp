void Zero(double *p, int n)
{
	if (n == NULL)
		return;

	while(n--) *p++=0;
}

///////////////////////////////////////////////
//��AΪm x l�׾���BΪl x n�׾���CΪm x n�׾��󣬼��� C=A x B���ӳ���Ϊ��
BOOL MatrixMulti(double * A, double * B, double * C, int M, int N, int L)
{
	int i,j,k;
	if ( A==NULL||B==NULL||C==NULL) return FALSE ;
	Zero(C,M*N);

	for (i=0;i<M;i++)
	{
		for (j=0;j<N;j++)
		{
//			for (k=0;k<L;k++) C[i*N+j] += A[i*L+k]*B[k*N+j];
			for (k=0;k<L;k++) *(C+i*N+j)+= *(A+i*L+k)* *(B+k*N+j);	//��Ч������
		}
	}
	return TRUE;
}

//�Գ������������� aΪn*n�׶Գ���������nΪ�������
int MatrixInversion(double *a, int n)
{
    int i, j, k, m;
    double     w, g, *b;
    
    b = new double [n];
    
    for(k = 0; k <= n - 1; k++)
    {
		w=a[0]+1.0e-25;

        m = n - k - 1;
        
        for(i = 1; i < n; i++)
        {
            g = a[i * n];
            b[i] = g / w;
            
            if(i <= m)
                b[i] = -b[i];

            int tmpOffset1 = (i - 1) * n - 1;
            int tmpOffset2 = i * n;

            for(j = 1; j <= i; j++)
                a[tmpOffset1 + j] = a[tmpOffset2 + j] + g * b[j];
        }
        a[n * n - 1] = 1.0 / w;
        
        for(i = 1; i <= n - 1; i++)
            a[(n - 1) * n + i - 1] = b[i];
    }
    
    for(i = 0; i <= n - 2; i++)
        for(j = i + 1; j <= n - 1; j++)
            a[i * n + j] = a[j * n + i];
    delete b;	b = NULL;
    return(2);
}
