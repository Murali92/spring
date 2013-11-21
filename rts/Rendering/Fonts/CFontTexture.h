#ifndef _CFONTTEXTURE_H
#define _CFONTTEXTURE_H
#include <map>
#include <list>
#include <string>


struct FT_FaceRec_;
struct FT_LibraryRec_;
typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_LibraryRec_* FT_Library;


class texture_size_exception : public std::exception
{
};


struct IGlyphRect { //FIXME use SRect
	IGlyphRect():
		x(0),y(0),
		w(0),h(0) {
	};

	IGlyphRect(float _x,float _y,float _w,float _h):
		x(_x),y(_y),
		w(_w),h(_h) {
	};

	float x0() const {
		return x;
	};
	float x1() const {
		return x+w;
	};
	float y0() const {
		return y;
	};
	float y1() const {
		return y+h;
	};

	float x,y;
	float w,h;
};

struct GlyphInfo {
	GlyphInfo()
	: advance(0)
	, height(0)
	, descender(0)
	, index(0)
	{ };

	IGlyphRect size;
	IGlyphRect texCord;
	float advance, height, descender;
	char32_t index;
};

class LanguageBlock
{
public:
	LanguageBlock(char32_t start,char32_t end):
		start(start),
		end(end)
	{
		glyphs=new GlyphInfo[end-start];// [start,end)
	}

	~LanguageBlock(){ delete[] glyphs; };

	inline bool OnThisPlane(char32_t ch) const {return start<=ch && ch<end;}
	GlyphInfo& Get(char32_t ch){return glyphs[ch-start];}
private:
	char32_t start,end;
	GlyphInfo* glyphs;
};

/**
This class just store glyphs and load new glyphs if requred
It works with image and don't care about rendering these glyphs
It works only and only with UTF32 chars
**/
class CFontTexture
{
public:
	CFontTexture(const std::string& fontfile, int size, int outlinesize, float  outlineweight);
	virtual ~CFontTexture();
public:
	int GetTextureWidth() const;
	int GetTextureHeight() const;
	int   GetOutlineWidth() const;
	float GetOutlineWeight() const;
	float GetLineHeight() const;
	float GetDescender() const;

	int GetTexture() const;
	// Get or load a glyph
	const GlyphInfo& GetGlyph(char32_t ch);

protected:
	const FT_Face& GetFace() const;

protected:
	float GetKerning(char32_t lchar, char32_t rchar);
	float GetKerning(const GlyphInfo& lgl,const GlyphInfo& rgl);

private:
	int outlineSize;
	float outlineWeight;
	float lineHeight;
	float fontDescender;

	float normScale;

private:
	unsigned int texWidth,texHeight;
	unsigned int texture;
	//! Create a new texture and copy all data from the old one
	//! Throw texture_size_exception if image's width or height is bigger than 2048
	void CreateTexture(int w,int h);
	//! Create a new texture
	void ResizeTexture(int w,int h);
	//! Copy glyph pixels on the texture
	void Update(const unsigned char* pixels,int x,int y,int w,int h);
	//! Fill given rect by null bytes
	void Clear(int x,int y,int w,int h);

private:
	FT_Library library;
	FT_Face face;
	unsigned char* faceDataBuffer;

private:
	std::map<char32_t,LanguageBlock*> blocks;    //!UTF32 block start -> LanguageBlock
	//! Load all chars in block's range
	LanguageBlock* LoadBlock(char32_t start,char32_t end);
	void LoadGlyph(LanguageBlock* block, char32_t ch);

private:
	struct Row {
		Row(int _ypos,int _height):
			position(_ypos),
			height(_height),
			wight(0) {
		};

		int position;
		int height;
		int wight;
	};
	std::list<Row> imageRows;
	int nextRowPos;

	Row* FindRow(int glyphWidth, int glyphHeight);
	Row* AddRow(int glyphWidth, int glyphHeight);
	//! Try to find a place where can be placed a glyph with given size
	//! If it is imposible, resize the texture and try it again
	IGlyphRect AllocateGlyphRect(int glyphWidth, int glyphHeight);
};

#endif // CFONTTEXTURE_H
