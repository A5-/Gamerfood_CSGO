#pragma once

#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

class ISurface
{
public:
	void DrawSetColor( int r, int g, int b, int a );
	void DrawSetColor( Color col );
	void DrawFilledRect( int x0, int y0, int x1, int y1 );
	void DrawOutlinedRect( int x0, int y0, int x1, int y1 );
	void DrawLine( int x0, int y0, int x1, int y1 );
	void DrawPolyLine( int* px, int* py, int numPoints );
	void DrawSetTextFont( HFont font );
	void DrawSetTextColor( int r, int g, int b, int a );
	void DrawSetTextColor( Color col );
	void DrawSetTextPos( int x, int y );
	void DrawPrintText( const wchar_t* text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT );
	void DrawSetTextureRGBA( int id, const unsigned char* rgba, int wide, int tall );
	void DrawSetTexture( int id );
	int CreateNewTextureID( bool procedural = false );
	HFont Create_Font();
	bool SetFontGlyphSet( HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0 );
	void GetTextSize( HFont font, const wchar_t* text, int& wide, int& tall );
	void DrawOutlinedCircle( int x, int y, int radius, int segments );
	void DrawTexturedPolygon( int n, Vertex_t* pVertice, bool bClipVertices = true );
};
