
#ifdef PRE_WIN32
	#include <windows.h>

	DWORD GetMSTime()
	{
		return timeGetTime();
	}
#endif



//#include "BDefs.h"
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include "processing.h"	


static int g_CurProgressBar=-1;
static int g_CurNumStars=0;

static char *g_ProgressBarNames[] = 
{
	"Generating BSP    ",
	"Lighting          ",
	"Generating Hulls  ",
	"Full Optimization ",
	"Setting Up        "
};



// All the system-dependent functions.
void SysDrawStatusText(char *pMsg)
{
	printf("%s\r\n", pMsg);
}



// Status stuff.
void StartProgressBar(int progressBar)
{
	printf("\n");
	g_CurProgressBar = progressBar;
	g_CurNumStars = 0;
	SetCurrentProgressBar(0.0f);
}

void EndProgressBar(int progressBar, int bHide)
{
	SetCurrentProgressBar(1.0f);
	g_CurProgressBar = -1;
	printf("\n");
}

void SetCurrentProgressBar(float percent)
{
	int		i, nStars, nMaxStars=50;
	char	str[256], *pEnd;

	if(g_CurProgressBar >= 0 && g_CurProgressBar <= 4)
	{
		sprintf(str, "\r%s [", g_ProgressBarNames[g_CurProgressBar]);
		pEnd = str + strlen(str);
		
		nStars = (int)(percent * (float)nMaxStars);
		if(nStars == g_CurNumStars)
			return;
		else
			g_CurNumStars = nStars;

		for(i=0; i < nStars; i++)
			*pEnd++ = '*';

		for(i=0; i < (nMaxStars-nStars); i++)
			*pEnd++ = ' ';

		*pEnd++ = ']';
		*pEnd = 0;

		printf(str);
	}
}


int AskQuestion(char *pQuestion, int type)
{
	char	key;

	if(type&QUES_YES && type&QUES_NO && type&QUES_CANCEL)
	{
		printf("%s (Yes/No/Cancel) ", pQuestion);
		while(1)
		{
			key = toupper(getch());
			if(key == 'Y')
				return QUES_YES;
			else if(key == 'N')
				return QUES_NO;
			else if(key == 'C')
				return QUES_CANCEL;
		}
	}
	else
	{
		printf("%s (Yes/No) ", pQuestion);
		while(1)
		{
			key = toupper(getch());
			if(key == 'Y')
				return QUES_YES;
			else if(key == 'N')
				return QUES_NO;
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{
	CProcessorGlobs globs;

	printf("--- LithTech World Processor ---\n\n");

	if(argc <= 1)
	{
		printf("No world file specified.\n");
		return 0;
	}

	memset(&globs, 0, sizeof(globs));
	strcpy(globs.m_InputFile, argv[1]);
	
	//set up the output name to match the input name, but change the extension to
	//dat
	uint32 nCurrInChar		= 0;
	uint32 nExtensionPos	= 0;

	//copy the string, and identify where the extension starts
	while(globs.m_InputFile[nCurrInChar] != '\0')
	{
		globs.m_OutputFile[nCurrInChar] = globs.m_InputFile[nCurrInChar];

		//see if we hit the extension
		if(globs.m_InputFile[nCurrInChar] == '.')
		{
			nExtensionPos = nCurrInChar;
		}
		nCurrInChar++;
	}
	
	//now switch the extension
	if(nCurrInChar > 0)
	{
		strcpy(&(g_Globs.m_OutputFile[nExtensionPos]), ".dat");
	}	

	globs.m_Argc = argc-2;
	globs.m_Argv = &argv[2];
	
	ProcessOptions(&globs);
	DoProcessing(&globs);
	
	printf("\nPress a key to exit");
	while(!kbhit());
	printf("\n");
	return 1;
}



