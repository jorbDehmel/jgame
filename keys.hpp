#ifndef KEYS_H
#define KEYS_H

// Key encodings for use with the KEYS global variable.
// Use like this: if (KEYS[keys::esc]) {...}
namespace keys {
    int esc = 27;
    int enter = 13;

    int a = 'a';
    int b = 'b';
    int c = 'c';
    int d = 'd';
    int e = 'e';
    int f = 'f';
    int g = 'g';
    int h = 'h';
    int i = 'i';
    int j = 'j';
    int k = 'k';
    int l = 'l';
    int m = 'm';
    int n = 'n';
    int o = 'o';
    int p = 'p';
    int q = 'q';
    int r = 'r';
    int s = 's';
    int t = 't';
    int u = 'u';
    int v = 'v';
    int w = 'w';
    int x = 'x';
    int y = 'y';
    int z = 'z';

    int zero = '0';
    int one = '1';
    int two = '2';
    int three = '3';
    int four = '4';
    int five = '5';
    int six = '6';
    int seven = '7';
    int eight = '8';
    int nine = '9';

    int space = ' ';

    int leftArrow = 1073741904;
    int rightArrow = 1073741903;
    int upArrow = 1073741906;
    int downArrow = 1073741905;

    int shift = 1073742053;
}

#endif
