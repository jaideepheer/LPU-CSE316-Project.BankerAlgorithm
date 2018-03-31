#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include"helper.h"
#include"UI.h"

/*
    This function is used to initialise the UIframe structure and allocate the memory for its buffer.
*/
void UIframe_init(struct UIframe *frame, int width, int height,
                    char lborder, char rborder, char roof, char floor, int textWrappingEnabled, int bordersEnabled)
{
    if(bordersEnabled)
    {
        // Reset internal width & height to compensate for border and roof
        width = width-2;
        height = height-2;
    }

    // Check for valid data.
    if(width<0||height<0||frame==NULL)return;

    // Allocate memory to store the frame's buffer.
    int i;
    frame->screen = (char**)malloc(height * sizeof(char*));
    for(i=0;i<height;++i)frame->screen[i] = (char*)malloc(width * sizeof(char));

    // Set the rest of the frame's attributes.
    frame->floor = floor;
    frame->roof = roof;
    frame->lborder = lborder;
    frame->rborder = rborder;
    frame->height = height;
    frame->width = width;
    frame->isTextWrappingEnabled = textWrappingEnabled;
    frame->bordersEnabled = bordersEnabled;
    frame->cursor[0]=0;
    frame->cursor[1]=0;

    // Fill the frame with spaces.
    UIframe_flush(frame);
}

void UIframe_flush(struct UIframe *frame)
{
    // Fill the screen with spaces.
    int i,j;
    for(i=0;i<frame->height;++i)
    {
        // Fill the row with spaces.
        memset(frame->screen[i], (int)' ', frame->width);
    }
    frame->cursor[0]=0;
    frame->cursor[1]=0;
}

/*
    This function prints the first strLen no. of chars. of the given string,
    starting from startPos, to the frame's buffer.
    It also performs text wrapping if enabled in the passed frame structure.
    It also increments the frame structure's internal cursor.

    Parameters: frame is the UIframe structure to operate on.
                str in the string to print in this line.
                startPos is the starting string position from where the string is printed to the buffer.
                strLen is the no. of characters to print from the string.
                position is the relative position of the string in the line.
                    It is a float value and must be between 0 and 1, inclusive.
                    For eg. position 0.5 causes center alignment.
*/
void UIframe_addLineChars(struct UIframe *frame, char *str, int startPos, int strLen, float position)
{
    // Increment cursor line.
    frame->cursor[0] = UIframe_setLineChars(frame,frame->cursor[0],str,startPos,strLen,position) + 1;
    frame->cursor[0] = frame->cursor[0]>frame->height-1?frame->height:frame->cursor[0];
    frame->cursor[1]=0;
}
void UIframe_addLine(struct UIframe *frame, char *str, int startPos, float position)
{
    UIframe_addLineChars(frame,  str, startPos, strlen(str)-startPos,position);
}
void UIframe_addLineAndBindTextFeilds(struct UIframe *frame, char *str, int startPos, float position, struct UITextField *textFieldArray)
{
    // Increment cursor line.
    frame->cursor[0] = UIframe_setLineCharsAndBindTextFeilds(frame,frame->cursor[0],str,startPos,strlen(str),position,textFieldArray) + 1;
    frame->cursor[0] = frame->cursor[0]>frame->height-1?frame->height:frame->cursor[0];
    frame->cursor[1]=0;
}

