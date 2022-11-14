#ifndef KEYS_H
#define KEYS_H

#define cuint8 const unsigned char

// Key encodings for use with the KEYS global variable.
// Use like this: if (KEYS[keys::esc]) {...}
namespace keys {
    cuint8 esc = 27;
    cuint8 enter = 13;

    cuint8 a = 'a';
    cuint8 b = 'b';
    cuint8 c = 'c';
    cuint8 d = 'd';
    cuint8 e = 'e';
    cuint8 f = 'f';
    cuint8 g = 'g';
    cuint8 h = 'h';
    cuint8 i = 'i';
    cuint8 j = 'j';
    cuint8 k = 'k';
    cuint8 l = 'l';
    cuint8 m = 'm';
    cuint8 n = 'n';
    cuint8 o = 'o';
    cuint8 p = 'p';
    cuint8 q = 'q';
    cuint8 r = 'r';
    cuint8 s = 's';
    cuint8 t = 't';
    cuint8 u = 'u';
    cuint8 v = 'v';
    cuint8 w = 'w';
    cuint8 x = 'x';
    cuint8 y = 'y';
    cuint8 z = 'z';

    cuint8 zero = '0';
    cuint8 one = '1';
    cuint8 two = '2';
    cuint8 three = '3';
    cuint8 four = '4';
    cuint8 five = '5';
    cuint8 six = '6';
    cuint8 seven = '7';
    cuint8 eight = '8';
    cuint8 nine = '9';

    cuint8 space = ' ';

    cuint8 leftArrow = -1;
    cuint8 rightArrow = -1;
    cuint8 upArrow = -1;
    cuint8 downArrow = -1;
}

#endif
