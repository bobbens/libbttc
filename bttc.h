

#ifndef BTTC_H
#  define BTTC_H

/**
 * @mainpage libbttc doxygen documentation
 * @author Edgar Simo-Serra <esimo@iri.upc.edu>
 * @version 1.0
 * @date March 2011
 *
 * @section License
 *
 @verbatim
    Copyright 2010, 2011 Edgar Simo-Serra

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 @endverbatim
 *
 *
 * @section Overview
 *
 * This library calculates the faces obtained by B-Tree Triangular Compression
 * (BTTC). This is usually for subdividing an image into a triangular mesh. The
 * core library is written in C but an octave/matlab interface is provided.
 *
 *
 * @section Example
 *
 * This example uses the matlab interface and operates on the classic lena
 * image, which should be 512x512 and grayscale.
 *
 * @code
 * lena  = imread( 'lena512.bmp' );
 * dlena = imresize( double(lena), [ 513, 513 ], 'bilinear' );
 * faces = bttc_m( dlena, 60 );
 *
 * imshow( dlena ./ 255 );
 * hold on;
 *
 * for i=1:size(faces,1)
 *    f = faces(i,:);
 *    x = [f(:,1) f(:,3) f(:,5) f(:,1)];
 *    y = [f(:,2) f(:,4) f(:,6) f(:,2)];
 *    line( x, y, 'LineWidth', 1, 'Color', 'g' );
 * end
 * @endcode
 *
 *
 * @section Changelog
 *
 * - Version 1.0, March 2012
 *    - Initial release.
 *
 * @section References
 *
 *  -Distasi, R.; Nappi, M.; Vitulano, S.; , "Image compression by B-tree triangular coding," Communications, IEEE Transactions on , vol.45, no.9, pp.1095-1100, Sep 1997
 *
 * @sa bttc
 */


/**
 * @brief Computes the B-Tree Triangular Compression of an image.
 *
 * This function returns faces in the form of a long vector grouped into batches of 6.
 *
 * faces = [ batch1 batch2 batch3 ... batchn ]
 *
 * where each batch has the form of
 *
 * batch = [ x0 y0 x1 y1 x2 y2 ]
 *
 *    @param[out] n Number of faces found (minimum 2 if no error occurred).
 *    @param[in] img Image to perform compression on.
 *    @param[in] pitch Pitch to shift when looking up rows in the image, should be equal to size if the entire image is being processed.
 *    @param[in] size Must a value in the form of 2^m+1 for m>=1.
 *    @param[in] threshold Threshold to mesh based on.
 *    @return NULL on error or the list of faces found on success (with n faces or batches).
 */
int *bttc( int *n, const double *img, int pitch, int size, double threshold );


#endif /* BTTC_H */



