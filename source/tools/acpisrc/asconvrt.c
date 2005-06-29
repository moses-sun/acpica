
/******************************************************************************
 *
 * Module Name: asconvrt - Source conversion code
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


/* Opening signature of the Intel legal header */

char        *HeaderBegin = "/******************************************************************************\n *\n * 1. Copyright Notice";


/******************************************************************************
 *
 * FUNCTION:    AsPrint
 *
 * DESCRIPTION: Common formatted print
 *
 ******************************************************************************/

void
AsPrint (
    char                    *Message,
    UINT32                  Count,
    char                    *Filename)
{


    printf ("-- %4u %28.28s : %s\n", Count, Message, Filename);
}


/******************************************************************************
 *
 * FUNCTION:    AsTrimLines
 *
 * DESCRIPTION: Remove extra blanks from the end of source lines.  Does not
 *              check for tabs.
 *
 ******************************************************************************/

void
AsTrimLines (
    char                    *Buffer,
    char                    *Filename)
{
    UINT32                  Length;
    char                    *SubBuffer = Buffer;
    char                    *StartWhiteSpace = NULL;
    UINT32                  SpaceCount = 0;


    while (*SubBuffer)
    {
        while (*SubBuffer != '\n')
        {
            if (!*SubBuffer)
            {
                goto Exit;
            }

            if (*SubBuffer == ' ')
            {
                if (!StartWhiteSpace)
                {
                    StartWhiteSpace = SubBuffer;
                }
            }
            else
            {
                StartWhiteSpace = NULL;
            }

            SubBuffer++;
        }

        if (StartWhiteSpace)
        {
            SpaceCount += (SubBuffer - StartWhiteSpace);

            Length = strlen (SubBuffer) + 1;
            memmove (StartWhiteSpace, SubBuffer, Length);
            StartWhiteSpace = NULL;
        }

        SubBuffer++;
    }


Exit:
    if (SpaceCount)
    {
        AsPrint ("Extraneous spaces Removed", SpaceCount, Filename);
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsTrimWhitespace
 *
 * DESCRIPTION: Remove "excess" blank lines - any more than 2 blank lines.
 *              this can happen during the translation when lines are removed.
 *
 ******************************************************************************/

void
AsTrimWhitespace (
    char                    *Buffer)
{
    int                     ReplaceCount = 1;


    while (ReplaceCount)
    {
        ReplaceCount = AsReplaceString ("\n\n\n\n", "\n\n\n", Buffer);
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsReplaceHeader
 *
 * DESCRIPTION: Replace the default Intel legal header with a new header
 *
 ******************************************************************************/

void
AsReplaceHeader (
    char                    *Buffer,
    char                    *NewHeader)
{
    UINT32                  Length;
    UINT32                  HdrLength;
    char                    *SubBuffer;
    char                    *TokenEnd;


    /* Find the original header */

    SubBuffer = strstr (Buffer, HeaderBegin);
    if (!SubBuffer)
    {
        return;
    }

    /* Find the end of the original header */

    TokenEnd = strstr (SubBuffer, "*/");
    TokenEnd = AsSkipPastChar (TokenEnd, '\n');

    /* Move up the rest of the buffer to delete the header */

    Length = strlen (TokenEnd) + 1;
    memmove (SubBuffer, TokenEnd, Length);

    /* Insert the new header */

    HdrLength = strlen (NewHeader);
    memmove (SubBuffer + HdrLength, SubBuffer, Length);
    memmove (SubBuffer, NewHeader, HdrLength);

}


/******************************************************************************
 *
 * FUNCTION:    AsReplaceString
 *
 * DESCRIPTION: Replace all instances of a target string with a replacement
 *              string.  Returns count of the strings replaced.
 *
 ******************************************************************************/

int
AsReplaceString (
    char                    *Target,
    char                    *Replacement,
    char                    *Buffer)
{
    char                    *SubString1;
    char                    *SubString2;
    char                    *SubBuffer;
    int                     TargetLength;
    int                     ReplacementLength;
    int                     ReplaceCount = 0;


    TargetLength = strlen (Target);
    ReplacementLength = strlen (Replacement);

    SubBuffer = Buffer;
    SubString1 = Buffer;


    while (SubString1)
    {
        /* Find the target string */

        SubString1 = strstr (SubBuffer, Target);

        /*
         * Check for translation escape string -- means to ignore
         * blocks of code while replacing
         */

        SubString2 = strstr (SubBuffer, "/*!");

        if ((SubString2) &&
            (SubString2 < SubString1))
        {
            /* Find end of the escape block starting at "Substring2" */

            SubString2 = strstr (SubString2, "!*/");
            if (!SubString2)
            {
                /* Didn't find terminator */

                return ReplaceCount;
            }

            /* Move buffer to end of escape block and continue */

            SubBuffer = SubString2;
        }

        /* Do the actual replace if the target was found */

        else if (SubString1)
        {
            SubBuffer = AsReplaceData (SubString1, TargetLength, Replacement, ReplacementLength);
            ReplaceCount++;
        }
    }

    return ReplaceCount;
}


/******************************************************************************
 *
 * FUNCTION:    AsMixedCaseToUnderscores
 *
 * DESCRIPTION: Converts mixed case identifiers to underscored identifiers.
 *              for example,
 *
 *              ThisUsefullyNamedIdentifier   becomes:
 *
 *              this_usefully_named_identifier
 *
 ******************************************************************************/

void
AsMixedCaseToUnderscores (
    char                    *Buffer)
{
    UINT32                  Length;
    char                    *SubBuffer = Buffer;
    char                    *TokenEnd;
    char                    *SubString;


    while (*SubBuffer)
    {

        /*
         * Check for translation escape string -- means to ignore
         * blocks of code while replacing
         */

        if ((SubBuffer[0] == '/') &&
            (SubBuffer[1] == '*') &&
            (SubBuffer[2] == '!'))
        {
            SubBuffer = strstr (SubBuffer, "!*/");
            if (!SubBuffer)
            {
                return;
            }
        }

        /* Ignore hex constants */

        if (SubBuffer[0] == '0')
        {
            if ((SubBuffer[1] == 'x') ||
                (SubBuffer[1] == 'X'))
            {
                SubBuffer += 2;
                while (isxdigit (*SubBuffer))
                {
                    SubBuffer++;
                }
                continue;
            }

            SubBuffer++;
            continue;
        }

        /* Ignore format specification fields */

        if (SubBuffer[0] == '%')
        {
            SubBuffer++;

            while ((isalnum (*SubBuffer)) ||
                   (*SubBuffer == '.'))
            {
                SubBuffer++;
            }

            continue;
        }


        /*
         * Convert each pair of letters that matches the form:
         *
         *      <LowerCase><UpperCase>
         * to
         *      <LowerCase><Underscore><LowerCase>
         */

        else if ((islower (SubBuffer[0]) || isdigit (SubBuffer[0])) &&
                 (isupper (SubBuffer[1])))

        {
            TokenEnd = SubBuffer;
            while ((isalnum (*TokenEnd)) || (*TokenEnd == '_'))
            {
                TokenEnd++;
            }

            SubBuffer[1] = (char) tolower (SubBuffer[1]);


            SubString = TokenEnd;
            Length = 0;

            while (*SubString != '\n')
            {
                if ((SubString[0] == ' ') &&
                    (SubString[1] == ' '))
                {
                    Length = SubString - SubBuffer - 2;
                    break;
                }

                SubString++;
            }


            if (!Length)
            {
                Length = strlen (&SubBuffer[1]);
            }

            memmove (&SubBuffer[2], &SubBuffer[1], (Length+1));
            SubBuffer[1] = '_';
            SubBuffer +=2;
        }

        SubBuffer++;
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsLowerCaseIdentifiers
 *
 * DESCRIPTION: Converts mixed case identifiers to lower case.  Leaves comments,
 *              quoted strings, and all-upper-case macros alone.
 *
 ******************************************************************************/

void
AsLowerCaseIdentifiers (
    char                    *Buffer)
{
    char                    *SubBuffer = Buffer;


    while (*SubBuffer)
    {
        /*
         * Check for translation escape string -- means to ignore
         * blocks of code while replacing
         */

        if ((SubBuffer[0] == '/') &&
            (SubBuffer[1] == '*') &&
            (SubBuffer[2] == '!'))
        {
            SubBuffer = strstr (SubBuffer, "!*/");
            if (!SubBuffer)
            {
                return;
            }
        }

        /* Ignore comments */

        if ((SubBuffer[0] == '/') &&
            (SubBuffer[1] == '*'))
        {
            SubBuffer = strstr (SubBuffer, "*/");
            if (!SubBuffer)
            {
                return;
            }

            SubBuffer += 2;
        }

        /* Ignore quoted strings */

        if (SubBuffer[0] == '\"')
        {
            SubBuffer++;
            SubBuffer = AsSkipPastChar (SubBuffer, '\"');
            if (!SubBuffer)
            {
                return;
            }
        }


        /*
         * Only lower case if we have an upper followed by a lower
         * This leaves the all-uppercase things (macros, etc.) intact
         */

        if ((isupper (SubBuffer[0])) &&
            (islower (SubBuffer[1])))
        {
            *SubBuffer = (char) tolower (*SubBuffer);
        }

        SubBuffer++;
    }

}


/******************************************************************************
 *
 * FUNCTION:    AsBracesOnSameLine
 *
 * DESCRIPTION: Move opening braces up to the same line as an if, for, else,
 *              or while statement (leave function opening brace on separate
 *              line).
 *
 ******************************************************************************/

void
AsBracesOnSameLine (
    char                    *Buffer)
{
    UINT32                  Length;
    char                    *SubBuffer = Buffer;
    char                    *Beginning;
    char                    *StartOfThisLine;
    char                    Tmp;


    while (*SubBuffer)
    {
        if (*SubBuffer == '{')
        {
            /*
             * Backup to previous non-whitespace
             */

            Beginning = SubBuffer - 1;
            while ((*Beginning == ' ')   ||
                   (*Beginning == '\n'))
            {
                Beginning--;
            }

            StartOfThisLine = Beginning;
            while (*StartOfThisLine != '\n')
            {
                StartOfThisLine--;
            }

            Beginning++;
            Tmp = *Beginning;
            *Beginning = 0;

            if ((strstr (StartOfThisLine, " if"))   ||
                (strstr (StartOfThisLine, " for"))  ||
                (strstr (StartOfThisLine, " else")) ||
                (strstr (StartOfThisLine, " while")))
            {
                SubBuffer++;
                Length = strlen (SubBuffer);
                memmove (Beginning + 2, SubBuffer, Length+3);

                memmove (Beginning, " {", 2);
                /* memmove (Beginning, " {\n", 3); */
            }
            else
            {
                *Beginning = Tmp;
            }
        }

        SubBuffer++;
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsRemoveStatement
 *
 * DESCRIPTION: Remove all statements that contain the given keyword.
 *              Limitations:  Removes text from the start of the line that
 *              contains the keyword to the next semicolon.  Currently
 *              doesn't ignore comments.
 *
 ******************************************************************************/

void
AsRemoveStatement (
    char                    *Buffer,
    char                    *Keyword)
{
    char                    *SubString;
    char                    *SubBuffer;
    int                     StrLength;


    SubBuffer = Buffer;
    SubString = Buffer;


    while (SubString)
    {
        SubString = strstr (SubBuffer, Keyword);

        if (SubString)
        {
            SubBuffer = SubString;

            /* Find start of this line */

            while (*SubString != '\n')
            {
                SubString--;
            }
            SubString++;

            /* Find end of this statement */

            SubBuffer = AsSkipPastChar (SubBuffer, ';');
            if (!SubBuffer)
            {
                return;
            }

            /* Find end of this line */

            SubBuffer = AsSkipPastChar (SubBuffer, '\n');
            if (!SubBuffer)
            {
                return;
            }

            /* If next line is blank, remove it too */

            if (*SubBuffer == '\n')
            {
                SubBuffer++;
            }

            StrLength = strlen (SubBuffer);

            memmove (SubString, SubBuffer, StrLength+1);
            SubBuffer = SubString;
        }
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsRemoveConditionalCompile
 *
 * DESCRIPTION: Remove a "#ifdef" statement, and all text that it encompasses.
 *              Limitations: cannot handle nested ifdefs.
 *
 ******************************************************************************/

void
AsRemoveConditionalCompile (
    char                    *Buffer,
    char                    *Keyword)
{
    char                    *SubString;
    char                    *SubBuffer;
    char                    *IfPtr;
    char                    *EndifPtr;
    char                    *ElsePtr;
    char                    *Comment;
    int                     StrLength;


    SubBuffer = Buffer;
    SubString = Buffer;


    while (SubString)
    {
        Comment = strstr (SubString, "/*");
        SubBuffer = strstr (SubString, Keyword);
        if (!SubBuffer)
        {
            return;
        }

        if ((Comment) &&
            (Comment < SubBuffer))
        {
            SubString = strstr (Comment, "*/");
            if (!SubString)
            {
                return;
            }

            SubString += 2;
            continue;
        }

        SubString = SubBuffer;

        /* Find start of this line */

        while (*SubString != '\n' && (SubString > Buffer))
        {
            SubString--;
        }
        SubString++;


        /* Find the "#ifxxxx" */

        IfPtr = strstr (SubString, "#if");
        if (!IfPtr)
        {
            return;
        }

        if (IfPtr > SubBuffer)
        {
            /* Not the right #if */

            SubString = SubBuffer + strlen (Keyword);
            continue;
        }

        /* Find closing #endif or #else */

        EndifPtr = strstr (SubBuffer, "#endif");
        if (!EndifPtr)
        {
            /* There has to be an #endif */

            return;
        }

        ElsePtr = strstr (SubBuffer, "#else");
        if ((ElsePtr) &&
            (EndifPtr > ElsePtr))
        {
            /* This #ifdef contains an #else clause */

            /* Find end of this line */

            SubBuffer = AsSkipPastChar (ElsePtr, '\n');
            if (!SubBuffer)
            {
                return;
            }

            /* Remove the #ifdef .... #else code */

            StrLength = strlen (SubBuffer);
            memmove (SubString, SubBuffer, StrLength+1);

            /* Next, we will remove the #endif statement */

            EndifPtr = strstr (SubString, "#endif");
            if (!EndifPtr)
            {
                /* There has to be an #endif */

                return;
            }

            SubString = EndifPtr;
        }

        /* Remove the ... #endif part */

        /* Find end of this line */

        SubBuffer = AsSkipPastChar (EndifPtr, '\n');
        if (!SubBuffer)
        {
            return;
        }

        StrLength = strlen (SubBuffer);

        memmove (SubString, SubBuffer, StrLength+1);
        SubBuffer = SubString;
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsRemoveLine
 *
 * DESCRIPTION: Remove every line that contains the keyword.  Does not
 *              skip comments.
 *
 ******************************************************************************/

void
AsRemoveLine (
    char                    *Buffer,
    char                    *Keyword)
{
    char                    *SubString;
    char                    *SubBuffer;
    int                     StrLength;


    SubBuffer = Buffer;
    SubString = Buffer;


    while (SubString)
    {
        SubString = strstr (SubBuffer, Keyword);

        if (SubString)
        {
            SubBuffer = SubString;

            /* Find start of this line */

            while (*SubString != '\n')
            {
                SubString--;
            }
            SubString++;

            /* Find end of this line */

            SubBuffer = AsSkipPastChar (SubBuffer, '\n');
            if (!SubBuffer)
            {
                return;
            }

            StrLength = strlen (SubBuffer);

            memmove (SubString, SubBuffer, StrLength+1);
            SubBuffer = SubString;
        }
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsRemoveEmptyBlocks
 *
 * DESCRIPTION: Remove any C blocks (e.g., if {}) that contain no code.  This
 *              can happen as a result of removing lines such as DEBUG_PRINT.
 *
 ******************************************************************************/

void
AsRemoveEmptyBlocks (
    char                    *Buffer,
    char                    *Filename)
{
    char                    *SubBuffer;
    char                    *BlockStart;
    BOOLEAN                 EmptyBlock = TRUE;
    BOOLEAN                 AnotherPassRequired = TRUE;
    UINT32                  BlockCount = 0;


    while (AnotherPassRequired)
    {
        SubBuffer = Buffer;
        AnotherPassRequired = FALSE;

        while (*SubBuffer)
        {
            if (*SubBuffer == '{')
            {
                BlockStart = SubBuffer;
                EmptyBlock = TRUE;

                SubBuffer++;
                while (*SubBuffer != '}')
                {
                    if ((*SubBuffer != ' ') &&
                        (*SubBuffer != '\n'))
                    {
                        EmptyBlock = FALSE;
                        break;
                    }
                    SubBuffer++;
                }

                if (EmptyBlock)
                {
                    /* Find start of the first line of the block */

                    while (*BlockStart != '\n')
                    {
                        BlockStart--;
                    }

                    /* Find end of the last line of the block */

                    SubBuffer = AsSkipUntilChar (SubBuffer, '\n');
                    if (!SubBuffer)
                    {
                        break;
                    }

                    memmove (BlockStart, SubBuffer, strlen (SubBuffer) +1);

                    SubBuffer = BlockStart;
                    BlockCount++;
                    AnotherPassRequired = TRUE;
                    continue;
                }

            }

            SubBuffer++;
        }
    }


    if (BlockCount)
    {
        AsPrint ("Code blocks deleted", BlockCount, Filename);
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsTabify4
 *
 * DESCRIPTION: Convert the text to tabbed text.  Alignment of text is
 *              preserved.
 *
 ******************************************************************************/

void
AsTabify4 (
    char                    *Buffer)
{
    char                    *SubBuffer = Buffer;
    char                    *NewSubBuffer;
    UINT32                  SpaceCount = 0;
    UINT32                  Column = 0;


    while (*SubBuffer)
    {
        if (*SubBuffer == '\n')
        {
            Column = 0;
        }

        else
        {
            Column++;
        }

        /* Ignore quoted strings */

        if (*SubBuffer == '\"')
        {
            SubBuffer++;
            SubBuffer = AsSkipPastChar (SubBuffer, '\"');
            if (!SubBuffer)
            {
                return;
            }
            SpaceCount = 0;
        }

        if (*SubBuffer == ' ')
        {
            SpaceCount++;

            if (SpaceCount >= 4)
            {
                SpaceCount = 0;

                NewSubBuffer = (SubBuffer + 1) - 4;
                *NewSubBuffer = '\t';

                SubBuffer++;
                memmove ((NewSubBuffer + 1), SubBuffer, strlen (SubBuffer) + 1);
                SubBuffer = NewSubBuffer;
            }

            if ((Column % 4) == 0)
            {
                SpaceCount = 0;
            }
        }

        else
        {
            SpaceCount = 0;
        }

        SubBuffer++;
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsTabify8
 *
 * DESCRIPTION: Convert the text to tabbed text.  Alignment of text is
 *              preserved.
 *
 ******************************************************************************/

void
AsTabify8 (
    char                    *Buffer)
{
    char                    *SubBuffer = Buffer;
    char                    *NewSubBuffer;
    UINT32                  SpaceCount = 0;
    UINT32                  Column = 0;
    UINT32                  TabCount = 0;
    UINT32                  LastLineTabCount = 0;
    UINT32                  LastLineColumnStart = 0;
    UINT32                  ThisColumnStart = 0;
    UINT32                  ThisTabCount =  0;
    char                    *FirstNonBlank = NULL;


    while (*SubBuffer)
    {
        if (*SubBuffer == '\n')
        {
            Column = 0;
            TabCount = 0;
            SubBuffer++;
            continue;
        }

        else
        {
            if (!FirstNonBlank)
            {
                FirstNonBlank = SubBuffer;
                while (*FirstNonBlank == ' ')
                {
                    FirstNonBlank++;
                }

                ThisColumnStart = FirstNonBlank - SubBuffer;

                if (LastLineTabCount == 0)
                {
                    ThisTabCount = 0;
                }

                else if (ThisColumnStart == LastLineColumnStart)
                {
                    ThisTabCount = LastLineTabCount -1;
                }

                else
                {

                    ThisTabCount = LastLineTabCount + 1;
                }
            }

            Column++;
        }


        if (*SubBuffer != ' ')
        {

            SubBuffer = AsSkipUntilChar (SubBuffer, '\n');
            if (!SubBuffer)
            {
                return;
            }
            if (TabCount > 0)
            {
                LastLineTabCount = TabCount;
            }

            FirstNonBlank = NULL;
            LastLineColumnStart = ThisColumnStart;
            TabCount = 0;
            Column = 0;
            SpaceCount = 0;
            SubBuffer++;
            continue;
        }


        SpaceCount++;

        if (SpaceCount >= 4)
        {
            SpaceCount = 0;

            NewSubBuffer = SubBuffer - 4;

            if (TabCount <= ThisTabCount ? (ThisTabCount +1) : 0)
            {
                NewSubBuffer++;
                *NewSubBuffer = '\t';
                SubBuffer++;
                TabCount++;
            }

            memmove ((NewSubBuffer + 1), SubBuffer, strlen (SubBuffer) + 1);
            SubBuffer = NewSubBuffer;

        }

        SubBuffer++;
    }
}


/******************************************************************************
 *
 * FUNCTION:    AsRemoveDebugMacros
 *
 * DESCRIPTION: Remove all "Debug" macros -- macros that produce debug output.
 *
 ******************************************************************************/

void
AsRemoveDebugMacros (
    char                    *Buffer)
{


    AsRemoveStatement (Buffer, "DEBUG_PRINT");
    AsRemoveStatement (Buffer, "DEBUG_EXEC");
    AsRemoveStatement (Buffer, "FUNCTION_TRACE");
    AsRemoveStatement (Buffer, "DUMP_");

    AsReplaceString ("return_VOID",         "return", Buffer);
    AsReplaceString ("return_PTR",          "return", Buffer);
    AsReplaceString ("return_ACPI_STATUS",  "return", Buffer);
    AsReplaceString ("return_VALUE",        "return", Buffer);

    AsRemoveConditionalCompile (Buffer, "ACPI_DEBUG");
}


/******************************************************************************
 *
 * FUNCTION:    AsCountLines
 *
 * DESCRIPTION: Count the number of lines in the input buffer.  Also count
 *              the number of long lines (lines longer than 80 chars).
 *
 ******************************************************************************/

UINT32
AsCountLines (
    char                    *Buffer,
    char                    *Filename)
{
    char                    *SubBuffer = Buffer;
    char                    *EndOfLine;
    UINT32                  LineCount = 0;
    UINT32                  LongLineCount = 0;


    while (*SubBuffer)
    {
        EndOfLine = AsSkipUntilChar (SubBuffer, '\n');
        if (!EndOfLine)
        {
            Gbl_TotalLines += LineCount;
            return LineCount;
        }

        if ((EndOfLine - SubBuffer) > 80)
        {
            LongLineCount++;
        }

        LineCount++;
        SubBuffer = EndOfLine + 1;
    }


    if (LongLineCount)
    {
        VERBOSE_PRINT (("%d Lines longer than 80 found in %s\n", LongLineCount, Filename));
        Gbl_LongLines += LongLineCount;
    }

    Gbl_TotalLines += LineCount;
    return LineCount;
}


/******************************************************************************
 *
 * FUNCTION:    AsCountTabs
 *
 * DESCRIPTION: Simply count the number of tabs in the input file buffer
 *
 ******************************************************************************/

void
AsCountTabs (
    char                    *Buffer,
    char                    *Filename)
{
    UINT32                  i;
    UINT32                  TabCount = 0;


    for (i = 0; Buffer[i]; i++)
    {
        if (Buffer[i] == '\t')
        {
            TabCount++;
        }
    }

    if (TabCount)
    {
        AsPrint ("Tabs found", TabCount, Filename);
        Gbl_Tabs += TabCount;
    }

    AsCountLines (Buffer, Filename);
}


/******************************************************************************
 *
 * FUNCTION:    AsCountNonAnsiComments
 *
 * DESCRIPTION: Count the number of "//" comments.  This type of comment is
 *              non-ANSI C.
 *
 ******************************************************************************/

void
AsCountNonAnsiComments (
    char                    *Buffer,
    char                    *Filename)
{
    char                    *SubBuffer = Buffer;
    UINT32                  CommentCount = 0;


    while (SubBuffer)
    {
        SubBuffer = strstr (SubBuffer, "//");
        if (SubBuffer)
        {
            CommentCount++;
            SubBuffer += 2;
        }
    }

    if (CommentCount)
    {
        AsPrint ("Non-ANSI Comments found", CommentCount, Filename);
        Gbl_NonAnsiComments += CommentCount;
    }
}

/******************************************************************************
 *
 * FUNCTION:    AsCountSourceLines
 *
 * DESCRIPTION: Count the number of C source lines.  Defined by 1) not a
 *              comment, and 2) not a blank line.
 *
 ******************************************************************************/

void
AsCountSourceLines (
    char                    *Buffer,
    char                    *Filename)
{
    char                    *SubBuffer = Buffer;
    UINT32                  LineCount = 0;
    UINT32                  WhiteCount = 0;
    UINT32                  CommentCount = 0;


    while (*SubBuffer)
    {
        /* Ignore comments */

        if ((SubBuffer[0] == '/') &&
            (SubBuffer[1] == '*'))
        {
            CommentCount++;
            SubBuffer += 2;

            while (SubBuffer[0] && SubBuffer[1] &&
                    !(((SubBuffer[0] == '*') &&
                      (SubBuffer[1] == '/'))))
            {
                if (SubBuffer[0] == '\n')
                {
                    CommentCount++;
                }

                SubBuffer++;
            }
        }

        /* A linefeed followed by a non-linefeed is a valid source line */

        else if ((SubBuffer[0] == '\n') &&
                 (SubBuffer[1] != '\n'))
        {
            LineCount++;
        }

        /* Two back-to-back linefeeds indicate a whitespace line */

        else if ((SubBuffer[0] == '\n') &&
                 (SubBuffer[1] == '\n'))
        {
            WhiteCount++;
        }

        SubBuffer++;
    }

    /* Adjust comment count for legal header */

    CommentCount -= LINES_IN_LEGAL_HEADER;


    Gbl_SourceLines += LineCount;
    Gbl_WhiteLines += WhiteCount;
    Gbl_CommentLines += CommentCount;

    VERBOSE_PRINT (("%d Comment %d White %d Code %d Lines in %s\n",
                CommentCount, WhiteCount, LineCount, LineCount+WhiteCount+CommentCount, Filename));
}


/******************************************************************************
 *
 * FUNCTION:    AsUppercaseTokens
 *
 * DESCRIPTION: Force to uppercase all tokens that begin with the prefix string.
 *              used to convert mixed-case macros and constants to uppercase.
 *
 ******************************************************************************/

void
AsUppercaseTokens (
    char                    *Buffer,
    char                    *PrefixString)
{
    char                    *SubBuffer;
    char                    *TokenEnd;
    char                    *SubString;
    int                     i;
    UINT32                  Length;


    SubBuffer = Buffer;

    while (SubBuffer)
    {
        SubBuffer = strstr (SubBuffer, PrefixString);
        if (SubBuffer)
        {
            TokenEnd = SubBuffer;
            while ((isalnum (*TokenEnd)) || (*TokenEnd == '_'))
            {
                TokenEnd++;
            }


            for (i = 0; i < (TokenEnd - SubBuffer); i++)
            {
                if ((islower (SubBuffer[i])) &&
                    (isupper (SubBuffer[i+1])))

                {

                    SubString = TokenEnd;
                    Length = 0;

                    while (*SubString != '\n')
                    {
                        if ((SubString[0] == ' ') &&
                            (SubString[1] == ' '))
                        {
                            Length = SubString - &SubBuffer[i] - 2;
                            break;
                        }

                        SubString++;
                    }


                    if (!Length)
                    {
                        Length = strlen (&SubBuffer[i+1]);
                    }

                    memmove (&SubBuffer[i+2], &SubBuffer[i+1], (Length+1));
                    SubBuffer[i+1] = '_';
                    i +=2;
                    TokenEnd++;
                }
            }

            for (i = 0; i < (TokenEnd - SubBuffer); i++)
            {
                SubBuffer[i] = (char) toupper (SubBuffer[i]);
            }

            SubBuffer = TokenEnd;
        }
    }
}

