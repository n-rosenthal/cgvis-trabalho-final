#ifndef DEJAVUFONT_H
#define DEJAVUFONT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t codepoint;
    float kerning;
} kerning_t;

typedef struct {
    uint32_t codepoint;
    int width, height;
    int offset_x, offset_y;
    float advance_x, advance_y;
    float s0, t0, s1, t1;
    size_t kerning_count;
    kerning_t kerning[1];   // flexible array, but careful
} texture_glyph_t;

typedef struct {
    size_t tex_width;
    size_t tex_height;
    size_t tex_depth;
    unsigned char tex_data[65536];
    float size;
    float height;
    float linegap;
    float ascender;
    float descender;
    size_t glyphs_count;
    texture_glyph_t glyphs[96];
} texture_font_t;

// Declaration only – the actual data is in dejavufont.cpp
extern texture_font_t dejavufont;

#ifdef __cplusplus
}
#endif

#endif // DEJAVUFONT_H