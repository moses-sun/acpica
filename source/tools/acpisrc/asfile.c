
/******************************************************************************
 *
 * Module Name: asfile - Main module for the acpi source processor utility
 *              $Revision: 1.10 $
 *
 *****************************************************************************/

/******************************************************************************
 *
 * 1. Copyright Notice
 *
 * Some or all of this work - Copyright (c) 1999, 2000, 2001, Intel Corp.
 * All rights reserved.
 *
 * 2. License
 *
 * 2.1. This is your license from Intel Corp. under its intellectual property
 * rights.  You may have additional license terms from the party that provided
 * you this software, covering your right to use that party's intellectual
 * property rights.
 *
 * 2.2. Intel grants, free of charge, to any person ("Licensee") obtaining a
 * copy of the source code appearing in this file ("Covered Code") an
 * irrevocable, perpetual, worldwide license under Intel's copyrights in the
 * base code distributed originally by Intel ("Original Intel Code") to copy,
 * make derivatives, distribute, use and display any portion of the Covered
 * Code in any form, with the right to sublicense such rights; and
 *
 * 2.3. Intel grants Licensee a non-exclusive and non-transferable patent
 * license (with the right to sublicense), under only those claims of Intel
 * patents that are infringed by the Original Intel Code, to make, use, sell,
 * offer to sell, and import the Covered Code and derivative works thereof
 * solely to the minimum extent necessary to exercise the above copyright
 * license, and in no event shall the patent license extend to any additions
 * to or modifications of the Original Intel Code.  No other license or right
 * is granted directly or by implication, estoppel or otherwise;
 *
 * The above copyright and patent license is granted only if the following
 * conditions are met:
 *
 * 3. Conditions
 *
 * 3.1. Redistribution of Source with Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification with rights to further distribute source must include
 * the above Copyright Notice, the above License, this list of Conditions,
 * and the following Disclaimer and Export Compliance provision.  In addition,
 * Licensee must cause all Covered Code to which Licensee contributes to
 * contain a file documenting the changes Licensee made to create that Covered
 * Code and the date of any change.  Licensee must include in that file the
 * documentation of any changes made by any predecessor Licensee.  Licensee
 * must include a prominent statement that the modification is derived,
 * directly or indirectly, from Original Intel Code.
 *
 * 3.2. Redistribution of Source with no Rights to Further Distribute Source.
 * Redistribution of source code of any substantial portion of the Covered
 * Code or modification without rights to further distribute source must
 * include the following Disclaimer and Export Compliance provision in the
 * documentation and/or other materials provided with distribution.  In
 * addition, Licensee may not authorize further sublicense of source of any
 * portion of the Covered Code, and must include terms to the effect that the
 * license from Licensee to its licensee is limited to the intellectual
 * property embodied in the software Licensee provides to its licensee, and
 * not to intellectual property embodied in modifications its licensee may
 * make.
 *
 * 3.3. Redistribution of Executable. Redistribution in executable form of any
 * substantial portion of the Covered Code or modification must reproduce the
 * above Copyright Notice, and the following Disclaimer and Export Compliance
 * provision in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3.4. Intel retains all right, title, and interest in and to the Original
 * Intel Code.
 *
 * 3.5. Neither the name Intel nor any other trademark owned or controlled by
 * Intel shall be used in advertising or otherwise to promote the sale, use or
 * other dealings in products derived from or relating to the Covered Code
 * without prior written authorization from Intel.
 *
 * 4. Disclaimer and Export Compliance
 *
 * 4.1. INTEL MAKES NO WARRANTY OF ANY KIND REGARDING ANY SOFTWARE PROVIDED
 * HERE.  ANY SOFTWARE ORIGINATING FROM INTEL OR DERIVED FROM INTEL SOFTWARE
 * IS PROVIDED "AS IS," AND INTEL WILL NOT PROVIDE ANY SUPPORT,  ASSISTANCE,
 * INSTALLATION, TRAINING OR OTHER SERVICES.  INTEL WILL NOT PROVIDE ANY
 * UPDATES, ENHANCEMENTS OR EXTENSIONS.  INTEL SPECIFICALLY DISCLAIMS ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * 4.2. IN NO EVENT SHALL INTEL HAVE ANY LIABILITY TO LICENSEE, ITS LICENSEES
 * OR ANY OTHER THIRD PARTY, FOR ANY LOST PROFITS, LOST DATA, LOSS OF USE OR
 * COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, OR FOR ANY INDIRECT,
 * SPECIAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THIS AGREEMENT, UNDER ANY
 * CAUSE OF ACTION OR THEORY OF LIABILITY, AND IRRESPECTIVE OF WHETHER INTEL
 * HAS ADVANCE NOTICE OF THE POSSIBILITY OF SUCH DAMAGES.  THESE LIMITATIONS
 * SHALL APPLY NOTWITHSTANDING THE FAILURE OF THE ESSENTIAL PURPOSE OF ANY
 * LIMITED REMEDY.
 *
 * 4.3. Licensee shall not export, either directly or indirectly, any of this
 * software or system incorporating such software without first obtaining any
 * required license or other approval from the U. S. Department of Commerce or
 * any other agency or department of the United States Government.  In the
 * event Licensee exports any such software from the United States or
 * re-exports any such software from a foreign destination, Licensee shall
 * ensure that the distribution and export/re-export of the software is in
 * compliance with all laws, regulations, orders, or other restrictions of the
 * U.S. Export Administration Regulations. Licensee agrees that neither it nor
 * any of its subsidiaries will export/re-export any technical data, process,
 * software, or service, directly or indirectly, to any country for which the
 * United States government or any agency thereof requires an export license,
 * other governmental approval, or letter of assurance, without first obtaining
 * such license, approval or letter.
 *
 *****************************************************************************/

