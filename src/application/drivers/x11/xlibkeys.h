//this code is from bsnes and slightly modified

unsigned char _scancode[256];

enum XScancode { UNMAPPED,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    Pause, Tilde,
    Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
    Dash, Equal, Backspace,
    Insert, Delete, Home, End, PageUp, PageDown,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    LeftBracket, RightBracket, Backslash, Semicolon, Apostrophe, Comma, Period, Slash,
    Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9, Keypad0,
    Separator, Enter, Add, Subtract, Multiply, Divide,
    Up, Down, Left, Right,
    Tab, Return, Spacebar, Menu,
    LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt
};

void x_poll(Display* display, i16* table) {
    if(!display) return;

    char state[32];
    XQueryKeymap(display, state);

    #define key(id) table[id]
    #define pressed(id) (bool)(state[_scancode[id] >> 3] & (1 << (_scancode[id] & 7)))

    key(F1) = pressed(F1);
    key(F2) = pressed(F2);
    key(F3) = pressed(F3);
    key(F4) = pressed(F4);
    key(F5) = pressed(F5);
    key(F6) = pressed(F6);
    key(F7) = pressed(F7);
    key(F8) = pressed(F8);
    key(F9) = pressed(F9);
    key(F10) = pressed(F10);
    key(F11) = pressed(F11);
    key(F12) = pressed(F12);

    key(Pause) = pressed(Pause);
    key(Tilde) = pressed(Tilde);

    key(Num1) = pressed(Num1);
    key(Num2) = pressed(Num2);
    key(Num3) = pressed(Num3);
    key(Num4) = pressed(Num4);
    key(Num5) = pressed(Num5);
    key(Num6) = pressed(Num6);
    key(Num7) = pressed(Num7);
    key(Num8) = pressed(Num8);
    key(Num9) = pressed(Num9);
    key(Num0) = pressed(Num0);

    key(Dash) = pressed(Dash);
    key(Equal) = pressed(Equal);
    key(Backspace) = pressed(Backspace);

    key(Insert) = pressed(Insert);
    key(Delete) = pressed(Delete);
    key(Home) = pressed(Home);
    key(End) = pressed(End);
    key(PageUp) = pressed(PageUp);
    key(PageDown) = pressed(PageDown);

    key(A) = pressed(A);
    key(B) = pressed(B);
    key(C) = pressed(C);
    key(D) = pressed(D);
    key(E) = pressed(E);
    key(F) = pressed(F);
    key(G) = pressed(G);
    key(H) = pressed(H);
    key(I) = pressed(I);
    key(J) = pressed(J);
    key(K) = pressed(K);
    key(L) = pressed(L);
    key(M) = pressed(M);
    key(N) = pressed(N);
    key(O) = pressed(O);
    key(P) = pressed(P);
    key(Q) = pressed(Q);
    key(R) = pressed(R);
    key(S) = pressed(S);
    key(T) = pressed(T);
    key(U) = pressed(U);
    key(V) = pressed(V);
    key(W) = pressed(W);
    key(X) = pressed(X);
    key(Y) = pressed(Y);
    key(Z) = pressed(Z);

    key(LeftBracket) = pressed(LeftBracket);
    key(RightBracket) = pressed(RightBracket);
    key(Backslash) = pressed(Backslash);
    key(Semicolon) = pressed(Semicolon);
    key(Apostrophe) = pressed(Apostrophe);
    key(Comma) = pressed(Comma);
    key(Period) = pressed(Period);
    key(Slash) = pressed(Slash);

    key(Keypad1) = pressed(Keypad1);
    key(Keypad2) = pressed(Keypad2);
    key(Keypad3) = pressed(Keypad3);
    key(Keypad4) = pressed(Keypad4);
    key(Keypad5) = pressed(Keypad5);
    key(Keypad6) = pressed(Keypad6);
    key(Keypad7) = pressed(Keypad7);
    key(Keypad8) = pressed(Keypad8);
    key(Keypad9) = pressed(Keypad9);
    key(Keypad0) = pressed(Keypad0);

    key(Separator) = pressed(Separator);
    key(Enter) = pressed(Enter);
    key(Add) = pressed(Add);
    key(Subtract) = pressed(Subtract);
    key(Multiply) = pressed(Multiply);
    key(Divide) = pressed(Divide);

    key(Up) = pressed(Up);
    key(Down) = pressed(Down);
    key(Left) = pressed(Left);
    key(Right) = pressed(Right);

    key(Tab) = pressed(Tab);
    key(Return) = pressed(Return);
    key(Spacebar) = pressed(Spacebar);
    key(Menu) = pressed(Menu);

    key(LeftShift) = pressed(LeftShift);
    key(RightShift) = pressed(RightShift);
    key(LeftControl) = pressed(LeftControl);
    key(RightControl) = pressed(RightControl);
    key(LeftAlt) = pressed(LeftAlt);
    key(RightAlt) = pressed(RightAlt);

    #undef key
    #undef pressed
}

