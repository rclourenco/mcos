#ifndef _MOUSE_H
 #define _MOUSE_H

int MouseOpen();
int MouseClose();

int MouseX();
int MouseY();
int MouseStatus();
void MouseShow();
void MouseHide();
void MouseGet(int *x, int *y, int *b);

#endif