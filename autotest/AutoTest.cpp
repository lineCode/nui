// AutoTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestUtil.h"


#include "TestGdiUtil.h"

// Base
#include "TestDataTypes.h"
#include "TestReflect.h"
#include "TestCore.h"
#include "TestDelayedRelease.h"

// Data
#include "TestString.h"
#include "TestXmlDataReader.h"
#include "TestModule.h"
#include "TestBuffer.h"
#include "TestFileMapping.h"
#include "TestZip.h"
#include "TestListT.h"
#include "TestArrayT.h"
#include "TestFileSystem.h"
#include "TestStringBundle.h"

// Util
#include "TestFileUtil.h"

// Ui
#include "TestEvent.h"
#include "TestFrameBase.h"
#include "TestMsgLoop.h"
#include "TestWindowBase.h"
#include "TestTimerSrv.h"

// Parser
#include "TestFrameBaseParser.h"
#include "TestFrameParser.h"


int _tmain(int argc, _TCHAR* argv[])
{
    srand(static_cast<unsigned int>(time(0)));

    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&token, &input, 0);

    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    NInstPtr<NCore> core(MemToolParam);
    core->DestroyCore();

    Gdiplus::GdiplusShutdown(token);

    printf("All Tests Finished.");
    getchar();
    return result;
}

