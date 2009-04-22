/**
 * Copyright (C) 2009 
 *	- Salvatore Santagati <salvatore.santagati@gmail.com>
 * 	- Abdur Rab <c.abdur@yahoo.com>
 *
 * All rights reserved.
 *
 * This program is free software; under the terms of the 
 * GNU General Public License as published by the Free Software Foundation; 
 * either version 2 of the License, or (at your option) any later version.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * @ Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer. 
 *
 * @ Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef CALCULATE_PREGAP_H
#include "calculate_pregap.h"
#endif


/* SUB-HEADER */
/*unsigned const char SUB_HEADER_ID_1 [ 8 ] = { 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00 } ; 
unsigned const char SUB_HEADER_ID_2 [ 8 ] = { 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x88, 0x00 } ;
unsigned const char SUB_HEADER_ID_3 [ 8 ] = { 0x00, 0x00, 0x89, 0x00, 0x00, 0x00, 0x89, 0x00 } ;
unsigned const char SUB_HEADER_ID_4 [ 8 ] = { 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00 } ;
*/


/* --- @calculate_pregap_length@ --- *
 *  
 * Arguments: 	@int cd_id_start@ = bytes when is detect primary volume of CD/DVD
 *		@int block@ = block size of image 
 *		@int header@ = header of block  	
 * 
 * Returns : 	Pregap of image.
 * 
 * Use:		Return pregap of image.	 
 */
off_t calculate_pregap_length ( off_t  cd_id_start, image_struct* img_struct, int header )
{
	return ( ( cd_id_start - ( img_struct -> block * 16 ) ) - header ); 
}

/* --- @calculate_block_size@ --- *
 *
 * Arguments:	@int cd_id_start@ = bytes when is detect primary volume of CD/DVD 
 *		@int cd_id_end@	= bytes when is present next volume of CD/DVD
 * 
 * Returns :	block size.
 * 
 * Use:		Return block size image.
 */
int calculate_block_size (  off_t cd_id_start, off_t cd_id_end, image_struct* img_struct )
{

	int block_sizes [ ] = { 2048, 2336, 2352, 2448 };
	size_t block  = cd_id_end - cd_id_start;

	img_struct -> block  = ( ( block % block_sizes [ 0 ] ) == 0 ) ? block_sizes [ 0 ] :
		( ( block % block_sizes [ 1 ] ) == 0 ) ? block_sizes [ 1 ] :
		( ( block % block_sizes [ 2 ] ) == 0 ) ? block_sizes [ 2 ] :
		block_sizes [ 3 ];

	/*for (; img_struct -> block > 2448; img_struct -> block /= 2 );
	if ( img_struct -> block < 2048 ) img_struct -> block *= 2;
	*/

	return ( img_struct -> block );	
}

/* --- @calculate_pregap@ --- *
 *
 * Arguments:   @file_ptrs *fptrs@ = pointer struct of source and destination file
 *		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *
 * Returns:     Zeor on success, @-1@ on error.
 *
 * Use:         Return the detection of the image.
 */
off_t calculate_pregap ( file_ptrs* fptrs,  image_struct*  img_struct ) 
{
	unsigned char	buf [ 12 ];
	/*int		img_size ;*/
	off_t		img_size ;
        off_t 		n_loop = 0;
	off_t		start = 32768; 
	/*int		start = 32768;  first primary volume  at 16 * BLOCK */
	off_t		cd_id_start = 0;
	off_t		cd_id_end = 0;
	int		header = 0;

	/* SYNCH HEADER */
	unsigned const char HEADER_ID [ 12 ] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
	
	/* SCD/VCD Signature */
	unsigned const char XA_ID [ 8 ] = { 0x43, 0x44, 0x2D, 0x58, 0x41, 0x30, 0x30, 0x31 };
	
	/* Signature for Image ISO-9660 */
	unsigned const char ISO_9660_START [ 8 ] = { 0x01, 0x43, 0x44, 0x30, 0x30, 0x31, 0x01, 0x00 };
	unsigned const char ISO_9660_END [ 8 ] = { 0xFF, 0x43, 0x44, 0x30, 0x30, 0x31, 0x01, 0x00 };


	img_struct -> type = 0;
	img_size = get_file_size ( fptrs -> fsource ) ; 
	
	for ( n_loop = start ; n_loop < img_size; n_loop++ )
	{
		set_file_pointer ( fptrs -> fsource, n_loop );
		fread ( buf, sizeof ( char ), 12, fptrs -> fsource );
	
		if ( !memcmp ( HEADER_ID, buf, 12 ) ) {
			if ( ( img_struct -> type == 0 ) || ( img_struct -> type == 2 ) ) {
				img_struct -> type += 1;
			}
		
			if ( ( header != 16 ) && ( header != 24 ) )
				header += 16;
		} /*else if ( ( !memcmp ( SUB_HEADER_ID_1, buf, 8 ) ) || ( !memcmp ( SUB_HEADER_ID_2, buf, 8 ) ) ||
			( !memcmp ( SUB_HEADER_ID_3, buf, 8 ) ) || ( !memcmp ( SUB_HEADER_ID_4, buf, 8 ) ) ) {*/
			
		else if ( !is_svcd_sub_header ( (unsigned char*) buf ) ) {
			if ( (	img_struct -> type == 0 ) || ( img_struct -> type == 2 ) ) {
				img_struct -> type += 1;
			}
			
			if ( ( header != 8 ) && ( header != 24 ) )
				header += 8;
			 
		} else if ( !memcmp ( ISO_9660_START, buf, 8 ) ) {
			cd_id_start = n_loop;
			n_loop += 1023;
			img_struct -> type += 2;
		} else if ( !memcmp ( ISO_9660_END, buf, 8 ) ) {
			cd_id_end = n_loop;
			n_loop = img_size;
		} else if ( !memcmp ( XA_ID, buf, 8 ) ) { 
			if ( img_struct -> type <= 7 )
				img_struct -> type += 7;
		}
	}
		
	/* Detect Block of image */	
	img_struct -> block  = calculate_block_size ( cd_id_start, cd_id_end, img_struct );
	
	/* Detect Header bytes */
	img_struct -> pregap = calculate_pregap_length ( cd_id_start, img_struct , header );
	printf(" PREGAP (%d)\n", img_struct -> pregap );	
	printf(" BLOCK (%d)\n", img_struct -> block );

	return ( 0 );	
}