void x_init(Display *display) {
    if(!display) return;

    memset(&_scancode, 0, sizeof _scancode);
    #define assign(x, y) _scancode[x] = XKeysymToKeycode(display, y);

    assign(F1, XK_F1);
    assign(F2, XK_F2);
    assign(F3, XK_F3);
    assign(F4, XK_F4);
    assign(F5, XK_F5);
    assign(F6, XK_F6);
    assign(F7, XK_F7);
    assign(F8, XK_F8);
    assign(F9, XK_F9);
    assign(F10, XK_F10);
    assign(F11, XK_F11);
    assign(F12, XK_F12);

    assign(Pause, XK_Pause);
    assign(Tilde, XK_asciitilde);

    assign(Num0, XK_0);
    assign(Num1, XK_1);
    assign(Num2, XK_2);
    assign(Num3, XK_3);
    assign(Num4, XK_4);
    assign(Num5, XK_5);
    assign(Num6, XK_6);
    assign(Num7, XK_7);
    assign(Num8, XK_8);
    assign(Num9, XK_9);

    assign(Dash, XK_minus);
    assign(Equal, XK_equal);
    assign(Backspace, XK_BackSpace);

    assign(Insert, XK_Insert);
    assign(Delete, XK_Delete);
    assign(Home, XK_Home);
    assign(End, XK_End);
    assign(PageUp, XK_Prior);
    assign(PageDown, XK_Next);

    assign(A, XK_A);
    assign(B, XK_B);
    assign(C, XK_C);
    assign(D, XK_D);
    assign(E, XK_E);
    assign(F, XK_F);
    assign(G, XK_G);
    assign(H, XK_H);
    assign(I, XK_I);
    assign(J, XK_J);
    assign(K, XK_K);
    assign(L, XK_L);
    assign(M, XK_M);
    assign(N, XK_N);
    assign(O, XK_O);
    assign(P, XK_P);
    assign(Q, XK_Q);
    assign(R, XK_R);
    assign(S, XK_S);
    assign(T, XK_T);
    assign(U, XK_U);
    assign(V, XK_V);
    assign(W, XK_W);
    assign(X, XK_X);
    assign(Y, XK_Y);
    assign(Z, XK_Z);

    assign(LeftBracket, XK_bracketleft);
    assign(RightBracket, XK_bracketright);
    assign(Backslash, XK_backslash);
    assign(Semicolon, XK_semicolon);
    assign(Apostrophe, XK_apostrophe);
    assign(Comma, XK_comma);
    assign(Period, XK_period);
    assign(Slash, XK_slash);

    assign(Keypad0, XK_KP_0);
    assign(Keypad1, XK_KP_1);
    assign(Keypad2, XK_KP_2);
    assign(Keypad3, XK_KP_3);
    assign(Keypad4, XK_KP_4);
    assign(Keypad5, XK_KP_5);
    assign(Keypad6, XK_KP_6);
    assign(Keypad7, XK_KP_7);
    assign(Keypad8, XK_KP_8);
    assign(Keypad9, XK_KP_9);

    assign(Separator, XK_KP_Separator);
    assign(Add, XK_KP_Add);
    assign(Subtract, XK_KP_Subtract);
    assign(Multiply, XK_KP_Multiply);
    assign(Divide, XK_KP_Divide);
    assign(Enter, XK_KP_Enter);

    assign(Up, XK_Up);
    assign(Down, XK_Down);
    assign(Left, XK_Left);
    assign(Right, XK_Right);

    assign(Tab, XK_Tab);
    assign(Return, XK_Return);
    assign(Spacebar, XK_space);
    assign(Menu, XK_Menu);

    assign(LeftControl, XK_Control_L);
    assign(RightControl, XK_Control_R);
    assign(LeftAlt, XK_Alt_L);
    assign(RightAlt, XK_Alt_R);
    assign(LeftShift, XK_Shift_L);
    assign(RightShift, XK_Shift_R);

    #undef assign
}
