#include<stdio.h>
#include<stdlib.h>
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
        for(j=0;j<frame->width;++j)frame->screen[i][j]=' ';
    }
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
void UIframe_addLine(struct UIframe *frame, char *str, int startPos, int strLen, float position)
{
    UIframe_setLine(frame,frame->cursor[0],str,startPos,strLen,position);

    // Increment cursor line.
    frame->cursor[0] += (strLen/frame->width)+1;
    frame->cursor[0] = frame->cursor[0]>frame->height-1?frame->height-1:frame->cursor[0];
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

*/
void UIframe_setLine(struct UIframe *frame, int line, char *str, int startPos, int strLen, float position)
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
        frame->screen[line][bufferPos++] = str[startPos];
    }

    // Print wrapped text
    if(frame->isTextWrappingEnabled && extraStrLen>0)UIframe_setLine(frame,line+1,str,startPos+printableStrLen,extraStrLen,0);
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
