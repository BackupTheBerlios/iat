/**
 * Copyright (C) 2009 
 *	- Salvatore Santagati <salvatore.santagati@gmail.com>
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

#ifndef MKCORE_H
#include "mkcore.h"
#endif

/* --- @img_2_bin@ --- *
 *
 * Arguments:   @file_ptrs *fptrs@ = input file
 * 		@int block_old@ = size of origin block
 *		@int block_new@ = size of new block
 *		@off_t pregap@ = length of pregap
 *
 *
 * Returns:	mode of image, @-1@ otherwise
 *
 * Use:		convert image to image as compatible for cuesheet file. 
 */
int img_2_bin ( file_ptrs* fptrs,  int block_old, int block_new, off_t pregap )
{
	int n_return_value = ERROR; 

	if ( img_2_img ( fptrs, block_old, block_new, pregap ) ) n_return_value = AOK;

	return ( n_return_value );	
}

/* --- @track_vcd@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *		@struct_cue *cue@ = pointer struct of cuesheet
 *		@off_t n_loop@  = number of byte from where the block starts
 *
 *
 * Returns:     Zeor on success, @-1@ on error. 
 *
 * Use:         detect track of VCD/SVCD image.
 */
int track_vcd ( file_ptrs* fptrs, image_struct* img_struct, struct_cue* cue, off_t n_loop )
{
	int     n_return_value = ERROR;
	static int file_number = 1;
	
	copy_sub_header		copy_header;

	switch ( img_struct->block ) {
		case 2352:
			n_loop +=16;
		case 2336:
			set_file_pointer ( fptrs -> fsource, ( n_loop ) );
			
			fread ( &copy_header, 1, sizeof ( copy_sub_header ), fptrs -> fsource );
			/* new file number + real time mode = new track */	
			if ( (  file_number  == ( *( copy_header.file ) ) )  && ( SUB_MODE_RT & ( *( copy_header.sub_mode ) ) ) ) {
				file_number++;				
				n_return_value = AOK;	
			}
		default:
			break;
			
	}

	return ( n_return_value );

}
