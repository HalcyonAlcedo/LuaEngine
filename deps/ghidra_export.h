#pragma once
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned long long undefined8;
typedef unsigned char byte;
typedef unsigned long long ulonglong;
typedef long long longlong;
typedef unsigned int uint;
typedef unsigned short ushort;

namespace MH {
    namespace Quest {
        static undefined(*ErrorCheck)(undefined, undefined, undefined, undefined, undefined4, undefined8, undefined8, undefined2, undefined4) = (undefined(*)(undefined, undefined, undefined, undefined, undefined4, undefined8, undefined8, undefined2, undefined4))0x141144830;
        static undefined(*CheckProgress)() = (undefined(*)())0x14134EFE0;
        static undefined(*GetCategory)() = (undefined(*)())0x1419751B0;
        static void* OptionalIdList = (void*)0x14324FAC0;
        static bool(*CheckComplete)(longlong, uint) = (bool(*)(longlong, uint))0x14134EF50;
        static undefined8(*OptionalCount)() = (undefined8(*)())0x141355C50;
        static uint(*OptionalAt)(undefined*, int) = (uint(*)(undefined*, int))0x141355C40;
        static undefined(*IsMasterRank)() = (undefined(*)())0x141976110;
        static void* GlobalOptionalQuestList = (void*)0x145010ae8; // 变动 未调用
        static undefined(*StarCategoryCheck)() = (undefined(*)())0x140F2C320;
        namespace QuestData {
            static void* ResourceVtable = (void*)0x1433EA8A0;
            static undefined(*ResourceFunc)() = (undefined(*)())0x141A0FF50;
            static void* ResourcePtr = (void*)0x145009308; // 变动，疑似143BD9AC0
        }
        namespace QuestNoList {
            static void* ResourceVtable = (void*)0x142F85B50;
            static undefined(*ResourceFunc)() = (undefined(*)())0x140478520; // 变动，未调用
            static void* ResourcePtr = (void*)0x144cb5e40; // 变动，疑似1433333C8
        }
    }
    namespace Player {
        static void* BasePtr = (void*)0x145013950;
        static undefined(*PlayerBasePlot)() = (undefined(*)())0x1450139A0;
        static void(*Effects)(undefined*, int, int) = (void(*)(undefined*, int, int))0x140AE0D70;
        static undefined(*Visual)() = (undefined(*)())0x141F9EEC6;
        static void* PlayerDataHandlePlot = (void*)0x143C21EA0;
        static void(*CallLmt)(undefined*, int, int) = (void(*)(undefined*, int, int))0x141C014B0;
        static void(*ActionFrameSpeed)(undefined8*) = (void(*)(undefined8*))0x142228F90;
        static undefined* (*GetPlayer)(undefined*) = (undefined * (*)(undefined*))0x141B8DBB0;
        static undefined(*HitPtr)(undefined*, undefined*) = (undefined(*)(undefined*, undefined*))0x141F5180B;
    }
    namespace EnvironmentalBiological {
        static undefined* (*ctor)(undefined*, undefined4, undefined4) = (undefined * (*)(undefined*, undefined4, undefined4))0x141D285C0;
        static void(*dtor)(undefined8*) = (void(*)(undefined8*))0x141D289D0;
    }
    namespace World {
        static undefined8(*MapClockLocal)(float*, float) = (undefined8(*)(float*, float))0x140AE7170;
        static undefined(*Message)() = (undefined(*)())0x144F87FF0;
        static undefined8(*ActionFrameSpeed)(undefined8*) = (undefined8(*)(undefined8*))0x142228F50;
        static undefined(*Screen)(undefined*) = (undefined(*)(undefined*))0x14236D070;
    }
    namespace Shlp {
        static void* (*GetShlp)(void*, int) = (void* (*)(void*, int))0x140F7DC00;
        static void(*CallShlp)(void*, void*, void*, void*) = (void(*)(void*, void*, void*, void*))0x141AA74A0;
        static void*(*ctor)() = (void* (*)())0x141AA74A0;
        static void(*dtor)(void*) = (void(*)(void*))0x1422282A0;
    }
    namespace Monster {
        static void(*dtor)(undefined8*) = (void(*)(undefined8*))0x141CA47E0;
        static void* vptr = (void*)0x14346A4BB;
        namespace DamageBehavior {
            static undefined(*NextAction)() = (undefined(*)())0x1413973B0;
        }
        static bool(*LaunchAction)(undefined*, uint) = (bool(*)(undefined*, uint))0x141CC5360;
        static undefined(*MotionFromId)() = (undefined(*)())0x141BFF880;
        static undefined* (*ctor)(undefined*, undefined4, undefined4) = (undefined * (*)(undefined*, undefined4, undefined4))0x141CA1F00;
        namespace SoftenTimers {
            static undefined(*WoundPartLocal)() = (undefined(*)())0x140AFC770;
        }
        static undefined(*GenerateFilePaths)() = (undefined(*)())0x141CCF6C0;
    }
    namespace Weapon {
        namespace UI {
            static undefined(*CalcAwakenedElement)() = (undefined(*)())0x14197D900;
            static uint(*CalcRawBloat)(uint, uint) = (uint(*)(uint, uint))0x14197D8D0;
            static void* RawBloatMultipliers = (void*)0x1433B8890;
            namespace Unkn {
                static undefined(*ConditionalElementalBloat)() = (undefined(*)())0x14191B480;
            }
            static undefined(*CalcElementalBloat)() = (undefined(*)())0x14197D8F0;
        }
        static void* (*ChangeWeapon)(void*, int, int) = (void* (*)(void*, int, int))0x141F5A410;
        static void* (*CompleteChangeWeapon)(void*, int, int) = (void* (*)(void*, int, int))0x142041700;
        static void* (*RefreshEquip)(void*) = (void* (*)(void*))0x141258800;
        static void(*Hit)() = (void(*)())0x141F71B21;
        static undefined(*WeaponOrnaments)() = (undefined(*)())0x1421136F1;
        static undefined(*MainWeaponPtr)() = (undefined(*)())0x142119129;
        static undefined(*SecondaryWeaponPtr)() = (undefined(*)())0x1421522FD;
        static undefined(*PartWeaponPtr)() = (undefined(*)())0x14211B036;
    }
    namespace EmSetter {
        static undefined8(*CreateMonster)() = (undefined8(*)())0x141A6C1C0;
    }
    namespace List {
        static void(*IncreaseCapacity)(void*, ulonglong) = (void(*)(void*, ulonglong))0x140249B70;
    }
    namespace GameVersion {
        static undefined(*CalcNum)() = (undefined(*)())0x1418D4D00;
        static void* StringPtr = (void*)0x143F992E8;
    }
    namespace String {
        static undefined8(*Format__)(undefined8, undefined8, undefined8, undefined8, undefined) = (undefined8(*)(undefined8, undefined8, undefined8, undefined8, undefined))0x140463B60;
        static undefined(*Format)() = (undefined(*)())0x1403052C0;
    }
    namespace Chat {
        static undefined(*BuildShowGameMessage_)() = (undefined(*)())0x1419A2C80;
        static void* MainPtr = (void*)0x14500CE70;
        static void(*ShowGameMessage)(undefined*, undefined*, float, uint, undefined1) = (void(*)(undefined*, undefined*, float, uint, undefined1))0x141A540D0;
        namespace Unkn {
            static undefined(*CallBuild)() = (undefined(*)())0x1411B6B20;
        }
        static undefined(*BuildShowGameMessage)() = (undefined(*)())0x1419A2D20;
        static long long* uGuiChatBase = (long long*)0x1451C464C;
    }
    namespace Damage {
        static bool(*ApplySoftenBuildup)(longlong, longlong, float) = (bool(*)(longlong, longlong, float))0x1402C78A0;
    }
    namespace File {
        static undefined(*LoadResource)() = (undefined(*)())0x1422228D0;
    }
    namespace QuestBoard {
        static undefined(*FilterQuestList)() = (undefined(*)())0x14114DB30;
    }
    namespace GamePad {
        static undefined8(*XboxPadPtr)() = (undefined8(*)())0x1451C4558;
    }
}
