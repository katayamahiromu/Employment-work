#pragma once
#include<string>
#include<Windows.h>

inline std::string WStringToUTF8(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();

	// UTF-8 で必要なバッファサイズを取得（終端NULL含む）
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1,
		nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0) return std::string();

	std::string result(size_needed - 1, 0); // 終端は除いて確保
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1,
		&result[0], size_needed, nullptr, nullptr);
	return result;
}

inline std::wstring StringToWString(const std::string& str)
{
	if (str.empty()) return std::wstring();

	int sizeNeeded = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wstr(sizeNeeded, 0);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], sizeNeeded);

	wstr.pop_back();
	return wstr;
}