/*
 * See licensing information in bttc.h
 */

#include "bttc.h"

#include <math.h> /* fabs */
#include <stdlib.h> /* malloc */
#include <string.h> /* memcpy */


#define MIN(x,y)  ((x)<(y)?(x):(y)) /**< Helper minimum function. */
#define MAX(x,y)  ((x)>(y)?(x):(y)) /**< Helper maximum function. */


/**
 * @brief Macro helper to make some things easier.
 */
#define TSET(T,x1,y1,x2,y2,x3,y3) \
do { \
   T[0] = x1; T[1] = y1; \
   T[2] = x2; T[3] = y2; \
   T[4] = x3; T[5] = y3; \
} while (0)


/**
 * @brief Calculates the G value of a PRAT.
 *
 * This is direct implementation of the function as defined in the reference paper.
 */
static double bttc_G( const int T[6], int x, int y, const double c[3] )
{
   double xx, yy, x1,y1, x2,y2, x3,y3;
   double alpha, beta;

   xx = x;    yy = y;
   x1 = T[0]; y1 = T[1];
   x2 = T[2]; y2 = T[3];
   x3 = T[4]; y3 = T[5];

   alpha = ((xx-x1)*(y3-y1)-(yy-y1)*(x3-x1)) / ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1));
   beta  = ((x2-x1)*(yy-y1)-(y2-y1)*(xx-x1)) / ((x2-x1)*(y3-y1)-(y2-y1)*(x3-x1));

   return c[0] + alpha*(c[1]-c[0]) + beta*(c[2]-c[0]);
}


int* bttc( int *n, const double *img, int pitch, int size, double threshold )
{
   size_t nstack, mstack;
   int *stack;
   size_t ndone, mdone;
   int *done;
   int *T1, *T2, *T;
   int m, met, pm[8];
   double c[3];
   int x, y, box[4];
   double G, F;
   double dot00, dot01, dot11, invDenom;
   int v0[2], v1[2];

   /* size must be in the form of 2^m+1 */
   if ((size<=0) || (((size-1) & (size-2)) != 0)) {
      *n = 0;
      return NULL;
   }

   /* Allocate temporary buffer. */
   mstack   = 256;
   stack    = malloc( mstack * 6*sizeof(int) );

   /* Allocate output buffer. */
   mdone    = 256;
   ndone    = 0;
   done     = malloc( mdone * 6*sizeof(int) );

   /* Initial two nodes. */
   m        = size-1;
   T1       = &stack[ 6*0 ];
   TSET( T1, 0, 0, 0, m, m, 0 );
   T2       = &stack[ 6*1 ];
   TSET( T2, m, m, m, 0, 0, m );
   nstack   = 2;

   /* Now we must purge the infidels. */
   while (nstack > 0) {
      /* Get the PRAT to work with. */
      T = &stack[ 6*(--nstack) ];

      /* Get colours. */
      c[0] = img[ pitch*T[0] + T[1] ];
      c[1] = img[ pitch*T[2] + T[3] ];
      c[2] = img[ pitch*T[4] + T[5] ];

      /* Check condition for all points in the PRAT. We do this by checking all
       * the points in the bounding box of the triangle that meet the criteria of
       * belonging to the triangle. Reference for this implementation look at:
       * http://www.blackpawn.com/texts/pointinpoly/default.html
       * We have tried to optimize it by removing as many calculations as possible
       * out of the loop. */
      v0[0]  = T[4] - T[0];
      v0[1]  = T[5] - T[1];
      v1[0]  = T[2] - T[0];
      v1[1]  = T[3] - T[1];
      dot00  = v0[0]*v0[0] + v0[1]*v0[1];
      dot01  = v0[0]*v1[0] + v0[1]*v1[1];
      dot11  = v1[0]*v1[0] + v1[1]*v1[1];
      invDenom = 1. / (dot00 * dot11 - dot01 * dot01);
      /* Bounding box. */
      box[0] = MIN( T[0], MIN( T[2], T[4] ) );
      box[1] = MIN( T[1], MIN( T[3], T[5] ) );
      box[2] = MAX( T[0], MAX( T[2], T[4] ) );
      box[3] = MAX( T[1], MAX( T[3], T[5] ) );
      met    = 1; /* Initially we consider it's matching. */
      for (x=box[0]; x<=box[2]; x++) {
         for (y=box[1]; y<=box[3]; y++) {
            int v2[2];
            double dot02, dot12;
            double u, v;
            v2[0] = x - T[0];
            v2[1] = y - T[1];
            dot02 = v0[0]*v2[0] + v0[1]*v2[1];
            dot12 = v1[0]*v2[0] + v1[1]*v2[1];
            u     = (dot11 * dot02 - dot01 * dot12) * invDenom;
            v     = (dot00 * dot12 - dot01 * dot02) * invDenom;
            if ((u < 0.) || (v < 0.) || (u + v > 1.))
               continue;

            /* Check the error. */
            G = bttc_G( T, x, y, c );
            F = img[ pitch*x + y ];
            if (fabs( F - G ) > threshold) {
               met = 0;
               break;
            }
         }
         /* Break out of both loops. */
         if (!met)
            break;
      }

      /* Condition met, this PRAT is done. */
      if (met) {
         if (ndone+1 > mdone) {
            mdone *= 2;
            done   = realloc( done, mdone * 6*sizeof(int) );
         }
         memcpy( &done[ 6*ndone++ ], T, 6*sizeof(int) );
         continue;
      }

      /* Must split the PRAT into two. */
      if (nstack+2 > mstack) {
         mstack *= 2;
         stack   = realloc( stack, mstack * 6*sizeof(int) );
      }
      pm[0]   = (T[2]+T[4])/2;
      pm[1]   = (T[3]+T[5])/2;
      memcpy( &pm[2], T, 6*sizeof(int) ); /* Must copy over T as we'll overwrite it. */
      T1      = &stack[ 6*(nstack+0) ];
      T2      = &stack[ 6*(nstack+1) ];
      TSET( T1, pm[0], pm[1], pm[2], pm[3], pm[4], pm[5] );
      TSET( T2, pm[0], pm[1], pm[6], pm[7], pm[2], pm[3] );
      nstack += 2;
   }

   /* Finished here. */
   *n = (int)ndone;
   return done;
}