#include "acpisrc.h"


/******************************************************************************
 *
 * FUNCTION:    AsProcessTree
 *
 * DESCRIPTION: Process the directory tree.  Files with the extension ".C" and
 *              ".H" are processed as the tree is traversed.
 *
 ******************************************************************************/

NATIVE_INT
AsProcessTree (
    ACPI_CONVERSION_TABLE   *ConversionTable,
    char                    *SourcePath,
    char                    *TargetPath)
{
    struct _finddata_t      FindInfo;
    long                    FindHandle;
    char                    *FileSpec;
    char                    *SourceDirPath;
    char                    *TargetDirPath;
    int                     MaxPathLength;


    MaxPathLength = max (strlen (SourcePath), strlen (TargetPath));

    if (!(ConversionTable->Flags & FLG_NO_FILE_OUTPUT))
    {
        if (ConversionTable->Flags & FLG_LOWERCASE_DIRNAMES)
        {
            strlwr (TargetPath);
        }

        VERBOSE_PRINT (("Creating Directory \"%s\"\n", TargetPath));
        if (mkdir (TargetPath))
        {
            if (errno != EEXIST)
            {
                printf ("Could not create target directory\n");
                return -1;
            }
        }
    }

    /* Do the C source files */

    FileSpec = calloc (strlen (SourcePath) + 5, 1);
    if (!FileSpec)
    {
        printf ("Could not allocate buffer for wildcard pathname\n");
        return -1;
    }

    strcpy (FileSpec, SourcePath);
    strcat (FileSpec, "/*.c");

    VERBOSE_PRINT (("Checking for C source files in path \"%s\"\n", FileSpec));

    FindHandle = _findfirst (FileSpec, &FindInfo);
    if (FindHandle != -1)
    {
        VERBOSE_PRINT (("File: %s\n", FindInfo.name));
        AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_SOURCE);

        while (_findnext (FindHandle, &FindInfo) == 0)
        {
            VERBOSE_PRINT (("File: %s\n", FindInfo.name));
            AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_SOURCE);
       }

        _findclose (FindHandle);
    }


    /* Do the C header files */

    strcpy (FileSpec, SourcePath);
    strcat (FileSpec, "/*.h");

    VERBOSE_PRINT (("Checking for C header files in path \"%s\"\n", FileSpec));

    FindHandle = _findfirst (FileSpec, &FindInfo);
    if (FindHandle != -1)
    {
        VERBOSE_PRINT (("File: %s\n", FindInfo.name));
        AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_HEADER);

        while (_findnext (FindHandle, &FindInfo) == 0)
        {
            VERBOSE_PRINT (("File: %s\n", FindInfo.name));
            AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_HEADER);
        }

        _findclose (FindHandle);
    }

    /* Do other files */

    strcpy (FileSpec, SourcePath);
    strcat (FileSpec, "/*.l");

    VERBOSE_PRINT (("Checking for lex files in path \"%s\"\n", FileSpec));

    FindHandle = _findfirst (FileSpec, &FindInfo);
    if (FindHandle != -1)
    {
        VERBOSE_PRINT (("File: %s\n", FindInfo.name));
        AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_SOURCE);

        while (_findnext (FindHandle, &FindInfo) == 0)
        {
            VERBOSE_PRINT (("File: %s\n", FindInfo.name));
            AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_SOURCE);
        }

        _findclose (FindHandle);
    }

    /* Do other files */

    strcpy (FileSpec, SourcePath);
    strcat (FileSpec, "/*.y");

    VERBOSE_PRINT (("Checking for yacc files in path \"%s\"\n", FileSpec));

    FindHandle = _findfirst (FileSpec, &FindInfo);
    if (FindHandle != -1)
    {
        VERBOSE_PRINT (("File: %s\n", FindInfo.name));
        AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_SOURCE);

        while (_findnext (FindHandle, &FindInfo) == 0)
        {
            VERBOSE_PRINT (("File: %s\n", FindInfo.name));
            AsProcessOneFile (ConversionTable, SourcePath, TargetPath, MaxPathLength, FindInfo.name, FILE_TYPE_SOURCE);
        }

        _findclose (FindHandle);
    }


    /* Do the subdirectories */

    strcpy (FileSpec, SourcePath);
    strcat (FileSpec, "/*.*");

    VERBOSE_PRINT (("Checking for subdirectories in path \"%s\"\n", FileSpec));

    FindHandle = _findfirst (FileSpec, &FindInfo);
    if (FindHandle != -1)
    {
        if (!AsCheckForDirectory (SourcePath, TargetPath, &FindInfo, &SourceDirPath, &TargetDirPath))
        {
            AsProcessTree (ConversionTable, SourceDirPath, TargetDirPath);
            free (SourceDirPath);
            free (TargetDirPath);
        }

        while (_findnext (FindHandle, &FindInfo) == 0)
        {
            if (!AsCheckForDirectory (SourcePath, TargetPath, &FindInfo, &SourceDirPath, &TargetDirPath))
            {
                AsProcessTree (ConversionTable, SourceDirPath, TargetDirPath);
                free (SourceDirPath);
                free (TargetDirPath);
            }
        }

        _findclose (FindHandle);
    }


    free (FileSpec);
    return 0;
}


