#ifndef __UI_C__
#define __UI_C__

struct UIframe{
    int width,height;
    int cursor[2];
    int isTextWrappingEnabled;
    char lborder,rborder,roof,floor;
    char **screen;
};

void UIframe_init(struct UIframe *frame, int width, int height, char lborder, char rborder, char roof, char floor, int textWrappingEnabled);
void UIframe_init2(struct UIframe *frame, int width, int height);
void UIframe_addLine(struct UIframe *frame, char *str, int strLen, float position);
void UIframe_setLine(struct UIframe *frame, int line, char *str, int strLen, float position);
void UIframe_print(struct UIframe *frame, int x, int y);
void UI_header();
#endif // __UI_C__