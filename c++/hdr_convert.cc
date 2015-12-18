/*
	Copyright (C) 2010  Andrew Cotter

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/**
	\file hdr_convert.cc
*/




#include "headers.hh"




//============================================================================
//    main function
//============================================================================


int main( int argc, char const* argv[] ) {

	int code = EXIT_SUCCESS;

	int help = 0;
	char* input  = NULL;
	char* output = NULL;
	double stops = std::numeric_limits< double >::quiet_NaN();

	struct poptOption options[] = {
		{ "help",   'h',  POPT_ARG_NONE,   &help,   0, "show help",                     NULL        },
		{ "input",  'i',  POPT_ARG_STRING, &input,  0, "input file (EXR)",              "INPUT"     },
		{ "output", 'o',  POPT_ARG_STRING, &output, 0, "output file",                   "OUTPUT"    },
		{ "stops",  's',  POPT_ARG_DOUBLE, &stops,  0, "stops by which to over-expose", "FLOAT(=0)" },
		{ NULL,     '\0', 0,               NULL,    0, NULL,                            NULL        }
	};

	poptContext context = poptGetContext( NULL, argc, argv, options, 0 );

	try {

		for ( ; ; ) {

			int const code = poptGetNextOpt( context );
			if ( code == -1 )
				break;
			else if ( code < -1 )
				throw std::runtime_error( poptStrerror( code ) );
		}
		if ( poptGetArg( context ) != NULL )    // check for positional arguments
			throw std::runtime_error( "unknown option" );

		if ( help ) {

			std::cout <<
				"Converts an HDR input file, in EXR format, into any image format supported by" << std::endl <<
				"ImageMagick, and performs gamma-correction. The brightest possible value in the" << std::endl <<
				"input image is taken to be 1.0 by default (files generated by hdr_create and" << std::endl <<
				"hdr_squish will observe this convention), but this may be adjusted by changing" << std::endl <<
				"the stops parameter, which is the number of stops by which the output image" << std::endl <<
				"should be over-exposed (or under-exposed, if negative)." << std::endl <<
				std::endl
			;
			poptPrintHelp( context, stdout, 0 );
		}
		else {

			if ( input == NULL )
				throw std::runtime_error( "input file not specified" );
			if ( output == NULL )
				throw std::runtime_error( "output file not specified" );
			if ( std::isnan( stops ) )
				stops = 0;

			std::cout << "Loading HDR image" << std::endl;
			unsigned int rows, columns;
			SharedArray< float > image = LoadEXR( input, rows, columns );

			std::cout << "Saving image to " << output << std::endl;
			SaveNonlinearImage( output, image, rows, columns, stops );
		}
	}
	catch( std::exception& exception ) {

		std::cerr << "Error: " << exception.what() << std::endl << std::endl;
		poptPrintHelp( context, stderr, 0 );
		code = EXIT_FAILURE;
	}

	return code;
}
