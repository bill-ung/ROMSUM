///////////////////////////////////////////////////////////////////////////////
// ROMSUM - Written by Bill Ung <bill@ufopinball.com>, Copyright (C) 1996-2018
// 
// A command-line utility produced for the pinball community to calculate
// the checksum of a given pinball ROM file.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////

#include <direct.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include <stdbool.h>

// Command-line options
#define ROMSUM_OPT_NONE         0x0000
#define ROMSUM_OPT_VERBOSE      0x0001
#define ROMSUM_OPT_WILDCARD     0x0002
#define ROMSUM_OPT_CSV_OUTPUT   0x0004

// General definitions
#define OUTPUT_LEN	512						// Maximum buffer length for output strings
#define LABEL_LEN	40						// Maximum buffer length for output labels
#define BUF_LEN		4096					// Maximum buffer length to support 0x1000 (4096) bytes

// Global variables
unsigned char		buf[BUF_LEN];			// Buffer used to calculate a checksum on a specific 0x1000 (4096) byte page
unsigned long		lsum;					// The full checksum for a given file

// Function prototypes
void main(int argc, char *argv[]);		// Handles command-line input, including options and wildcards
void romsum_file(char * file, int opt);	// File operations and EPROM sizing
void romsum(int page, int opt);			// Checksum calculation code
void print_help(bool help);				// Print "Help" as requested
//void print_output( char * in );				// Print ROMSUM output with proper formatting

// ----------------------------------------------------------------------------
// Function:   main()
// Purpose:    Handles command-line input, including options and wildcards
// Parameters: 
//   int argc     - Number of command-line arguments
//   char *argv[] - Array of command-line arguments
// Returns:    void
// ----------------------------------------------------------------------------
void main(int argc, char *argv[])
{
	int					opt;
	struct _finddata_t	fileinfo;
	long				find_h;
	bool				bFirst = true;

	// Initialize
	opt = ROMSUM_OPT_NONE;

	// Are the parameters right?
	if (argc < 2 || argc > 3)
	{
		// No, print help & exit
		print_help(true);
		return;
	}

	if (argc == 2)
	{
		// Has help been requested?
		if (strcmp(argv[1], "/H") == 0 ||
			strcmp(argv[1], "/h") == 0 ||
			strcmp(argv[1], "/?") == 0 ||
			strcmp(argv[1], "-H") == 0 ||
			strcmp(argv[1], "-h") == 0 ||
			strcmp(argv[1], "-?") == 0
			)
		{
			// Print help & exit
			print_help(true);
			return;
		}
	}

	// Parse parameters
	if (argc == 3)
	{
		// Is verbose mode checked?
		if (strcmp(argv[2], "/v") == 0 ||
			strcmp(argv[2], "/V") == 0 ||
			strcmp(argv[2], "-v") == 0 ||
			strcmp(argv[2], "-V") == 0
			)
		{
			// Verbose mode
			opt |= ROMSUM_OPT_VERBOSE;
		}
	}
	if (argc == 3)
	{
		// Is CSV mode checked?
		if (strcmp(argv[2], "/c") == 0 ||
			strcmp(argv[2], "/C") == 0 ||
			strcmp(argv[2], "-c") == 0 ||
			strcmp(argv[2], "-C") == 0
			)
		{
			// CSV mode
			opt |= ROMSUM_OPT_CSV_OUTPUT;
		}
	}

	// Validate input
	if ((opt & ROMSUM_OPT_VERBOSE) != 0 && (opt & ROMSUM_OPT_CSV_OUTPUT) != 0)
	{
		// Cannot run CSV mode at the same time with Verbose mode, print help and exit
		print_help(true);
		return;
	}

    // Print banner
	if ((opt & ROMSUM_OPT_CSV_OUTPUT) == 0)
		print_help( false );

    // Set wildcard flag
    if ((strstr (argv[1], "*") != 0) || (strstr (argv[1], "?") != 0))
        opt |= ROMSUM_OPT_WILDCARD;

    // Process
	find_h = _findfirst( argv[1], &fileinfo );
    if (find_h != -1)
    {
        // Do the first file
		if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
		{
			romsum_file(fileinfo.name, opt);
			bFirst = false;
		}

        // Loop the rest of them
        while (_findnext (find_h, &fileinfo) == 0)
		{
			if( opt & ROMSUM_OPT_VERBOSE && !bFirst)
				printf( "\n" );
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
			{
				romsum_file(fileinfo.name, opt);
				bFirst = false;
			}
		}
    }
    else
    {
		// Unable to locate requested file(s)
        if (opt & ROMSUM_OPT_WILDCARD)
            printf( "Error, no matching files found.\n" );
        else
            printf( "Error opening %s.\n", argv[1] );
    }

	return;
} // main()

