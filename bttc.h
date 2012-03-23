

#ifndef BTTC_H
#  define BTTC_H

/**
 * @mainpage libbttc doxygen documentation
 * @author Edgar Simo-Serra <esimo@iri.upc.edu>
 * @version 1.0
 * @date March 2012
 *
 * @section License
 *
 @verbatim
    Copyright 2012 Edgar Simo-Serra

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

    Edgar Simo-Serra, Institut de Robotica i Informatica Industrial (CSIC-UPC)
    esimo@iri.upc.edu, http://www-iri.upc.es/people/esimo/
 @endverbatim
 *
 *
 * @section Overview
 *
 * This library calculates the faces obtained by B-Tree Triangular Coding
 * (BTTC). This is usually for subdividing an image into a triangular mesh. The
 * core library is written in C but an octave/matlab interface is provided.
 *
 * The main focus of this library is simplicity. The code is simple enough to
 * directly integrate it into another program as it is a single C code file
 * with no external dependencies.
 *
 *
 * @section Installation
 *
 * To compile the C interface to the library it is sufficient to run:
 * @code
 * make # compiles the library
 * @endcode
 * To install the C interface you additionally must run (as superuser):
 * @code
 * make install # installs by default to /usr (edit Makefile to change)
 * @endcode
 *
 * The octave/matlab interfaces are separate from the C library and can
 * respectively be compiled by:
 * @code
 * make octave # compiles octave interface
 * make matlab # compiles matlab interface
 * @endcode
 *
 *
 * @section Example
 *
 * This example uses the matlab interface and operates on the classic lena
 * image, which should be 512x512 and grayscale. The matlab function accepts
 * two parameters, first is the image and second is the threshold to use. It
 * can output the result in two different ways. With a single output value it
 * will output all the x,y coordinates of the triangle corners. With two values
 * it will output first a list of the vertices and secondly the IDs of the
 * vertices which correspond to the corners of the triangle faces. The example
 * should be more explanatory.
 *
 * @code
 * lena  = imread( 'lena512.bmp' ); % Load the image
 * % Image must be doubles of size must be 2^m-1
 * dlena = imresize( double(lena), [ 513, 513 ], 'bilinear' );
 *
 * % First we shall do it with one output parameter
 * faces = bttc_m( dlena, 60 ); % Image is dlena with a threshold of 60
 * figure;
 * imshow( dlena ./ 255 ); % Display the image
 * hold on;
 * % Draw all the face lines one by one
 * for i=1:size(faces,1)
 *    f = faces(i,:);
 *    x = [f(:,1) f(:,3) f(:,5) f(:,1)];
 *    y = [f(:,2) f(:,4) f(:,6) f(:,2)];
 *    line( x, y, 'LineWidth', 1, 'Color', 'g' );
 * end
 *
 * % Now we can do it with two output parameters
 * [ Vi, Fi ] = bttc_m( dlena, 60 );
 * figure;
 * imshow( dlena ./ 255 ); % Display the image
 * hold on;
 * % Draw all the face lines one by one
 * for i=1:size(Fi,1)
 *    v = Vi( Fi(i,:), : );
 *    x = [v(:,1);v(1,1)];
 *    y = [v(:,2);v(1,2)];
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
 *  -Distasi, R.; Nappi, M.; Vitulano, S.; "Image compression by B-tree triangular coding," Communications, IEEE Transactions on, vol.45, no.9, pp.1095-1100, Sep 1997
 *
 * @sa bttc
 */


/**
 * @brief Computes the B-Tree Triangular Coding of an image.
 *
 * This function returns faces in the form of a long vector grouped into batches of 6.
 *
 * @code
 * faces = [ batch1 batch2 batch3 ... batchn ]
 * @endcode
 *
 * where each batch has the form of
 *
 * @code
 * batch = [ x0 y0 x1 y1 x2 y2 ]
 * @endcode
 *
 *    @param[out] n Number of faces found (minimum 2 if no error occurred).
 *    @param[in] img Image to perform the coding on. This image is row-major
 *               and has a pitch of 'pitch' between rows.
 *    @param[in] pitch Pitch to shift when looking up rows in the image,
 *               should be equal to size if the entire image is being processed.
 *    @param[in] size Must a value in the form of 2^m+1 for m>=1. It indicates the
 *               area to process, which goes from [0,size-1] in both x and y directions.
 *    @param[in] threshold Threshold to mesh based on. The range of the threshold depends on
 *               the value range of the image and is the limit of error between the linear
 *               interpolation of the triangular face and the real value of the pixels.
 *    @return NULL on error or the list of faces found on success (with n faces or batches).
 *            This value is dynamically allocated and thus must be freed by the user.
 */
int *bttc( int *n, const double *img, int pitch, int size, double threshold );


#endif /* BTTC_H */



