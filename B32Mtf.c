/*
 * B32Mtf : Author Ernst Berg with assistance from ChatGPT4 October 2023
 * This is Base32_Move_To_Front (B32Mtf) .
 *
 * B32Mtf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * B32Mtf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with B32Mtf.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The Move-to-Front (MTF) algorithm implemented in this project is a widely-used algorithm in the 
 * field of data compression and cryptography. The algorithm was originally invented by Bentley, 
 * Sleator, Tarjan, and Wei, and has been a part of public domain algorithms employed in various applications.
 * 
 * This implementation is inspired by the algorithm's description in multiple public papers and texts. 
 * No proprietary or copyrighted code has been used in constructing it. If you're using this code, 
 * a reference to the original algorithm creators would be appropriate.
 *
 * Use of the wonderful GNU Multiple Perecision Arithmtic Laibrary is announced.
 * Since version 6, GMP is distributed under the dual licenses, GNU LGPL v3 and GNU GPL v2.
 * GMP is part of the GNU project. For more information about the GNU project, please see the official GNU web site.
 * https://www.gnu.org/
 *
 * SEE EOF for Author's comments:
 *
 *
 */

#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>

const char base32_table[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

int check_valid_base32(const char *str) {
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i) {
		char *pos = strchr(base32_table, str[i]);
		if (pos == NULL) {
			printf("Invalid character in input: %c at position %lu\n", str[i], i);
			return 0; // Indicates an invalid character found
		}
	}
	return 1; // Indicates all characters are valid
}

void mpz_to_base32(mpz_t num, char *output) {
	mpz_t tmp, remainder;
	mpz_init_set(tmp, num);
	mpz_init(remainder); // Initialize remainder just once
	int i = 0;

	while (mpz_cmp_ui(tmp, 0) > 0) {
		mpz_fdiv_qr_ui(tmp, remainder, tmp, 32);
		unsigned long int rem = mpz_get_ui(remainder);
		output[i++] = base32_table[rem];
	}

	output[i] = '\0';  // Null-terminate the string

	mpz_clear(remainder); // Clear remainder just once
	mpz_clear(tmp);
}

void base32_to_mpz(const char *input, mpz_t num) {
	mpz_set_ui(num, 0);  // Initialize to zero
	size_t len; 
	for( len = 0; ; len++ ) {
		if( input[len] == '\0' ) break;
	}
	for (ssize_t i = len - 1; i >= 0; i--) {
		char *pos = strchr(base32_table, input[i]);
		if (pos) {
			int value = pos - base32_table;
			mpz_mul_ui(num, num, 32);
			mpz_add_ui(num, num, value);
		} else {
			printf("Invalid character in input: %d index is %d\n", input[i], i);
			return;
		}
	}
}

char indexToBase32(uint8_t index) {
	return base32_table[index];
}

uint8_t base32ToIndex(char c) {
	char *pos = strchr(base32_table, c);
	return pos ? pos - base32_table : 0xFF;  // return 0xFF for error
}

void moveToFront(char *data) {
	char table[32];
	memcpy(table, base32_table, 32);

	size_t len = strlen(data);

	for (size_t i = 0; i < len; ++i) {
		char val = data[i];
		char j;
		for (j = 0; j < 32; ++j) {
			if (table[j] == val) {
				uint8_t index = j;
				data[i] = indexToBase32(index); // Transform index to Base32 char
				memmove(&table[1], &table[0], j);
				table[0] = val;
				break;
			}
		}
	}
}

void reverseMoveToFront(char *data) {
	char table[32];
	memcpy(table, base32_table, 32);

	size_t len = strlen(data);
	printf("MTF DECODE length: %lu\n", len);

	for (size_t i = 0; i < len; ++i) {
		uint8_t index = base32ToIndex(data[i]); // Transform Base32 char to index
		if (index != 0xFF) {
			data[i] = table[index];  // Restore the original character
			memmove(&table[1], &table[0], index);
			table[0] = data[i];
		} else {
			printf("Invalid character in input: %d\n", data[i]);
			return;
		}
	}
}


void SaveFile_MPZ ( mpz_t Data, unsigned int Length, unsigned long long Index0 )
{
	unsigned char              *File_Data, Write_Name[256];
	FILE                       *Write_File;
	unsigned int               Flag1;
	File_Data = calloc( Length, sizeof(unsigned char ));
	if( File_Data == NULL ){ printf("No memory!\n"); exit(0); }
	mpz_export(File_Data,NULL,-1,Length,-1,0,Data);
	sprintf(Write_Name,"Base32_MTF_Experiment_%lu.dat\0",Index0);
	Write_File = fopen(Write_Name,"w");
	fwrite(File_Data,sizeof(unsigned char),Length,Write_File);
	fclose(Write_File);
	printf("File: %s written\n",Write_Name);
	free(File_Data);
}

