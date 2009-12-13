

/*********************************/
/* Principal Components Analysis */
/*********************************/

/*********************************************************************/
/* Principal Components Analysis or the Karhunen-Loeve expansion is a
   classical method for dimensionality reduction or exploratory data
   analysis.  One reference among many is: F. Murtagh and A. Heck,
   Multivariate Data Analysis, Kluwer Academic, Dordrecht, 1987.

   Author:
   F. Murtagh
   Phone:        + 49 89 32006298 (work)
                 + 49 89 965307 (home)
   Earn/Bitnet:  fionn@dgaeso51,  fim@dgaipp1s,  murtagh@stsci
   Span:         esomc1::fionn
   Internet:     murtagh@scivax.stsci.edu

   F. Murtagh, Munich, 6 June 1989                                   */
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cmupca.h>

#include <malloc.h>



#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )

CMUPCA::CMUPCA()
{}
eigenVectorsCMUPCA * CMUPCA::returnEigens()
{

    eigenVectorsCMUPCA * newEigen = (eigenVectorsCMUPCA *) malloc(sizeof(eigenVectorsCMUPCA));
    newEigen->count=nEigens;
    newEigen->eigens=eigenVectArr;
    return newEigen;

}
IplImage * CMUPCA::returnAverage()
{
    for (int i =0;i<averageInput->width;i++)
    {
        for (int j=0;j<averageInput->height;j++)
        {
            CvScalar s;
            s =cvGet2D(averageInput,j,i);
//            printf("%e \n",s.val[0]);

        }
    }
    return averageInput;

}

/**  Correlation matrix: creation  ***********************************/

void CMUPCA::corcol(double **data, int n, int m, double **symmat)
/* Create m * m correlation matrix from given n * m data matrix. */
{
    double eps = 0.005;
    double x, *mean, *stddev;
    int i, j, j1, j2;

    /* Allocate storage for mean and std. dev. vectors */

    mean = vector(m);
    stddev = vector(m);

    /* Determine mean of column vectors of input data matrix */

    for (j = 1; j <= m; j++)
    {
        mean[j] = 0.0;
        for (i = 1; i <= n; i++)
        {
            mean[j] += data[i][j];
        }
        mean[j] /= (double)n;
    }

    printf("\nMeans of column vectors:\n");
    for (j = 1; j <= m; j++)
    {
        CvScalar s;
        s.val[0]=(double)mean[j];
        cvSet2D(averageInput,floor((j-1)/width),(j-1)%width,s);

        printf("%7.1f",mean[j]);
    }
    printf("\n");

    /* Determine standard deviations of column vectors of data matrix. */

    for (j = 1; j <= m; j++)
    {
        stddev[j] = 0.0;
        for (i = 1; i <= n; i++)
        {
            stddev[j] += (   ( data[i][j] - mean[j] ) *
                             ( data[i][j] - mean[j] )  );
        }
        stddev[j] /= (double)n;
        stddev[j] = sqrt(stddev[j]);
        /* The following in an inelegant but usual way to handle
        near-zero std. dev. values, which below would cause a zero-
        divide. */
        if (stddev[j] <= eps) stddev[j] = 1.0;
    }

    printf("\nStandard deviations of columns:\n");
    for (j = 1; j <= m; j++)
    {
        printf("%7.1f", stddev[j]);
    }
    printf("\n");

    /* Center and reduce the column vectors. */

    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= m; j++)
        {
            data[i][j] -= mean[j];
            x = sqrt((double)n);
            x *= stddev[j];
            data[i][j] /= x;
        }
    }

    /* Calculate the m * m correlation matrix. */
    for (j1 = 1; j1 <= m-1; j1++)
    {
        symmat[j1][j1] = 1.0;
        for (j2 = j1+1; j2 <= m; j2++)
        {
            symmat[j1][j2] = 0.0;
            for (i = 1; i <= n; i++)
            {
                symmat[j1][j2] += ( data[i][j1] * data[i][j2]);
            }
            symmat[j2][j1] = symmat[j1][j2];
        }
    }
    symmat[m][m] = 1.0;

    return;

}

/**  Variance-covariance matrix: creation  *****************************/

void CMUPCA::covcol(double ** data, int n,int  m, double **symmat)
/* Create m * m covariance matrix from given n * m data matrix. */
{
    double *mean;
    int i, j, j1, j2;

    /* Allocate storage for mean vector */

    mean = vector(m);

    /* Determine mean of column vectors of input data matrix */

    for (j = 1; j <= m; j++)
    {
        mean[j] = 0.0;
        for (i = 1; i <= n; i++)
        {
            mean[j] += data[i][j];
        }
        mean[j] /= (double)n;
    }

    printf("\nMeans of column vectors:\n");
    for (j = 1; j <= m; j++)
    {
        printf("%7.1f",mean[j]);
    }
    printf("\n");

    /* Center the column vectors. */

    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= m; j++)
        {
            data[i][j] -= mean[j];
        }
    }

    /* Calculate the m * m covariance matrix. */
    for (j1 = 1; j1 <= m; j1++)
    {
        for (j2 = j1; j2 <= m; j2++)
        {
            symmat[j1][j2] = 0.0;
            for (i = 1; i <= n; i++)
            {
                symmat[j1][j2] += data[i][j1] * data[i][j2];
            }
            symmat[j2][j1] = symmat[j1][j2];
        }
    }

    return;

}

