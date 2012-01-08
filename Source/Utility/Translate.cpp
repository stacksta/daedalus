/*
Copyright (C) 2012 Salvy6735

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "stdafx.h"

#include "Translate.h"
#include "IO.h"

#include <string>
//*****************************************************************************
//
//*****************************************************************************
struct pTranslate
{
	u32		id;				// ID that corresponds to string
	char	*translated;	// Translated string
};

struct pLanguage
{
	std::string dir;
	std::string name;
};

pTranslate text[148];
pLanguage  language[ 6 ];
u32		   gNumLanguage =0;
//*****************************************************************************
//
//*****************************************************************************
const char * Translate(u32 id, const char *original)
{
	for( u32 i=0; i < ARRAYSIZE(text); i++ )
	{
		if( text[i].id == id )
		{
			if( text[i].translated )
				return text[i].translated;
			else 
				return original;
		}
	}
	return original;
}

//*****************************************************************************
//
//*****************************************************************************
void Translate_Clear()
{
	memset(text, 0, sizeof(text));
}

//*****************************************************************************
//
//*****************************************************************************
void	Translate_Load( const char * p_dir )
{
	gNumLanguage		= 0;
	IO::FindHandleT		find_handle;
	IO::FindDataT		find_data;

	if(IO::FindFileOpen( p_dir, &find_handle, find_data ))
	{
		do
		{	
			char * filename( find_data.Name );
			char * last_period( strrchr( filename, '.' ) );
			if(last_period != NULL)
			{
				if( _strcmpi(last_period, ".lng") == 0 )
				{
					gNumLanguage++;
					language[gNumLanguage].dir = filename;

					IO::Path::RemoveExtension( filename );
					language[gNumLanguage].name = filename;
					
				}
			}
		}
		while(IO::FindFileNext( find_handle, find_data ));

		IO::FindFileClose( find_handle );
	}
}

//*****************************************************************************
//
//*****************************************************************************
const char * Translate_Language(u32 idx)
{
	if(idx != 0 )
		return language[idx].name.c_str();
	else
		return "English";	
}

//*****************************************************************************
//
//*****************************************************************************
const char * Translate_Directory(u32 idx)
{
	return language[idx].dir.c_str();
}

//*****************************************************************************
//
//*****************************************************************************
bool Translate_Read(char *file)
{
	char line[1024];
	char path[MAX_PATH];
	char *string;
	FILE *stream;

	u32 count = 0;
	u32 id	  = 0;

	strcpy(path, "Translation/");
	strcat(path, file);

	stream = fopen(path,"r");
	if( stream == NULL )
	{
		Translate_Clear();	// Reset back to default language
		return false;
	}

	while( fgets(line, 1023, stream) )
	{
		IO::Path::Tidy(line);			// Strip spaces from end of lines

		// Handle comments
		if (line[0] == '/')
			continue;

		id = atoi(line);
		string = strchr(line,',');
		if( string != NULL )
		{
			string++;
			if( count <= ARRAYSIZE(text) )
			{
				// Write translated id/strings to array
				text[count].id = id;
				text[count].translated = (char*)malloc(strlen(string)+2);
				if(text[count].translated == NULL)
				{
					printf("Cannot allocate memory to load translated strings");
					return false;
				}
				strcpy(text[count].translated, string);
				count++;
			}
		}
	}
	fclose(stream);
	return true;
}