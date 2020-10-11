#pragma once

#ifndef Q_BMP
#define Q_BMP

#include <afxwin.h>
#include <vector>

/** 
 * Print quicker to screen by first saving pixels to a BMP on memory,
 * and only then copying it all to screen.
 */
class QuickBMP
{
public:
	const CRect& rect; // client rectangle, supplied by user

	QuickBMP(CDC* pDC, const CRect& rect, COLORREF color=RGB(0, 0, 0)) : 
		pDC(pDC),
		rect(rect),
		memoryDC(new CDC),
		bitmap(new CBitmap),
		color_v(rect.Width() * (1 + rect.Height()), color) // TODO: think about removing one line
	{
		memoryDC->CreateCompatibleDC(pDC);
	}
	~QuickBMP() {
		bitmap->DeleteObject();
		memoryDC->DeleteDC();
		delete memoryDC;
		delete bitmap;
	}
	void setPixel(int x, int y, COLORREF color) {
		color_v[y * rect.Width() + x] = color;
	}
	void print() {
		bitmap->CreateBitmap(rect.Width(), rect.Height(), 1, 32, &color_v[0]);
		memoryDC->SelectObject(bitmap); // Bitmaps can only be selected into memory DC's
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), memoryDC, 0, 0, SRCCOPY); // copy from memory to screen
	}

private:
	CDC* pDC; // screen device context
	CDC* memoryDC; // memory-based device context
	CBitmap* bitmap;
	std::vector<COLORREF> color_v;  // stores pixel colors in a c-style 2-dim array:
									// color_v[x][y] == color_v[y*w + x].
									// initialized to all-white.
};

#endif