/**  Sums-of-squares-and-cross-products matrix: creation  **************/

void CMUPCA::scpcol(double **data, int n,int m, double **symmat)
/* Create m * m sums-of-cross-products matrix from n * m data matrix. */
{
    int i, j1, j2;

    /* Calculate the m * m sums-of-squares-and-cross-products matrix. */

    for (j1 = 1; j1 <= m; j1++)
    {
        for (j2 = j1; j2 <= m; j2++)
        {
            symmat[j1][j2] = 0.0;
            for (i = 1; i <= n; i++)
            {
                symmat[j1][j2] += data[i][j1] * data[i][j2];
            }
            symmat[j2][j1] = symmat[j1][j2];
        }
    }

    return;

}

/**  Error handler  **************************************************/

void CMUPCA::erhand(char err_msg[])
/* Error handler */
{
    fprintf(stderr,"Run-time error:\n");
    fprintf(stderr,"%s\n", err_msg);
    fprintf(stderr,"Exiting to system.\n");
}

/**  Allocation of vector storage  ***********************************/

double *CMUPCA::vector(int n)
/* Allocates a double vector with range [1..n]. */
{

    double *v;

    v = (double *) malloc ((unsigned) n*sizeof(double));
    if (!v) erhand("Allocation failure in vector().");
    return v-1;

}

/**  Allocation of double matrix storage  *****************************/

double **CMUPCA::matrix(int n,int m)
/* Allocate a double matrix with range [1..n][1..m]. */
{
    int i;
    double **mat;

    /* Allocate pointers to rows. */
    mat = (double **) malloc((unsigned) (n)*sizeof(double*));
    if (!mat) erhand("Allocation failure 1 in matrix().");
    mat -= 1;

    /* Allocate rows and set pointers to them. */
    for (i = 1; i <= n; i++)
    {
        mat[i] = (double *) malloc((unsigned) (m)*sizeof(double));
        if (!mat[i]) erhand("Allocation failure 2 in matrix().");
        mat[i] -= 1;
    }

    /* Return pointer to array of pointers to rows. */
    return mat;

}

/**  Deallocate vector storage  *********************************/

void CMUPCA::free_vector(double * v,int n)

/* Free a double vector allocated by vector(). */
{
    free((char*) (v+1));
}

/**  Deallocate double matrix storage  ***************************/

void CMUPCA::free_matrix(double ** mat,int n,int m)

/* Free a double matrix allocated by matrix(). */
{
    int i;

    for (i = n; i >= 1; i--)
    {
        free ((char*) (mat[i]+1));
    }
    free ((char*) (mat+1));
}

/**  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */

void CMUPCA::tred2(double **a, int n,double * d, double *e)

{
    int l, k, j, i;
    double scale, hh, h, g, f;

    for (i = n; i >= 2; i--)
    {
        l = i - 1;
        h = scale = 0.0;
        if (l > 1)
        {
            for (k = 1; k <= l; k++)
                scale += fabs(a[i][k]);
            if (scale == 0.0)
                e[i] = a[i][l];
            else
            {
                for (k = 1; k <= l; k++)
                {
                    a[i][k] /= scale;
                    h += a[i][k] * a[i][k];
                }
                f = a[i][l];
                g = f>0 ? -sqrt(h) : sqrt(h);
                e[i] = scale * g;
                h -= f * g;
                a[i][l] = f - g;
                f = 0.0;
                for (j = 1; j <= l; j++)
                {
                    a[j][i] = a[i][j]/h;
                    g = 0.0;
                    for (k = 1; k <= j; k++)
                        g += a[j][k] * a[i][k];
                    for (k = j+1; k <= l; k++)
                        g += a[k][j] * a[i][k];
                    e[j] = g / h;
                    f += e[j] * a[i][j];
                }
                hh = f / (h + h);
                for (j = 1; j <= l; j++)
                {
                    f = a[i][j];
                    e[j] = g = e[j] - hh * f;
                    for (k = 1; k <= j; k++)
                        a[j][k] -= (f * e[k] + g * a[i][k]);
                }
            }
        }
        else
            e[i] = a[i][l];
        d[i] = h;
    }
    d[1] = 0.0;
    e[1] = 0.0;
    for (i = 1; i <= n; i++)
    {
        l = i - 1;
        if (d[i])
        {
            for (j = 1; j <= l; j++)
            {
                g = 0.0;
                for (k = 1; k <= l; k++)
                    g += a[i][k] * a[k][j];
                for (k = 1; k <= l; k++)
                    a[k][j] -= g * a[k][i];
            }
        }
        d[i] = a[i][i];
        a[i][i] = 1.0;
        for (j = 1; j <= l; j++)
            a[j][i] = a[i][j] = 0.0;
    }
}