// B32Mtf 
int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s ENCODE|DECODE infile \n", argv[0]);
		return 1;
	}

	const char *mode = argv[1];
	const char *infile = argv[2];

	struct stat stat_buff;
	mpz_t num;
	mpz_init(num);
	size_t p;
	FILE *Read_File = fopen(infile, "rb");
	fstat(fileno(Read_File), &stat_buff);

	if (Read_File == NULL) {
		fprintf(stderr, "Couldn't open %s\n", infile);
		exit(EXIT_FAILURE);
	}

	uint8_t *S0 = calloc(stat_buff.st_size, sizeof(uint8_t));
	size_t w,z;

	if (fread(S0, sizeof(uint8_t), stat_buff.st_size, Read_File)) {
		printf("Source loaded\n");
	} else {
		printf("Source load error\n");
		exit(EXIT_FAILURE);
	}
	fclose(Read_File);
	mpz_import(num, stat_buff.st_size, -1, sizeof(uint8_t), 0, 0, S0);
	free(S0);
	size_t t = mpz_sizeinbase(num, 32) + 3; 
	uint8_t *base32Str = calloc( t, sizeof(uint8_t)); // Extra extra byte safety for NULL if all bytes are used.
	mpz_to_base32( num, base32Str );
	check_valid_base32(base32Str);

	if (strcmp(mode, "ENCODE") == 0) {
		moveToFront((uint8_t *)base32Str);
		base32_to_mpz( base32Str, num);
		w = z = mpz_sizeinbase( num, 2 );  
		z = z / 8;
		if( w % 8 != 0 ) z++;
		SaveFile_MPZ( num, z, 0 );
	} else {
		reverseMoveToFront((uint8_t *)base32Str);
		base32_to_mpz( base32Str, num);
		w = z = mpz_sizeinbase( num, 2 );  
		z = z / 8;
		if( w % 8 != 0 ) z++;
		SaveFile_MPZ( num, z, 1 );
	}


	free(base32Str);

	return 0;
}


/*
 *  Devolped on Fedora Linux using GCC compiler linking -lgmp option:
 *
 B32Mtf was a half second idea that took two days to construct while I experienced assistance from a language model A.I. ChatGPT4.
 It is the dawn of home-(A.I). and much like those monochrome monitors and eight bit computers of my youth, there is a gravity I am willing to fall towards with the Age.

 So this is a program that has taken the Million digit file and encoded it, then reads that output in and decodes and writes the decoded out.
 I verified that it decoded the whole 415,241 bytes of the milliondigitfile and so this is good enough to share with other Information Alchemists on https://theoddduckdin.freeforums.net

 What it does:

 It reads a binary file (any file is binary) and expresses that binry number; that any binary file is, in base 32 symbols as defined by GMP and not the standard base32 alphabet as defined in RFC 4648. 
 These symbols are then "moved to front" either in the ENCODE direction or in reverse in the DECODE direction.
 A new (Integer) file is written. 
 It is my belief that "any file any way." Meaning any file can be decoded or encoded even if it wasn't prcessed before.
 To that I add that any file is in a bi-directional (bijective) sequence that I wouldn't be surprised if it cycles back around to the original file for either direction.
 I haven't run this to prove that so there is a challenge if you have the time.

 It it:

 It is a Codec. One can encode and re-encode over and over again because each new file in either direction is a new integer in a (most likely) cycle of integers.
 In this most simple example of construct there may be some Information Alchemy. Maybe one of the "integers" will compress so there is a possible "gold strike" in data-mining in respect of Mark Nelson's (https://marknelson.us/posts/2006/06/20/million-digit-challenge.html).

 In the least context; it is an example of this type of Codec. Using text of the number base and applying the Move to Front algorithm.
 It is functional as far as I know as of October 2023

//==========================================
// ChatGPT processed the above comment below per my request :)
//


B32Mtf: A Journey into Information Alchemy

B32Mtf started as a fleeting idea, blossoming into a two-day adventure with the support of ChatGPT4, a language model AI. As we stand at the cusp of the home-AI era, I feel the same allure that I once felt for monochrome monitors and 8-bit computers. It's a gravitational pull that's too intriguing to resist.

Forum for Information Alchemists:
Feel free to share your thoughts and experimentations on The Odd Duck Din Forum.

What Does B32Mtf Do?

The program reads a binary file and converts it into a base-32 representation using GMP's symbols, diverging from the standard base-32 alphabet defined in RFC 4648. These symbols are then re-arranged using a "Move to Front" algorithm, creating an encoded or decoded file.

A Hypothetical Challenge:
Though untested, I believe the sequence of integers produced could cycle back to the original file, presenting an intriguing problem for the curious to solve.

The Essence of B32Mtf:
At its core, B32Mtf is a codec. Each encoding or decoding iteration produces a new integer, possibly within a cycling sequence. In this simple framework lies the prospect of Information Alchemy—perhaps one of these iterations could lead to data compression, akin to striking gold in the realm of data mining.

A Functioning Example:
B32Mtf is more than just an idea; it's functional. Though some debugging scaffolds remain, they serve as an entry point for those interested in further development.

I hope this captures both the technical details and the spirit of your work, Ernst.

---
FINAL: Isn't that funny?!
A.I. Humor of the day.


*/