/*
    This function prints the first strLen no. of chars. of the given string,
    starting from startPos, to the frame's buffer.
    It also performs text wrapping if enabled in the passed frame structure.

    Parameters: frame is the UIframe structure to operate on.
                line is the line no. of the buffer to print on(starts from 1).
                str in the string to print in this line.
                startPos is the starting string position from where the string is printed to the buffer.
                strLen is the no. of characters to print from the string.
                position is the relative position of the string in the line.
                    It is a float value and must be between 0 and 1, inclusive.
                    For eg. position 0.5 causes center alignment.
                textFieldArray is an array of pointers to text field structures to bind within the frame.

    Return: returns the line number which was last set by this function, usefull if there is text wrapping.
*/
int UIframe_internal_setLineChars(struct UIframe *frame, int line, char *str, int startPos, int strLen, float position, struct UITextField *textFieldArray, int textFieldArrayCounter)
{
    // Check for valid data.
    if(frame==NULL||str==NULL||line>=frame->height)return;
    // Make sure position is a value between 0 and 1 and clip if not.
    position = clip(position, 0, 1);

    // Calculate the printable sting length in this line.
    int printableStrLen = (strLen<=frame->width)?strLen:frame->width;
    int extraStrLen = strLen-printableStrLen;

    // Calculate the no. of spaces needed in this frame line.
    int bufferPos = (frame->width - printableStrLen)*position;

    // Print the string to buffer.
    for(;printableStrLen>0;++startPos,--printableStrLen)
    {
        if(str[startPos]=='\n')
        {
            // Newline character found...
            extraStrLen+=printableStrLen;
            ++startPos;
            break;
        }
        // Check if textFieldArray is given and text field formater '%fn' is present
        else if(textFieldArray!=NULL&&str[startPos]=='%'&&str[startPos+1]=='f')
        {
            // get integer after '%f'
            int fieldlen=0,tmp=0;
            char *p;
            p = &str[startPos+2];
            while(*p>'0'&&*p<'9')
            {
                fieldlen = fieldlen*10 + *p - '0';
                ++tmp;
                ++p;
            }
            fieldlen = clip(fieldlen,0,frame->width - startPos);
            // Set values for the textField structure
            textFieldArray[textFieldArrayCounter].charPos = bufferPos;
            textFieldArray[textFieldArrayCounter].length = fieldlen;
            textFieldArray[textFieldArrayCounter].linePos = line;
            textFieldArray[textFieldArrayCounter].parentFrame = frame;
            ++textFieldArrayCounter;
            // Increment loop accordingly to skip the text field buffer part.
            bufferPos += fieldlen;
            startPos += 1+tmp;
            printableStrLen -= 1+tmp;
            continue;
        }
        frame->screen[line][bufferPos++] = str[startPos];
    }
    // Print wrapped text
    if(frame->isTextWrappingEnabled && extraStrLen>0)
        return UIframe_internal_setLineChars(frame,line+1,str,startPos,extraStrLen,0,textFieldArray,textFieldArrayCounter);
    // If no wrapping return current line number.
    return line;
}
int UIframe_setLineCharsAndBindTextFeilds(struct UIframe *frame, int line, char *str, int startPos, int strLen, float position, struct UITextField *textFieldArray)
{
    UIframe_internal_setLineChars(frame,line,str,startPos,strLen,position,textFieldArray,0);
}
int UIframe_setLineChars(struct UIframe *frame, int line, char *str, int startPos, int strLen, float position)
{
    return UIframe_internal_setLineChars(frame,line,str,startPos,strLen,position,NULL,0);
}
int UIframe_setLine(struct UIframe *frame, int line, char *str, int startPos, float position)
{
    return UIframe_setLineChars(frame,frame->cursor[0],str,startPos,strlen(str)-startPos,position);
}

/*
    This function prints the passed frame's buffer to the console at the passed X and Y positions,
    relative to the current cursor's position in the console.
*/
void UIframe_print(struct UIframe *frame, int x, int y)
{
    int i,j;
    // Move cursor to y pos. from the current pos.
    for(i=0;i<y;++i)printf("\n");

    if(frame->bordersEnabled)
    {
        // Move cursor to x pos.
        for(j=0;j<x;++j)printf(" ");
        // Print the roof.
        for(i=0;i<frame->width+2;++i)printf("%c",frame->roof);
        printf("\n");
    }
    
    for(i=0;i<frame->height;++i)
    {
        // Move cursor to x pos.
        for(j=0;j<x;++j)printf(" ");
        // Print border.
        if(frame->bordersEnabled)printf("%c",frame->rborder);
        for(j=0;j<frame->width;++j)
        {
            printf("%c",frame->screen[i][j]);
        }
        // Print border.
        if(frame->bordersEnabled)printf("%c",frame->lborder);
        printf("\n");
    }

    if(frame->bordersEnabled)
    {
        // Move cursor to x pos.
        for(j=0;j<x;++j)printf(" ");
        // Print the floor.
        for(i=0;i<frame->width+2;++i)printf("%c",frame->floor);
        printf("\n");
    }
}

/*-----------------------------------------------------*/
//              UITextField code
/*-----------------------------------------------------*/
/*
    This function initialises the passed UITextField structure from the passed UIframe.
    A UITextField structure marks a small part of a UIFrame's line and can update its buffer seamlessly.
    This helps make the program faster by allowing pre-generation of the UIFrame with constant strings 
        and adding UITextField wherever changing values are required.
*/
void UItextField_getFromFrame(struct UITextField* field, struct UIframe* frame, int linePos, int charPos, int length)
{
    // check if parameters are valid
    if(frame==NULL||field==NULL||linePos>frame->height-1||charPos>frame->width-1)return;
    // Clip length to prevent going beyond width.
    clip(length,0,frame->width - charPos);
    field->linePos = linePos;
    field->charPos = charPos;
    field->parentFrame = frame;
    field->length = length;
}
/*
    This function updates the parent frames buffer with the string passed.
    This works like printf() and accepts a format string wih variable arguments.
*/
void UItextField_setText(struct UITextField* field, char *str,...)
{
    // check if parameters are valid
    if(field==NULL)return;
    char *start = &field->parentFrame->screen[field->linePos][field->charPos];
    // Flush the field
    memset(start,' ',field->length);
    char postFeildchar = field->parentFrame->screen[field->linePos][field->charPos+field->length];
    va_list args;
    va_start(args,str);
    vsnprintf(start,field->length+1,str,args);
    va_end(args);
    field->parentFrame->screen[field->linePos][field->charPos+field->length] = postFeildchar;
}