/**  Tridiagonal QL algorithm -- Implicit  **********************/

void CMUPCA::tqli(double d[] , double e[],int n, double ** z)

{
    int m, l, iter, i, k;
    double s, r, p, g, f, dd, c, b;


    for (i = 2; i <= n; i++)
        e[i-1] = e[i];
    e[n] = 0.0;
    for (l = 1; l <= n; l++)
    {
        iter = 0;
        do
        {
            for (m = l; m <= n-1; m++)
            {
                dd = fabs(d[m]) + fabs(d[m+1]);
                if (fabs(e[m]) + dd == dd) break;
            }
            if (m != l)
            {
                if (iter++ == 30) erhand("No convergence in TLQI.");
                g = (d[l+1] - d[l]) / (2.0 * e[l]);
                r = sqrt((g * g) + 1.0);
                g = d[m] - d[l] + e[l] / (g + SIGN(r, g));
                s = c = 1.0;
                p = 0.0;
                for (i = m-1; i >= l; i--)
                {
                    f = s * e[i];
                    b = c * e[i];
                    if (fabs(f) >= fabs(g))
                    {
                        c = g / f;
                        r = sqrt((c * c) + 1.0);
                        e[i+1] = f * r;
                        c *= (s = 1.0/r);
                    }
                    else
                    {
                        s = f / g;
                        r = sqrt((s * s) + 1.0);
                        e[i+1] = g * r;
                        s *= (c = 1.0/r);
                    }
                    g = d[i+1] - p;
                    r = (d[i] - g) * s + 2.0 * c * b;
                    p = s * r;
                    d[i+1] = g + p;
                    g = c * r - b;
                    for (k = 1; k <= n; k++)
                    {
                        f = z[k][i+1];
                        z[k][i+1] = s * z[k][i] + c * f;
                        z[k][i] = c * z[k][i] - s * f;
                    }
                }
                d[l] = d[l] - p;
                e[l] = g;
                e[m] = 0.0;
            }
        }
        while (m != l);
    }
}



void  CMUPCA::runPCA(CvMat** dataset,int count)

{
    FILE *stream;
    int  n, m,  i, j, k, k2;
    double **data, **symmat, **symmat2,  *evals, *interm;

    double in_value;
    char option='r';

    /*********************************************************************
       Get from command line:
       input data file name, #rows, #cols, option.

       Open input file: fopen opens the file whose name is stored in the
       pointer argv[argc-1]; if unsuccessful, error message is printed to
       stderr.
    *********************************************************************/
    n = count;
    nEigens=count-1;              /* # rows */
    eigenVectArr=(IplImage **)cvAlloc(nEigens*sizeof(IplImage *));
    averageInput=cvCreateImage(cvSize( dataset[n-1]->width,dataset[n-1]->height),IPL_DEPTH_64F, 1);
    m = dataset[n-1]->width*dataset[n-1]->height;
    width=dataset[n-1]->width;
    height=dataset[n-1]->height;

    data = matrix(n, m);  /* Storage allocation for input data */

    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= m; j++)
        {
            CvScalar s;

            int h= floor((j-1)/width);
            int w=(j-1)%width;
          //  printf("%d %d \n",h,w);
            s=cvGet2D(dataset[i-1],h,w);
            data[i][j] = (double)s.val[0];
        }
    }


    // for (i = 1; i <= n; i++) {for (j = 1; j <= m; j++)  {
    //   printf("%7.1f", data[i][j]);  }  printf("\n");  }
