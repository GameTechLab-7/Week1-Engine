#pragma once

#include <string>
#include "ContainerAllocator.h"
#include "Core/HAL/PlatformType.h"

/*
# TCHAR가 ANSICHAR인 경우
1. const ANSICHAR* 로 FString 생성
2. std::string에서 FString 생성

# TCHAR가 WIDECHAR인 경우
1. const ANSICHAR* 로 FString 생성
1. const WIDECHAR* 로 FString 생성
2. std::wstring에서 FString 생성
3. std::string에서 FString 생성
*/

class FString
{
private:
    using BaseStringType = std::basic_string<
        TCHAR,
        std::char_traits<TCHAR>,
        FDefaultAllocator<TCHAR>
    >;

    BaseStringType PrivateString;

public:
    FString() {}

#if IS_WIDECHAR
private:
    static std::wstring ConvertWideChar(const ANSICHAR* NarrowStr);

public:
    FString(const std::wstring& InString) : PrivateString(InString) {}
    FString(const std::string& InString) : PrivateString(ConvertWideChar(InString.c_str())) {}
    FString(const WIDECHAR* InString) : PrivateString(InString) {}
    FString(const ANSICHAR* InString) : PrivateString(ConvertWideChar(InString)) {}
#else
public:
    FString(const std::string& InString) : PrivateString(InString) {}
    FString(const ANSICHAR* InString) : PrivateString(InString) {}
#endif

    static FString FromInt(int32 Num);

public:
    // bool Equals(const FString& Other) const;
};
