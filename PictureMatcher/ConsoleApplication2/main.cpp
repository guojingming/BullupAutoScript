#include <windows.h>  
#include <vector>  
#include <stdio.h>  
#include <stdlib.h>
#include <TCHAR.H>  
#include <iostream>  
#include <string>  
#include <atltrace.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>

using namespace std;
using namespace cv;

HBITMAP get_window_screen(LPWSTR caption_name, int& window_x, int& window_y);
int SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName);
CvPoint getNextMinLoc(IplImage* result, int templatWidth, int templatHeight, double maxValIn, CvPoint lastLoc);
void SafeResetSizeOfRect(IplImage* src, CvRect& rect);
int get_template_location(const char * window_caption, const char * template_path, int& window_x, int& window_y, int& template_in_window_x, int& template_in_window_y);

/*
@author:		about:blank
@mail:		1043840980@qq.com
@qq:			1043840980
-----------------------------
Usage:

./PictureMatcher.exe <window_caption> <template_path> 

Example:

./PictureMatcher.exe "League of Legends" "C:/User/Public/play_btn.png"

*/
int main(int argc, char* argv[]){
	int window_x, window_y, template_x, template_y;
	//get_template_location("League of Legends", argv[1], window_x, window_y, template_x, template_y);
	get_template_location("League of Legends", "C:/Users/Public/Bullup/auto_program/resources/china/play.bmp", window_x, window_y, template_x, template_y);

	printf("{ \"window_x\": %d,\"window_y\": %d,\"template_x\": %d,\"template_y\": %d }", window_x, window_y, template_x, template_y);
	
	//system("pause");
	return 0;
}

int get_template_location(const char * window_caption, const char * template_path, int& window_x, int& window_y, int& template_in_window_x, int& template_in_window_y) {
	wchar_t wtext[40];
	mbstowcs(wtext, window_caption, strlen(window_caption) + 1);//Plus null
	LPWSTR window_caption_lpwstr = wtext;
	HBITMAP hbitmap = get_window_screen(window_caption_lpwstr, window_x, window_y);
	SaveBitmapToFile(hbitmap, L"C:/Users/Public/Bullup/screen_shot.png");
	IplImage*src, *templat, *result, *show;
	int srcW, templatW, srcH, templatH, resultW, resultH;
	//加载源图像
	src = cvLoadImage("C:/Users/Public/Bullup/screen_shot.png", CV_LOAD_IMAGE_GRAYSCALE);
	//用于显示结果

	//加载模板图像
	templat = cvLoadImage(template_path, CV_LOAD_IMAGE_GRAYSCALE);
	srcW = src->width;
	srcH = src->height;
	templatW = templat->width;
	templatH = templat->height;
	//计算结果矩阵的大小
	resultW = srcW - templatW + 1;
	resultH = srcH - templatH + 1;
	//创建存放结果的空间
	result = cvCreateImage(cvSize(resultW, resultH), 32, 1);
	double minVal, maxVal;
	CvPoint minLoc, maxLoc;
	//调用模板匹配函数
	cvMatchTemplate(src, templat, result, CV_TM_SQDIFF);
	//查找最相似的值及其所在坐标
	cvMinMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, NULL);
	template_in_window_x = minLoc.x;
	template_in_window_y = minLoc.y;
	//printf("minVal  %f   maxVal %f\n ", minVal, maxVal);
	////显示结果
	//printf("%d %d", minLoc.x, minLoc.y);
	return 0;
}

//后台抓图   2017/9/3
HBITMAP get_window_screen(LPWSTR caption_name, int &window_x, int& window_y) {
	HWND dest_window = FindWindow(NULL, caption_name);
	//const char *abc = "001607EE";
	//HWND dest_window = (HWND)strtoul(abc, NULL, 16);	//16进制
	HDC dest_DC = GetDC(dest_window);
	HDC mem_DC = CreateCompatibleDC(dest_DC);
	RECT rect;
	BYTE*   Data;
	HBITMAP h_bitmap;
	GetWindowRect(dest_window, &rect);
	window_x = rect.left;
	window_y = rect.top;
	BITMAPINFO   bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bi.bmiHeader.biWidth = GetSystemMetrics(SM_CXSCREEN);
	bi.bmiHeader.biHeight = GetSystemMetrics(SM_CYSCREEN);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	h_bitmap = CreateDIBSection(mem_DC, &bi, DIB_RGB_COLORS, (void**)&Data, NULL, 0);
	SelectObject(mem_DC, h_bitmap);
	BitBlt(mem_DC, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, dest_DC, 0, 0, SRCCOPY);
	ReleaseDC(NULL, dest_DC);
	DeleteDC(mem_DC);
	return h_bitmap;
}

int SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName) {
	HDC            hDC; //设备描述表  
	int            iBits;//当前显示分辨率下每个像素所占字节数  
	WORD           wBitCount;//位图中每个像素所占字节数      
	DWORD          dwPaletteSize = 0;//定义调色板大小  
	DWORD          dwBmBitsSize;//位图中像素字节大小  
	DWORD          dwDIBSize;// 位图文件大小  
	DWORD          dwWritten;//写入文件字节数  
	BITMAP         Bitmap;//位图结构  
	BITMAPFILEHEADER   bmfHdr;   //位图属性结构     
	BITMAPINFOHEADER   bi;       //位图文件头结构  
	LPBITMAPINFOHEADER lpbi;     //位图信息头结构     指向位图信息头结构  
	HANDLE          fh;//定义文件句柄  
	HANDLE            hDib;//分配内存句柄  
	HANDLE            hPal;//分配内存句柄  
	HANDLE          hOldPal = NULL;//调色板句柄    
	//计算位图文件每个像素所占字节数     
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	else if (iBits <= 32)
		wBitCount = 24;
	//计算调色板大小     
	if (wBitCount <= 8)
		dwPaletteSize = (1 << wBitCount) *sizeof(RGBQUAD);
	//设置位图信息头结构     
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	//为位图内容分配内存     
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	if (lpbi == NULL) {
		return 0;
	}
	*lpbi = bi;
	// 处理调色板  
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal) {
		hDC = GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值     
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,
		(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
	//恢复调色板        
	if (hOldPal) {
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}
	//创建位图文件         
	fh = CreateFile(lpFileName, GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;
	// 设置位图文件头     
	bmfHdr.bfType = 0x4D42;  // "BM"     
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	// 写入位图文件头     
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容     
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	//清除        
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return 1;
}

//获取矩形框左上角
CvPoint getNextMinLoc(IplImage* result, int templatWidth, int templatHeight, double maxValIn, CvPoint lastLoc) {
	int y, x;
	int startY, startX, endY, endX;
	//计算大矩形的左上角坐标
	startY = lastLoc.y - templatHeight;
	startX = lastLoc.x - templatWidth;
	//计算大矩形的右下角的坐标  大矩形的定义 可以看视频的演示
	endY = lastLoc.y + templatHeight;
	endX = lastLoc.x + templatWidth;
	//不允许矩形越界
	startY = startY < 0 ? 0 : startY;
	startX = startX < 0 ? 0 : startX;
	endY = endY > result->height - 1 ? result->height - 1 : endY;
	endX = endX > result->width - 1 ? result->width - 1 : endX;
	//将大矩形内部 赋值为最大值 使得 以后找的最小值 不会位于该区域  避免找到重叠的目标
	for (y = startY; y<endY; y++) {
		for (x = startX; x<endX; x++) {
			cvSetReal2D(result, y, x, maxValIn);
		}
	}
	double minVal, maxVal;
	CvPoint minLoc, maxLoc;
	//查找result中的最小值 及其所在坐标
	cvMinMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, NULL);
	return minLoc;
}

// 安全重置矩形大小  
void SafeResetSizeOfRect(IplImage* src, CvRect& rect) {
	rect.x = rect.x < 0 ? 0 : rect.x;
	rect.y = rect.y < 0 ? 0 : rect.y;
	rect.width = rect.width < 0 ? 0 : rect.width;
	rect.height = rect.height < 0 ? 0 : rect.height;
	if (rect.x > src->width || rect.y > src->height) {
		rect = cvRect(0, 0, src->width, src->height);
	}
	rect.width = std::min(rect.width, src->width - rect.x);
	rect.height = std::min(rect.height, src->height - rect.y);
}
