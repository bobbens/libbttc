/*
 * See licensing information in bttc.h
 */

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
   if ((nlhs < 1) || (nlhs > 2))
      mexErrMsgTxt("Function must have one or two outputs.");
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
   if (faces == NULL) {
      mexErrMsgTxt("bttc failed to run.");
      return;
   }

   if (nlhs == 1) {
      int dims[2];
      dims[0] = n;
      dims[1] = 6;
      plhs[0] = mxCreateNumericArray( 2, dims, mxDOUBLE_CLASS, mxREAL );
      double *dat = (double*)mxGetData(plhs[0]);
      int i, j;
      for (i=0; i<n; i++)
         for (j=0; j<6; j++)
            dat[ j*n + i ] = (double) faces[ 6*i + j ]+1;
   }
   else {
      int nvert;
      double *vert;
      int *face, *v;
      int i, j, k, f;

      /* Do face meshing. */
      nvert = 0;
      vert  = malloc( n * 3*3*sizeof(double) ); /* Worst case, every vertex is unique. */
      face  = malloc( n * 3*sizeof(int) );
      for (i=0; i<n; i++) {
         for (j=0; j<3; j++) {
            v = &faces[ 6*i + 2*j ];
            f = 0;
            for (k=0; k<nvert; k++) {
               if ((v[0]==(int)vert[3*k+0]) &&
                   (v[1]==(int)vert[3*k+1])) {
                  f = 1;
                  break;
               }
            }
            /* Already exists. */
            if (!f) {
               vert[ 3*nvert+0 ] = v[0];
               vert[ 3*nvert+1 ] = v[1];
               vert[ 3*nvert+2 ] = img[ size*v[1] + v[0] ];
               k      = nvert;
               nvert++;
            }

            /* Set the face id. */
            face[ 3*i+j ] = k;
         }
      }

      double *dat;
      int dims[2];
      /* Save the vertex array. */
      dims[0] = nvert;
      dims[1] = 3;
      plhs[0] = mxCreateNumericArray( 2, dims, mxDOUBLE_CLASS, mxREAL );
      dat = (double*)mxGetData(plhs[0]);
      for (i=0; i<nvert; i++) {
         dat[ 0*dims[0] + i ] = vert[ 3*i + 0 ]+1.;
         dat[ 1*dims[0] + i ] = vert[ 3*i + 1 ]+1.;
         dat[ 2*dims[0] + i ] = vert[ 3*i + 2 ];
      }
      /* Save the faces. */
      dims[0] = n;
      plhs[1] = mxCreateNumericArray( 2, dims, mxDOUBLE_CLASS, mxREAL );
      dat = (double*)mxGetData(plhs[1]);
      for (i=0; i<n; i++)
         for (j=0; j<3; j++)
            dat[ j*dims[0] + i ] = (double)face[ 3*i + j ]+1;

      /* Clean up. */
      free(face);
      free(vert);
   }
   
   /* Clean up. */
   free(faces);
}


