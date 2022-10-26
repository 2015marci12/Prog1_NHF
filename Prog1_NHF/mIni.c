#include "mIni.h"

static char* skip_whitespace(char* s)
{
    while (*s != '\0' && isspace(*s))
    {
        s++;
    }
    return s;
}

static char* skipTo(char* s, const char* delims)
{
    while (*s != '\0' && delims != strchr(delims, *s)) 
    {
        s++;
    }
    return s;
}

static void strFillNull(char* s)
{
    while (*s != '\0')
    {
        *s = '\0';
        s++;
    }
}

static void strcpy_term(char* dst, size_t buff, const char* src) 
{
    size_t advance;
    for (advance = 0; (advance < (buff - 1)) && ('\0' != src[advance]); advance++)
    {
        dst[advance] = src[advance];
    }
    dst[advance] = '\0';
}

static void strTrimEnd(char* s) 
{
    char* p = s + strlen(s);
    while (p > s && isspace(*--p))
    {
        *p = '\0';
    };
}

int mIni_Parse(FILE* file, mIni_Handler_t handler, const void* userdata)
{
    char section[M_INI_MAX_SECTION_LEN] = { '\0' };
    char id[M_INI_MAX_ID_LEN] = { '\0' };
    char line[M_INI_LINE] = { '\0' };

    //Read line
    while (fgets(line, M_INI_LINE, file) != NULL)
    {
        //strip off potential end of line comment.
        char* comment = line;
        while (*comment && comment < &line[M_INI_LINE])
        {
            comment = skipTo(line, ";#");
            char* commentprev = comment - 1;
            //only if the comment is preceeded by a space or it is at the beginning of the line.
            if ((commentprev < line) || (commentprev >= line && isspace(*commentprev)))
                strFillNull(comment);
            else
                comment++;
        }   

        //Skip whitespace at the beginning.
        char* linebegin = skip_whitespace(line);
        //if end of line, then proceed to the next line.
        if ('\0' == *linebegin) continue;

        //Check if there is a section declaration on the line.
        if (*linebegin == '[') 
        {
            char* sectionbegin = linebegin + 1;
            char* sectionend = skipTo(linebegin, "]");

            if ('\0' == *sectionend) return -1; //Error: section end not found on line.

            //Set end so strcpy can work.
            *sectionend = '\0';

            //Copy section name.
            strcpy_term(section, M_INI_MAX_SECTION_LEN, sectionbegin);
        }
        else 
        {
            //Check if there is a key declaration here.
            char* keyvaluebegin = skipTo(linebegin, "=:");
            if ('\0' != *keyvaluebegin)
            {
                //Extract key and increment to the beginning of the value.
                *keyvaluebegin = '\0'; //Mark as end for strcpy.
                strcpy_term(id, M_INI_MAX_ID_LEN, linebegin); //Copy id.
                strTrimEnd(id); //Trim spaces off of the token end.

                keyvaluebegin = skip_whitespace(++keyvaluebegin);
            }
            //The whole line is a value.
            else keyvaluebegin = linebegin;

            //Trim the end of the values.
            strTrimEnd(keyvaluebegin);

            //Call handler.
            int handlerret = (handler)(userdata, section, id, keyvaluebegin);

            //if handler returns an error then exit.
            if (handlerret < 0) return handlerret;
        }        
    }
    return 0;
}

int mIni_File(const char* file, mIni_Handler_t handler, const void* userdata)
{
    FILE* f = NULL;
    if (0 != fopen_s(&f, file, "r")) 
    {
        ERROR("Cannot open ini file %s\n", file);
        return -1;
    }
    int ret = mIni_Parse(f, handler, userdata);
    fclose(f);
    return ret;
}