BOOLEAN
AsDetectLoneLineFeeds (
    char                    *Filename,
    char                    *Buffer)
{
    UINT32                  i = 1;
    UINT32                  LfCount = 0;

    if (!Buffer[0])
    {
        return FALSE;
    }

    while (Buffer[i])
    {
        if (Buffer[i] == 0x0A)
        {
            if (Buffer[i-1] != 0x0D)
            {
                LfCount++;
            }
        }

        i++;
    }

    if (LfCount)
    {
        printf ("****UNIX CONTAMINATION DETECTED****\n");
        printf ("%d lone linefeeds in file %s\n", LfCount, Filename);
        return TRUE;
    }

    return (FALSE);
}


/******************************************************************************
 *
 * FUNCTION:    AsConvertFile
 *
 * DESCRIPTION: Perform the requested transforms on the file buffer (as
 *              determined by the ConversionTable and the FileType).
 *
 ******************************************************************************/

void
AsConvertFile (
    ACPI_CONVERSION_TABLE   *ConversionTable,
    char                    *FileBuffer,
    char                    *Filename,
    NATIVE_INT              FileType)
{
    UINT32                  i;
    UINT32                  Functions;
    ACPI_STRING_TABLE       *StringTable;
    ACPI_IDENTIFIER_TABLE   *ConditionalTable;
    ACPI_IDENTIFIER_TABLE   *LineTable;


    switch (FileType)
    {
    case FILE_TYPE_SOURCE:
        Functions           = ConversionTable->SourceFunctions;
        StringTable         = ConversionTable->SourceStringTable;
        LineTable           = ConversionTable->SourceLineTable;
        ConditionalTable    = ConversionTable->SourceConditionalTable;
        break;

    case FILE_TYPE_HEADER:
        Functions           = ConversionTable->HeaderFunctions;
        StringTable         = ConversionTable->HeaderStringTable;
        LineTable           = ConversionTable->HeaderLineTable;
        ConditionalTable    = ConversionTable->HeaderConditionalTable;
        break;

    default:
        printf ("Unknown file type, cannot process\n");
        return;
    }


    Gbl_Files++;
    VERBOSE_PRINT (("Processing %d bytes\n", strlen (FileBuffer)));
//    TERSE_PRINT (("."));


    /* Process all the string replacements */

    if (StringTable)
    {
        for (i = 0; StringTable[i].Target; i++)
        {
            AsReplaceString (StringTable[i].Target,
                                StringTable[i].Replacement,
                                FileBuffer);
        }
    }

    if (LineTable)
    {
        for (i = 0; LineTable[i].Identifier; i++)
        {
            AsRemoveLine (FileBuffer, LineTable[i].Identifier);
        }
    }


    if (ConditionalTable)
    {
        for (i = 0; ConditionalTable[i].Identifier; i++)
        {
            AsRemoveConditionalCompile (FileBuffer, ConditionalTable[i].Identifier);
        }
    }


    /* Process the function table */

    for (i = 0; i < 32; i++)
    {
        /* Decode the function bitmap */

        switch ((1 << i) & Functions)
        {
        case 0:
            /* This function not configured */
            break;


        case CVT_COUNT_TABS:

            AsCountTabs (FileBuffer, Filename);
            break;


        case CVT_COUNT_NON_ANSI_COMMENTS:

            AsCountNonAnsiComments (FileBuffer, Filename);
            break;


        case CVT_TRIM_LINES:

            AsTrimLines (FileBuffer, Filename);
            break;


        case CVT_COUNT_LINES:

            AsCountSourceLines (FileBuffer, Filename);
            break;


        case CVT_BRACES_ON_SAME_LINE:

            AsBracesOnSameLine (FileBuffer);
            break;


        case CVT_MIXED_CASE_TO_UNDERSCORES:

            AsMixedCaseToUnderscores (FileBuffer);
            break;


        case CVT_LOWER_CASE_IDENTIFIERS:

            AsLowerCaseIdentifiers (FileBuffer);
            break;


        case CVT_REMOVE_DEBUG_MACROS:

            AsRemoveDebugMacros (FileBuffer);
            break;


        case CVT_TRIM_WHITESPACE:

            AsTrimWhitespace (FileBuffer);
            break;


        case CVT_REMOVE_EMPTY_BLOCKS:

            AsRemoveEmptyBlocks (FileBuffer, Filename);
            break;


        case CVT_SPACES_TO_TABS4:

            AsTabify4 (FileBuffer);
            break;


        case CVT_SPACES_TO_TABS8:

            AsTabify8 (FileBuffer);
            break;


        default:

            printf ("Unknown conversion subfunction opcode\n");
            break;
        }
    }


    if (ConversionTable->NewHeader)
    {
        AsReplaceHeader (FileBuffer, ConversionTable->NewHeader);
    }


    /* Tabify should always be last */

//    AsTabify (FileBuffer, 4);
}