// ----------------------------------------------------------------------------
// Function:   romsum_file()
// Purpose:    File operations and EPROM sizing
// Parameters:
//   char *file - Input filename
//   int opt    - Verbose output option
// Returns:    void
// ----------------------------------------------------------------------------
void romsum_file( char *file, int opt )
{
	int             fd;
    unsigned long   len;
    int             pages;
    int             ii;
	char			cwd[OUTPUT_LEN];
	char			dir[OUTPUT_LEN];
    char            out[OUTPUT_LEN];
    char            romsiz[LABEL_LEN];
    char            file_out[OUTPUT_LEN];
	short			ssum = 0;
	char			zeros[] = "00000000";
	char			stmp[LABEL_LEN], ltmp[LABEL_LEN];

    // Open file
    _sopen_s(&fd, file, _O_RDONLY | _O_BINARY, _SH_DENYWR, 0);
    if (fd == -1)
    {
        // Report error
        printf( "Error opening %s.\n", file );
    }
	else
	{
		// Get some file information
		len = _filelength(fd);

		pages = (int)(len / 0x1000L);

		// Verbose mode sum calculation
		if ((opt & ROMSUM_OPT_VERBOSE) && len <= 0x8000 && !(len % 0x1000))
		{
			// Loop through pages
			for (ii = 0; ii < pages; ii++)
			{
				// Do page 0
				_lseek(fd, (ii * 0x1000L), SEEK_SET);
				_read(fd, buf, 0x1000);
				if (opt & ROMSUM_OPT_WILDCARD && !ii)
				{
					// Add a file header
					if ((opt & ROMSUM_OPT_CSV_OUTPUT) == 0)
						printf("File %s:\n\n", file);
				}
				romsum(ii, opt);
			}
			if ((opt & ROMSUM_OPT_CSV_OUTPUT) == 0)
				printf("\n");
		}

		// Sum the whole block
		lsum = 0L;

		// Must have at least one page
		if (!pages)
			pages = 1;

		// Loop through pages
		for (ii = 0; ii < pages; ii++)
		{
			// Empty buffer on first loop
			if (!ii)
				memset(buf, '\0', BUF_LEN);

			// Do each page
			_lseek(fd, (ii * 0x1000L), SEEK_SET);
			_read(fd, buf, 0x1000);
			romsum(-1, opt);
		}

		// Define rom size
		switch (len)
		{
		case 2097152:
			strcpy_s(romsiz, LABEL_LEN, " [ATMEL AT49BV1614 Flash Memory]");
			break;

		case 1048576:
			strcpy_s(romsiz, LABEL_LEN, " [27c801]");
			break;

		case 524288:
			strcpy_s(romsiz, LABEL_LEN, " [27c040 or 27c4001]");
			break;

		case 262144:
			strcpy_s(romsiz, LABEL_LEN, " [27c020]");
			break;

		case 131072:
			strcpy_s(romsiz, LABEL_LEN, " [27c010]");
			break;

		case 65536:
			strcpy_s(romsiz, LABEL_LEN, " [27512]");
			break;

		case 32768:
			strcpy_s(romsiz, LABEL_LEN, " [27256]");
			break;

		case 16384:
			strcpy_s(romsiz, LABEL_LEN, " [27128]");
			break;

		case 8192:
			strcpy_s(romsiz, LABEL_LEN, " [2764]");
			break;

		case 4096:
			strcpy_s(romsiz, LABEL_LEN, " [2732]");
			break;

		case 2048:
			strcpy_s(romsiz, LABEL_LEN, " [2716]");
			break;

		case 1024:
			strcpy_s(romsiz, LABEL_LEN, " [2708]");
			break;

		case 512:
			strcpy_s(romsiz, LABEL_LEN, " [2704]");
			break;

		case 32:
			strcpy_s(romsiz, LABEL_LEN, " [82S123 PROM]");
			break;

		default:
			sprintf_s(romsiz, LABEL_LEN, " [%lu bytes]", len);
			break;
		} // switch

		// Pad filename with spaces
		strcpy_s(file_out, OUTPUT_LEN, file);

		// Knock off the upper 16-bits and pad with zeroes
		ssum = (short)(lsum & 0x0000FFFFL);
		sprintf_s(stmp, LABEL_LEN, "%s%X", zeros, ssum);
		sprintf_s(ltmp, LABEL_LEN, "%s%lX", zeros, lsum);

		// Report
		if ((opt & ROMSUM_OPT_CSV_OUTPUT) == 0)
		{
			sprintf_s(out, OUTPUT_LEN, "File %s == 0x%s (0x%s)", file_out, &stmp[strlen(stmp) - 4], &ltmp[strlen(ltmp) - 8]);
			strcat_s(out, OUTPUT_LEN, romsiz);
			strcat_s(out, OUTPUT_LEN, "\n");
			printf(out);
		}
		else
		{
			_getcwd(cwd, OUTPUT_LEN);
			for (ii = strlen(cwd) - 1; ii > 0; ii--)
			{
				if (cwd[ii] == '\\')
				{
					strcpy_s(dir, OUTPUT_LEN, &cwd[ii+1]);
					break;
				}
			}
			sprintf_s(out, OUTPUT_LEN, "\"%s\",\"%d\",\"%s\",\"%s\"\n", file_out, len, &stmp[strlen(stmp) - 4], dir);
			printf(out);
		}

        // Close file
        _close(fd);
    }

	return;
} // romsum_file()

