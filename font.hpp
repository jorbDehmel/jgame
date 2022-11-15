#include "smartSprite.hpp"

#ifndef FONT_H
#define FONT_H

void write(SmartSprite *font, char *what, Surface *where, bool first = true) {
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
    default:
        font->frame(concat(what[0], (char*)".bmp"));
        stamp(font, where);
        break;
    }

    font->xInc(font->W() + 1);

    write(font, &what[1], where, false);
    return;
}

#endif