/******************************************************************************
 *
 * FUNCTION:    AsProcessOneFile
 *
 * DESCRIPTION: Process one source file.  The file is opened, read entirely
 *              into a buffer, converted, then written to a new file.
 *
 ******************************************************************************/

NATIVE_INT
AsProcessOneFile (
    ACPI_CONVERSION_TABLE   *ConversionTable,
    char                    *SourcePath,
    char                    *TargetPath,
    int                     MaxPathLength,
    char                    *Filename,
    NATIVE_INT              FileType)
{
    char                    *Pathname;


    /* Allocate a file pathname buffer for both source and target */

    Pathname = calloc (MaxPathLength + strlen (Filename) + 2, 1);
    if (!Pathname)
    {
        printf ("Could not allocate buffer for file pathnames\n");
        return -1;
    }

    /* Generate the source pathname and read the file */

    if (SourcePath)
    {
        strcpy (Pathname, SourcePath);
        strcat (Pathname, "/");
    }

    strcat (Pathname, Filename);

    if (AsGetFile (Pathname, &Gbl_FileBuffer, &Gbl_FileSize))
    {
        return -1;
    }

    /* Process the file in the buffer */

    AsConvertFile (ConversionTable, Gbl_FileBuffer, Pathname, FileType);

    if (!(ConversionTable->Flags & FLG_NO_FILE_OUTPUT))
    {
        /* Generate the target pathname and write the file */

        strcpy (Pathname, TargetPath);
        if (SourcePath)
        {
            strcat (Pathname, "/");
            strcat (Pathname, Filename);
        }

        AsPutFile (Pathname, Gbl_FileBuffer, ConversionTable->Flags);
    }

    free (Gbl_FileBuffer);
    free (Pathname);

    return 0;
}


/******************************************************************************
 *
 * FUNCTION:    AsCheckForDirectory
 *
 * DESCRIPTION: Check if the current file is a directory.  If not, construct
 *              the full pathname for the source and target paths.  Checks
 *              for the dot and dot-dot files as well (they are ignored)
 *
 ******************************************************************************/

