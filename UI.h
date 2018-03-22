#ifndef __UI_C__
#define __UI_C__

struct UIframe{
    int width,height;
    int cursor[2];
    int isTextWrappingEnabled, bordersEnabled;
    char lborder,rborder,roof,floor;
    char **screen;
};

void UIframe_init(struct UIframe *frame, int width, int height,
                    char lborder, char rborder, char roof, char floor, int textWrappingEnabled, int bordersEnabled);
void UIframe_addLine(struct UIframe *frame, char *str, int startPos, int strLen, float position);
void UIframe_setLine(struct UIframe *frame, int line, char *str, int startPos, int strLen, float position);
void UIframe_print(struct UIframe *frame, int x, int y);
void UIframe_flush(struct UIframe *frame);
void UI_header();
#endif // __UI_C__