//
    symmat = matrix(m, m);  /* Allocation of correlation (etc.) matrix */

    /* Look at analysis option; branch in accordance with this. */

    switch (option)
    {
    case 'R':
    case 'r':
        printf("Analysis of correlations chosen.\n");
        corcol(data, n, m, symmat);

        /* Output correlation matrix.
        for (i = 1; i <= m; i++) {
         for (j = 1; j <= 8; j++)  {
          printf("%7.4f", symmat[i][j]);  }
          printf("\n");  }
        */
        break;
    case 'V':
    case 'v':
        printf("Analysis of variances-covariances chosen.\n");
        covcol(data, n, m, symmat);

        /* Output variance-covariance matrix.
        for (i = 1; i <= m; i++) {
        for (j = 1; j <= 8; j++)  {
          printf("%7.1f", symmat[i][j]);  }
          printf("\n");  }
        */
        break;
    case 'S':
    case 's':
        printf("Analysis of sums-of-squares-cross-products");
        printf(" matrix chosen.\n");
        scpcol(data, n, m, symmat);

        /* Output SSCP matrix.
        for (i = 1; i <= m; i++) {
         for (j = 1; j <= 8; j++)  {
           printf("%7.1f", symmat[i][j]);  }
           printf("\n");  }
        */
        break;
    default:
        printf("Option: %s\n",option);
        printf("For option, please type R, V, or S\n");
        printf("(upper or lower case).\n");
        printf("Exiting to system.\n");

        break;
    }

    /*********************************************************************
        Eigen-reduction
    **********************************************************************/

    /* Allocate storage for dummy and new vectors. */
    evals = vector(m);     /* Storage alloc. for vector of eigenvalues */
    interm = vector(m);    /* Storage alloc. for 'intermediate' vector */
    symmat2 = matrix(m, m);  /* Duplicate of correlation (etc.) matrix */
    for (i = 1; i <= m; i++)
    {
        for (j = 1; j <= m; j++)
        {
            symmat2[i][j] = symmat[i][j]; /* Needed below for col. projections */
        }
    }
    tred2(symmat, m, evals, interm);  /* Triangular decomposition */
    tqli(evals, interm, m, symmat);   /* Reduction of sym. trid. matrix */
    /* evals now contains the eigenvalues,
       columns of symmat now contain the associated eigenvectors. */

    printf("\nEigenvalues:\n");
    for (j = m; j >= 1; j--)
    {
        printf("%18.5f\n", evals[j]);
    }
    printf("\n(Eigenvalues should be strictly positive; limited\n");
    printf("precision machine arithmetic may affect this.\n");
    printf("Eigenvalues are often expressed as cumulative\n");
    printf("percentages, representing the 'percentage variance\n");
    printf("explained' by the associated axis or principal component.)\n");

    printf("\nEigenvectors:\n");
    printf("(First three; their definition in terms of original vbes.)\n");
    for (i = 1; i <= nEigens; i++)
    {

        eigenVectArr[i-1]=cvCreateImage(cvSize(width,height),IPL_DEPTH_64F,1);

    }
    for (j = 1; j <= m; j++)
    {
        for (i = 1; i <= nEigens; i++)
        {
            CvScalar s;
            s.val[0]= (double)symmat[j][m-i+1];

            cvSet2D(eigenVectArr[i-1],floor((j-1)/width),(j-1)%width,s);


          //  printf("%12.4f", symmat[j][m-i+1]);
        }
        printf("\n");
    }

    /* Form projections of row-points on first three prin. components. */
    /* Store in 'data', overwriting original data. */
    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= m; j++)
        {
            interm[j] = data[i][j];
        }   /* data[i][j] will be overwritten */
        for (k = 1; k <= 3; k++)
        {
            data[i][k] = 0.0;
            for (k2 = 1; k2 <= m; k2++)
            {
                data[i][k] += interm[k2] * symmat[k2][m-k+1];
            }
        }
    }

    printf("\nProjections of row-points on first 3 prin. comps.:\n");
    for (i = 1; i <= n; i++)
    {
        for (j = 1; j <= 3; j++)
        {
            printf("%12.4f", data[i][j]);
        }
        printf("\n");
    }

    /* Form projections of col.-points on first three prin. components. */
    /* Store in 'symmat2', overwriting what was stored in this. */
    for (j = 1; j <= m; j++)
    {
        for (k = 1; k <= m; k++)
        {
            interm[k] = symmat2[j][k];
        }  /*symmat2[j][k] will be overwritten*/
        for (i = 1; i <= 3; i++)
        {
            symmat2[j][i] = 0.0;
            for (k2 = 1; k2 <= m; k2++)
            {
                symmat2[j][i] += interm[k2] * symmat[k2][m-i+1];
            }
            if (evals[m-i+1] > 0.0005)   /* Guard against zero eigenvalue */
                symmat2[j][i] /= sqrt(evals[m-i+1]);   /* Rescale */
            else
                symmat2[j][i] = 0.0;    /* Standard kludge */
        }
    }

    printf("\nProjections of column-points on first 3 prin. comps.:\n");
    for (j = 1; j <= m; j++)
    {
        for (k = 1; k <= 3; k++)
        {
            printf("%12.4f", symmat2[j][k]);
        }
        printf("\n");
    }

    free_matrix(data, n, m);
    free_matrix(symmat, m, m);
    free_matrix(symmat2, m, m);
    free_vector(evals, m);
    free_vector(interm, m);

}
/**  Correlation matrix: creation  ***********************************/
