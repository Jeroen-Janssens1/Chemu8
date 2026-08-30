#include "Graphics.h"
#include <iostream>
#include <string>

void Graphics::SetPixel(unsigned char xCoord, unsigned char yCoord)
{
	m_Screen[yCoord * m_ScreenWidth + xCoord];
}

#ifdef _DEBUG
void Graphics::PrintScreenVals()
{
	std::cout << std::endl << "Screen Values (0 = off, 1 = on)" << std::endl;
	for (int y = 0; y < m_ScreenHeight; y++)
	{
		for (int x = 0; x < m_ScreenWidth; x++)
			std::cout << std::to_string(m_Screen[y * m_ScreenWidth + x]);
		std::cout << std::endl;
	}
}
#endif