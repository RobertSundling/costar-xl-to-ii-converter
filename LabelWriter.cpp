
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_ESC (27)
#define ASCII_ETB (23)
#define ASCII_SYN (22)


int bytesPerLine = 56;
int dotTab = 0;
int lineTab = 0;

int GetByte ( )
{
	int c = fgetc(stdin);
	//printf("%02xh ", c);
	return c;
}

void PutByte ( int i )
{
	fputc(i, stdout);
}

void PutCommand ( int i )
{
	PutByte(ASCII_ESC);
	PutByte(i);
}

char s2[256];
int byteOffset = 0;
int bitOffset = 7;

int lastBytesToSkip = -1;
int lastNumBytes = -1;

void StartLine ( )
{
	memset(s2, 0, 256);
	int bits = dotTab * 8;
//	bits = (bits * 136) / 203;

	byteOffset = bits / 8;
	bitOffset = (bits % 8);

	//fprintf(stderr, "Startline (byte offset=%d)", byteOffset);
}


void SkipBit ( )
{
	bitOffset++;
	if ( bitOffset == 8 )
	{
		bitOffset = 0;
		byteOffset++;
	}
}

void SetBit ( char* array = s2 )
{
	array[byteOffset] |= (1 << bitOffset);
	SkipBit();
}


int ReverseByte ( int j )
{
	int r = 0;

	for ( int i = 0; i <= 7; i++ )
	{
		if ( j & (1 << i) )
			r |= (1 << (7-i));
	}
	return r;
}


void EndLine ( )
{
	// Resample the line from a 203 dpi line into our 136 dpi line

	char s[256];
	memset(s, 0, 256);

	int bytes = byteOffset;

	byteOffset = 0;
	bitOffset = 7;

	int bitCount = 0;
	int lab = -1;

	for ( int byte = 0; byte < bytes; byte++ )
	{
		for ( int bit = 0; bit <= 7; bit++ )
		{
			int absBit = byte * 8 + bit;
			absBit = (absBit * 136) / 203;

			if ( absBit != lab )
			{
				int by = absBit / 8;
				int bi = absBit % 8;

				if ( s2[byte] & (1 << bit) )
				{
					s[by] |= (1 << bi);
				}

				byteOffset = by + 1;
				lab = absBit;
			}
		}
	}

	
/*	for ( int t = 0; t <= byteOffset; t++ )
	{
		for ( int tt = 0; tt <= 7; tt++ )
		{
			if ( s[t] & (1 << tt) )
				fprintf(stderr, "X");
			else
				fprintf(stderr, " ");
		}
	}
	fprintf(stderr, "\n");*/

	for ( ; byteOffset > 0; byteOffset-- )
		if ( s[byteOffset-1] != 0 ) break;

	for ( int bytesToSkip = 0; bytesToSkip < byteOffset; bytesToSkip++ )
		if ( s[bytesToSkip] != 0 ) break;

	if ( bytesToSkip >= byteOffset || byteOffset <= 0 )
	{
		//fprintf(stderr, "Skipping line\n");

		PutCommand('f');
		PutByte('1');
		PutByte(1);
		fprintf(stderr, "\n");
		return;
	}

	//fprintf(stderr, "Skipping %02d bytes, ", bytesToSkip);

	if ( bytesToSkip != lastBytesToSkip )
	{
		PutCommand('B');
		PutByte(bytesToSkip);
		lastBytesToSkip = bytesToSkip;
	}

	int numBytes = byteOffset - bytesToSkip;

	if ( numBytes != lastNumBytes )
	{
		PutCommand('D');
		PutByte(numBytes);
		lastNumBytes = numBytes;
	}

	//fprintf(stderr, "Writing %d bytes\n", numBytes);
		
	// Write the uncompressed version
	PutByte(ASCII_SYN);
	for ( int i = 0; i < numBytes; i++ )
		PutByte(ReverseByte(s[bytesToSkip + i]));
}

void main ( void )
{
	_setmode( _fileno( stdin ), _O_BINARY );
	_setmode( _fileno( stdout ), _O_BINARY );
   
	while ( !feof(stdin) ) 
	{
		int c = GetByte();
		
		switch ( c )
		{
		case ASCII_ESC:
			c = GetByte();

			switch ( c )
			{
			case 'B':
				dotTab = GetByte();
				//printf("Dot tab: %d\n", dotTab);
				break;

			case 'Q':
				PutCommand(c);
				c = GetByte();
				PutByte(c);
				c = GetByte();
				PutByte(c);
				//printf("Line tab set\n");
				break;

			// Set dots per line
			case 'D':
				bytesPerLine = GetByte();
				//printf("%d bytes per line\n", bytesPerLine);
				break;

			case 'L':
				PutCommand(c);
				c = GetByte();
				PutByte(c);
				c = GetByte();
				PutByte(c);
				//printf("Label length set\n");
				break;

			case 'E':
				PutCommand(c);
				//printf("Form feed\n");
				break;

			case 'A':
				PutCommand(c);
				//printf("Printer query - return status\n");
				break;

			case 'a':
				PutCommand(c);
				//printf("Printer query - return hardware status\n");
				break;

			case '@':
				PutCommand(c);
				//printf("Reset printer\n");
				break;

			case '*':
				PutCommand(c);
				//printf("Restore default settings\n");
				break;

			case 'R':
				PutCommand(c);
				c = GetByte();
				PutByte(c);
				//printf("Set baud rate\n");
				break;

			case 'W':
				PutCommand(c);
				c = GetByte();
				PutByte(c);
				c = GetByte();
				PutByte(c);
				//printf("Wrap data -- ?\n");
				break;

			case 'f':
				PutCommand(c);
				c = GetByte(); // Must be '1'
				PutByte(c);
				{
					int n = GetByte();
					PutByte(n);

/*					for ( int i = 0; i < n; i++ )
						fprintf(stderr, "\n");*/
					//printf("Skip %d lines\n", n);
				}
						
				break;

			case 'V':
				PutCommand(c);
				//printf("Printer query - version\n");
				break;

			case 'y':
				//printf("High resolution\n");
				break;

			case 'z':
				//printf("Low resolution\n");
				break;

			default:
				//printf("Unknown command\n");
				break;
			}

			break;

		case ASCII_ETB:
			//printf("Compressed data\n");
			{
				//fprintf(stderr, "C ");
				StartLine();
				for ( int x = 0; x < bytesPerLine * 8; )
				{
					int c = GetByte();
					int fill = (c & 0x80) != 0;
					int count = (c & 0x7F) + 1;

					for ( int i = 0; i < count; i++ )
					{
						if ( fill )
						{
							SetBit();
						}
						else
						{
							SkipBit();
						}
					}

					x += count;
				}
				EndLine();

				//printf("Data read\n");
			}

			break;
		case ASCII_SYN:
			//printf("Uncompressed data\n");
			{
				//fprintf(stderr, "U ");
				StartLine();
				for ( int x = 0; x < bytesPerLine; )
				{
					int c = GetByte();

					for ( int b = 7; b >= 0; b-- )
					{
						if ( c & (1 << b) )
							SetBit();
						else
							SkipBit();
					}
					
					x ++;
				}
				EndLine();

				//printf("Data read\n");
			}
			break;

		case EOF:
			break;

		default:
			//printf("Unknown character %c\n", c);
			break;
		}
	}
}