// ----------------------------------------------------------------------------
// Function:   romsum()
// Purpose:    Checksum calculation code
// Parameters:
//   int page - 0-based page number for calculation, or -1 for entire file
//   int opt  - Verbose output option
// Returns:    void
// ----------------------------------------------------------------------------
void romsum( int page, int opt )
{
    int             ii;
    unsigned long   sum;
    char            out[OUTPUT_LEN];

	// Check verbose mode or whole file calculation
    if (page != -1)
    {
        // Initialize verbose mode
        sum = 0L;

        // Sum page
        for ( ii = 0; ii < 0x1000; ii++ )
            sum += (unsigned long)buf[ii];
    }
    else
    {
        // Sum file
        for ( ii = 0; ii < 0x1000; ii++ )
            lsum += (unsigned long)buf[ii];
    }

    // Knock off upper 16-bits
    sum = sum & 0x0000FFFFL;

    // Report sum
    if (page != -1)
    {
		if (sum < 0x0010L)
			sprintf_s(out, OUTPUT_LEN, "  Page %d == 0x000%X\n", page, sum);
		else if (sum < 0x0100L)
			sprintf_s(out, OUTPUT_LEN, "  Page %d == 0x00%X\n", page, sum);
		else if (sum < 0x1000L)
			sprintf_s(out, OUTPUT_LEN, "  Page %d == 0x0%X\n", page, sum);
		else
			sprintf_s(out, OUTPUT_LEN, "  Page %d == 0x%X\n", page, sum);
		printf(out);
    }

    return;
} // romsum()

// ----------------------------------------------------------------------------
// Function:   print_help()
// Purpose:    Prints "Help" as requested
// Parameters: None
// Returns:    void
// ----------------------------------------------------------------------------
void print_help( bool help )
{
	// Print standard banner
    printf( "\n"                                                                      );
    printf( "ROMSUM32 Version 3.2 (2018-01-17)\n"                                     );
    printf( "Copyright (C) 1995-2018, Bill Ung <bill@ufopinball.com>\n"               );
    printf( "All Rights Reserved\n"                                                   );
    printf( "\n"                                                                      );

	// Print complete help as necessary
	if ( !help )
		return;
    printf( "Usage:\n"                                                                );
    printf( "\n"                                                                      );
    printf( "  ROMSUM <filename> [/v]\n"                                              );
    printf( "\n"                                                                      );
    printf( "Wildcards are allowed in the filename.\n"                                );
    printf( "\n"                                                                      );
    printf( "The /v parameter specifies verbose mode which performs individual\n"     );
    printf( "checksums each 0x1000 byte page, for any ROM that is 32768 (0x8000)\n"   );
    printf( "bytes or less in length, and can be evenly divided into 0x1000 byte\n"   );
    printf( "pages.\n"                                                                );
    printf( "\n"                                                                      );
    printf( "This is for use on Williams System 11{A,B,C} games that calculate\n"     );
    printf( "checksums for each 0x1000 byte page.\n"                                  );

    return;
} // print_help()

/*// ----------------------------------------------------------------------------
// Function:   print_output()
// Purpose:    Print ROMSUM output with proper formatting
// Parameters:
//   char *in - Checksum string to format for output
// Returns:    void
// ----------------------------------------------------------------------------
void print_output( char *in )
{
    bool            xfound = false;
    unsigned short  ii;
    char            out[OUTPUT_LEN];

	printf(in);
/*
    for (ii = 0; ii < strlen( in ); ii++)
    {
        if (xfound == true && in[ii] >= 'a' && in[ii] <= 'f')
            out[ii] = in[ii] - 32;  // turn to upper-case
        else
            out[ii] = in[ii];

        if (in[ii] == 'x')
            xfound = true;  // turn it on at start of hex value

        if (in[ii] == ' ')
            xfound = false; // turn it off at end of hex value
    }
    out[ii] = '\n';
    out[ii] = '\0';

    printf( out );

    return;
} // print_output()
*/
// ----------------------------------------------------------------------------
