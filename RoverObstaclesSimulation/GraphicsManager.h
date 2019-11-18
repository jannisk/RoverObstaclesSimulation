#pragma once
class GraphicsManager
{
	HWND m_hWnd;
	HPEN m_pen;
	HBRUSH m_hPurpleBrush;

public:
	GraphicsManager();
	GraphicsManager(HWND);
	~GraphicsManager();
	
	
	void FieldPaint(HWND );
	void SetBkgColor(HWND);
};

