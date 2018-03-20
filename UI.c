#include<stdio.h>
#include<stdlib.h>
#include"helper.h"
#include"UI.h"

/*
    This function is used to initialise the UIframe structure and allocate the memory for its buffer.
*/
void UIframe_init(struct UIframe *frame, int width, int height, char lborder, char rborder, char roof, char floor, int textWrappingEnabled)
{
    // Reset internal width & height to compensate for border and roof
    width = width-2;
    height = height-2;

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
    frame->cursor[0]=0;
    frame->cursor[1]=0;

    // Fill the screen with spaces.
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
void UIframe_init2(struct UIframe *frame, int width, int height)
{
    // Check for valid data.
    if(width<0||height<0||frame==NULL)return;
    UIframe_init(frame, width, height, '|', '|', '-', '-', 1);
}

void UIframe_addLine(struct UIframe *frame, char *str, int strLen, float position)
{
    UIframe_setLine(frame,frame->cursor[0],str,strLen,position);
}
void UIframe_setLine(struct UIframe *frame, int line, char *str, int strLen, float position)
{
    --strLen;
    // Check for valid data.
    if(frame==NULL||str==NULL)return;
    // Make sure position is a value between 0 and 1 and clip if if not.
    position = clip(position, 0, 1);

    // Calculate the no. of spaces needed in this frame line.
    int spacesNeeded = (frame->width - clip(strLen, 0, frame->width))*position;

    // Print the string to buffer.
    int i,p=0;
    for(i=0;i<frame->width;++i)
    {
        if(i<spacesNeeded || i>(spacesNeeded+strLen))frame->screen[line][i] = ' ';
        else frame->screen[line][i] = str[p++];
    }

    // Increment cursor to next line.
    frame->cursor[0]=line<frame->height-1?line+1:line;
    frame->cursor[1]=0;
}

void UIframe_print(struct UIframe *frame, int x, int y)
{
    int i,j;
    // Move cursor to y pos. from the current pos.
    for(i=0;i<y;++i)printf("\n");

    // Move cursor to x pos.
    for(j=0;j<x;++j)printf(" ");
    // Print the roof.
    for(i=0;i<frame->width+2;++i)printf("%c",frame->roof);
    printf("\n");
    
    for(i=0;i<frame->height;++i)
    {
        // Move cursor to x pos.
        for(j=0;j<x;++j)printf(" ");
        // Print border.
        printf("%c",frame->rborder);
        for(j=0;j<frame->width;++j)
        {
            printf("%c",frame->screen[i][j]);
        }
        // Print border.
        printf("%c",frame->lborder);
        printf("\n");
    }

    // Move cursor to x pos.
    for(j=0;j<x;++j)printf(" ");
    // Print the floor.
    for(i=0;i<frame->width+2;++i)printf("%c",frame->floor);
    printf("\n");
}
void UI_header()
{
    printf("Welcome");
}