NATIVE_INT
AsCheckForDirectory (
    char                    *SourceDirPath,
    char                    *TargetDirPath,
    struct _finddata_t      *FindInfo,
    char                    **SourcePath,
    char                    **TargetPath)
{
    char                    *SrcPath;
    char                    *TgtPath;


    if (!(FindInfo->attrib & _A_SUBDIR))
    {
        return -1;
    }

    if (!(strcmp (FindInfo->name, ".")) ||
        !(strcmp (FindInfo->name, "..")))
    {
        return -1;
    }


    SrcPath = calloc (strlen (SourceDirPath) + strlen (FindInfo->name) + 2, 1);
    if (!SrcPath)
    {
        printf ("Could not allocate buffer for directory source pathname\n");
        return -1;
    }

    TgtPath = calloc (strlen (TargetDirPath) + strlen (FindInfo->name) + 2, 1);
    if (!TgtPath)
    {
        printf ("Could not allocate buffer for directory target pathname\n");
        free (SrcPath);
        return -1;
    }

    strcpy (SrcPath, SourceDirPath);
    strcat (SrcPath, "/");
    strcat (SrcPath, FindInfo->name);

    strcpy (TgtPath, TargetDirPath);
    strcat (TgtPath, "/");
    strcat (TgtPath, FindInfo->name);

    *SourcePath = SrcPath;
    *TargetPath = TgtPath;

    return 0;
}


/******************************************************************************
 *
 * FUNCTION:    AsGetFile
 *
 * DESCRIPTION: Open a file and read it entirely into a an allocated buffer
 *
 ******************************************************************************/

int
AsGetFile (
    char                    *Filename,
    char                    **FileBuffer,
    UINT32                  *FileSize)
{

    int                     FileHandle;
    UINT32                  Size;
    char                    *Buffer;


    /* Binary mode leaves CR/LF pairs */

    FileHandle = open (Filename, O_BINARY | O_RDONLY);
    if (!FileHandle)
    {
        printf ("Could not open %s\n", Filename);
        return -1;
    }

    if (fstat (FileHandle, &Gbl_StatBuf))
    {
        printf ("Could not get file status for %s\n", Filename);
        goto ErrorExit;
    }

    /*
     * Create a buffer for the entire file
     * Add 10% extra to accomodate string replacements
     */

    Size = Gbl_StatBuf.st_size;
    Buffer = calloc (Size + (Size / 10), 1);
    if (!Buffer)
    {
        printf ("Could not allocate buffer of size %d\n", Size + (Size / 10));
        goto ErrorExit;
    }

    /* Read the entire file */

    Size = read (FileHandle, Buffer, Size);
    if (Size == -1)
    {
        printf ("Could not read the input file %s\n", Filename);
        goto ErrorExit;
    }

    Buffer [Size] = 0;         /* Null terminate the buffer */
    close (FileHandle);


    /* Check for unix contamination */

    if (AsDetectLoneLineFeeds (Filename, Buffer))
    {
        return -1;
    }

    /* Text mode translates CR/LF to LF only on input */

    FileHandle = open (Filename, O_TEXT | O_RDONLY);
    if (!FileHandle)
    {
        printf ("Could not open %s\n", Filename);
        return -1;
    }

    if (fstat (FileHandle, &Gbl_StatBuf))
    {
        printf ("Could not get file status for %s\n", Filename);
        goto ErrorExit;
    }
    Size = Gbl_StatBuf.st_size;


    /* Read the entire file */

    Size = read (FileHandle, Buffer, Size);
    if (Size == -1)
    {
        printf ("Could not read the input file %s\n", Filename);
        goto ErrorExit;
    }

    Buffer [Size] = 0;         /* Null terminate the buffer */
    close (FileHandle);


    *FileBuffer = Buffer;
    *FileSize = Size;

    return 0;


ErrorExit:

    close (FileHandle);
    return -1;
}


/******************************************************************************
 *
 * FUNCTION:    AsPutFile
 *
 * DESCRIPTION: Create a new output file and write the entire contents of the
 *              buffer to the new file.  Buffer must be a zero terminated string
 *
 ******************************************************************************/

int
AsPutFile (
    char                    *Pathname,
    char                    *FileBuffer,
    UINT32                  SystemFlags)
{
    UINT32                  FileSize;
    int                     DestHandle;
    int                     OpenFlags;


    /* Create the target file */

    OpenFlags = O_TRUNC | O_CREAT | O_WRONLY;

    if (SystemFlags & FLG_NO_CARRIAGE_RETURNS)
    {
        OpenFlags |= O_BINARY;
    }
    else
    {
        OpenFlags |= O_TEXT;
    }

    DestHandle = open (Pathname, OpenFlags, _S_IREAD | _S_IWRITE);
    if (DestHandle == -1)
    {
        perror ("Could not create destination file");
        printf ("Could not create destination file \"%s\"\n", Pathname);
        return -1;
    }

    /* Write the buffer to the file */

    FileSize = strlen (FileBuffer);
    write (DestHandle, FileBuffer, FileSize);

    close (DestHandle);

    return 0;
}

