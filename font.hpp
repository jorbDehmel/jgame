#include "smartSprite.hpp"

#ifndef FONT_H
#define FONT_H

/////////////////////////////////////

// Write using a specific type of font sprite
void write(SmartSprite *font, char *what, Surface *where, u16 returnX, bool first = true) {
    if (what == nullptr || what[0] == '\0') {
        return;
    }

    // write what[0]
    switch (what[0]) {
    case ' ':
        break;
    case '!':
        font->frame((char*)"_exclamation.bmp");
        stamp(font, where);
        break;
    case '?':
        font->frame((char*)"_question.bmp");
        stamp(font, where);
        break;
    case '.':
        font->frame((char*)"_period.bmp");
        stamp(font, where);
        break;
    case ',':
        font->frame((char*)"_comma.bmp");
        stamp(font, where);
        break;
    case '(':
        font->frame((char*)"_open_par.bmp");
        stamp(font, where);
        break;
    case ')':
        font->frame((char*)"_close_par.bmp");
        stamp(font, where);
        break;
    case '[':
        font->frame((char*)"_open_sq_bracket.bmp");
        stamp(font, where);
        break;
    case ']':
        font->frame((char*)"_close_sq_bracket.bmp");
        stamp(font, where);
        break;
    case '{':
        font->frame((char*)"_open_cu_bracket.bmp");
        stamp(font, where);
        break;
    case '}':
        font->frame((char*)"_close_cu_bracket.bmp");
        stamp(font, where);
        break;
    case '/':
        font->frame((char*)"_slash.bmp");
        stamp(font, where);
        break;
    case ':':
        font->frame((char*)"_colon.bmp");
        stamp(font, where);
        break;
    case ';':
        font->frame((char*)"_semicolon.bmp");
        stamp(font, where);
        break;
    case '\n':
        font->yInc(font->H() + 1);
        font->X(0);
        font->xInc(-font->W() - 1);
        break;
    default:
        font->frame(concat(what[0], (char*)".bmp"));
        stamp(font, where);
        break;
    }

    font->xInc(font->W() + 1);
    if (font->X() + font->W() + 1 >= returnX) {
        font->yInc(font->H() + 1);
        font->X(0);
    }

    write(font, &what[1], where, returnX, false);

    if (first) {
        font->X(-font->W());
    }

    return;
}

/////////////////////////////////////

#endif
