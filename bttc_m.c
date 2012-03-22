

#include "bttc.h"

#include "mex.h"


void mexFunction( int nlhs, mxArray *plhs[],
      int nrhs, const mxArray *prhs[] )
{
   int *faces, n;
   double *img, threshold;
   int size;

   /* Check parameters. */
   if (nrhs < 2)
      mexErrMsgTxt("At least two input arguments required.");
   if (!mxIsDouble(prhs[0]))
      mexErrMsgTxt("Image parameter must be double matrix." );
   if (!mxIsDouble(prhs[1]))
      mexErrMsgTxt("Threshold parameter must be double." );

   /* Get parameters. */
   img       = (double*)mxGetData( prhs[0] );
   threshold = mxGetScalar( prhs[1] );

   /* Checks. */
   const mwSize *dim = mxGetDimensions(prhs[0]);
   if (dim[0] != dim[1])
      mexErrMsgTxt("Image must be square.");
   size = dim[0];
   if (((size-1)<=0) || (((size-1) & (size-2)) != 0))
      mexErrMsgTxt("Image side must be in the form of 2^m+1.");

   /* Actual algorithm. */
   faces = bttc( &n, img, size, size, threshold );
   if (faces == NULL)
      mexErrMsgTxt("bttc failed to run.");

   int dims[2];
   dims[0] = n;
   dims[1] = 6;
   plhs[0] = mxCreateNumericArray( 2, dims, mxDOUBLE_CLASS, mxREAL );
   double *dat = (double*)mxGetData(plhs[0]);
   int i, j;
   for (i=0; i<n; i++)
      for (j=0; j<6; j++)
         dat[ j*n + i ] = (double) faces[ 6*i + j ];
   free(faces);
}


