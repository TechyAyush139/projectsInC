/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }

    // remember filenames
    int multiplier =  atoi(argv[1]); 
    char* infile = argv[2];
    char* outfile = argv[3];
    
    if (multiplier < 1 || multiplier > 100)
        {
            printf("Factor must be in the range [1-100]\n");
            return 1;
    }
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    BITMAPFILEHEADER outbf;
    BITMAPINFOHEADER outbi;
    
    outbf = bf;
    outbi = bi;
    outbi.biWidth = bi.biWidth * multiplier;
    outbi.biHeight = bi.biHeight * multiplier;
    
     // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    // determine padding for out scanlines
    int outPadding =  (4 - (outbi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;


    outbf.bfSize = 54 + outbi.biWidth * abs(outbi.biHeight) * 3 + abs(outbi.biHeight) * outPadding;
    outbi.biSizeImage = ((((outbi.biWidth * outbi.biBitCount) + 31) & ~31) / 8) * abs(outbi.biHeight);
    
    // write outfile's BITMAPFILEHEADER
		fwrite(&outbf, sizeof(BITMAPFILEHEADER), 1, outptr);

		// write outfile's BITMAPINFOHEADER
		fwrite(&outbi, sizeof(BITMAPINFOHEADER), 1, outptr);

		for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
		{
				// Write each line by the amount enter by the user
				for(int n = 0; n < multiplier; n++)
				{
						// scan every line for each pixel
						for (int j = 0; j < bi.biWidth; j++)
						{
								
								RGBTRIPLE triple;

								// read the pixels from infile
								fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

								// write the pixels to outfile
								for(int m = 0; m < multiplier; m++) 
								{
										fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
								}
						}

						// skip over padding, if any
						fseek(inptr, padding, SEEK_CUR);

						// then add it back (to demonstrate how)
						for (int k = 0; k <outPadding; k++) 
						{
						    fputc(0x00, outptr);
						}
								

						fseek(inptr, -(bi.biWidth * 3 + padding ), SEEK_CUR);

				}
				fseek(inptr, bi.biWidth*3+padding, SEEK_CUR);
		}

		// close infile
		fclose(inptr);

		// close outfile
		fclose(outptr);

		// that's all folks
		return 0;
}