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
        static undefined(*CheckProgress)() = (undefined(*)())0x14134e310;
        static undefined(*GetCategory)() = (undefined(*)())0x1419744e0;
        static void* OptionalIdList = (void*)0x14324d980;
        static bool(*CheckComplete)(longlong, uint) = (bool(*)(longlong, uint))0x14134e280;
        static undefined8(*OptionalCount)() = (undefined8(*)())0x141354f80;
        static uint(*OptionalAt)(undefined*, int) = (uint(*)(undefined*, int))0x141354f70;
        static undefined(*IsMasterRank)() = (undefined(*)())0x141975440;
        static void* GlobalOptionalQuestList = (void*)0x145010ae8;
        static undefined(*StarCategoryCheck)() = (undefined(*)())0x140f2b640;
        namespace QuestData {
            static void* ResourceVtable = (void*)0x1433e8750;
            static undefined(*ResourceFunc)() = (undefined(*)())0x141a0f280;
            static void* ResourcePtr = (void*)0x145009308;
        }
        namespace QuestNoList {
            static void* ResourceVtable = (void*)0x142f83b20;
            static undefined(*ResourceFunc)() = (undefined(*)())0x140478520;
            static void* ResourcePtr = (void*)0x144cb5e40;
        }
    }
    namespace Player {
        static void* BasePtr = (void*)0x145011710;
        static undefined(*PlayerBasePlot)() = (undefined(*)())0x145011760;
        static void(*Effects)(undefined*, int, int) = (void(*)(undefined*, int, int))0x140AE0090;
        static undefined(*Visual)() = (undefined(*)())0x141F9DAE6;
        static void* PlayerDataHandlePlot = (void*)0x1451CA0E0;
        static void(*CallLmt)(undefined*, int, int) = (void(*)(undefined*, int, int))0x141C006E0;
        static void(*ActionFrameSpeed)(undefined8*) = (void(*)(undefined8*))0x142227BB0;
        static undefined* (*GetPlayer)(undefined*) = (undefined * (*)(undefined*))0x141b8cde0;
        static undefined(*HitPtr)(undefined*, undefined*) = (undefined(*)(undefined*, undefined*))0x141F5048B;
    }
    namespace EnvironmentalBiological {
        static undefined* (*ctor)(undefined*, undefined4, undefined4) = (undefined * (*)(undefined*, undefined4, undefined4))0x141d277f0;
        static void(*dtor)(undefined8*) = (void(*)(undefined8*))0x141D27C00;
    }
    namespace World {
        static undefined8(*MapClockLocal)(float*, float) = (undefined8(*)(float*, float))0x140AE6490;
        static undefined(*Message)() = (undefined(*)())0x144F85DB0;
        static undefined8(*ActionFrameSpeed)(undefined8*) = (undefined8(*)(undefined8*))0x142227B70;
        static undefined(*Screen)(undefined*) = (undefined(*)(undefined*))0x14236BC90;
    }
    namespace Shlp {
        static void* (*GetShlp)(void*, int) = (void* (*)(void*, int))0x140F7CF20;
        static void(*CallShlp)(void*, void*, void*, void*) = (void(*)(void*, void*, void*, void*))0x141AA67D0;
        static void*(*ctor)() = (void* (*)())0x141AA67D0;
        static void(*dtor)(void*) = (void(*)(void*))0x142226EC0;
    }
    namespace Monster {
        static void(*dtor)(undefined8*) = (void(*)(undefined8*))0x141CA3A10;
        static void* vptr = (void*)0x14344d718;
        namespace DamageBehavior {
            static undefined(*NextAction)() = (undefined(*)())0x1413966e0;
        }
        static bool(*LaunchAction)(undefined*, uint) = (bool(*)(undefined*, uint))0x141cc4590;
        static undefined(*MotionFromId)() = (undefined(*)())0x141bfeab0;
        static undefined* (*ctor)(undefined*, undefined4, undefined4) = (undefined * (*)(undefined*, undefined4, undefined4))0x141CA1130;
        namespace SoftenTimers {
            static undefined(*WoundPartLocal)() = (undefined(*)())0x140afba90;
        }
        static undefined(*GenerateFilePaths)() = (undefined(*)())0x141cce8f0;
    }
    namespace Weapon {
        namespace UI {
            static undefined(*CalcAwakenedElement)() = (undefined(*)())0x14197cc30;
            static uint(*CalcRawBloat)(uint, uint) = (uint(*)(uint, uint))0x14197cc00;
            static void* RawBloatMultipliers = (void*)0x1433b6740;
            namespace Unkn {
                static undefined(*ConditionalElementalBloat)() = (undefined(*)())0x14191a7b0;
            }
            static undefined(*CalcElementalBloat)() = (undefined(*)())0x14197cc20;
        }
        static void* (*ChangeWeapon)(void*, int, int) = (void* (*)(void*, int, int))0x141F59090;
        static void* (*CompleteChangeWeapon)(void*, int, int) = (void* (*)(void*, int, int))0x142040320;
        static void* (*RefreshEquip)(void*) = (void* (*)(void*))0x141257b30;
        static void(*Hit)() = (void(*)())0x141F70741;
        static undefined(*WeaponOrnaments)() = (undefined(*)())0x142112311;
        static undefined(*MainWeaponPtr)() = (undefined(*)())0x142117D49;
        static undefined(*SecondaryWeaponPtr)() = (undefined(*)())0x142150F13;
    }
    namespace EmSetter {
        static undefined8(*CreateMonster)() = (undefined8(*)())0x141a6b4f0;
    }
    namespace List {
        static void(*IncreaseCapacity)(void*, ulonglong) = (void(*)(void*, ulonglong))0x140249b20;
    }
    namespace GameVersion {
        static undefined(*CalcNum)() = (undefined(*)())0x1418d4030;
        static void* StringPtr = (void*)0x143eeb1f0;
    }
    namespace String {
        static undefined8(*Format__)(undefined8, undefined8, undefined8, undefined8, undefined) = (undefined8(*)(undefined8, undefined8, undefined8, undefined8, undefined))0x1404639a0;
        static undefined(*Format)() = (undefined(*)())0x1403050f0;
    }
    namespace Chat {
        static undefined(*BuildShowGameMessage_)() = (undefined(*)())0x1419a1fb0;
        static void* MainPtr = (void*)0x14500AC30;
        static void(*ShowGameMessage)(undefined*, undefined*, float, uint, undefined1) = (void(*)(undefined*, undefined*, float, uint, undefined1))0x141A53400;
        namespace Unkn {
            static undefined(*CallBuild)() = (undefined(*)())0x1411b5e50;
        }
        static undefined(*BuildShowGameMessage)() = (undefined(*)())0x1419a2050;
    }
    namespace Damage {
        static bool(*ApplySoftenBuildup)(longlong, longlong, float) = (bool(*)(longlong, longlong, float))0x1402c7850;
    }
    namespace File {
        static undefined(*LoadResource)() = (undefined(*)())0x1422214f0;
    }
    namespace QuestBoard {
        static undefined(*FilterQuestList)() = (undefined(*)())0x14114ce60;
    }
    namespace GamePad {
        static undefined8(*XboxPadPtr)() = (undefined8(*)())0x1451C2318;
    }
}
