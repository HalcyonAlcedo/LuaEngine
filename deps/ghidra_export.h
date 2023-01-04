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
        static undefined8(*ErrorCheck)() = (undefined8(*)())0x14114e510;
        static undefined8(*CheckProgress)() = (undefined8(*)())0x14135e260;
        static undefined8(*GetCategory)(undefined8) = (undefined8(*)(undefined8))0x1419875c0;
        static void* OptionalIdList = (void*)0x1432a7cb0;
        static bool(*CheckComplete)(longlong, uint) = (bool(*)(longlong, uint))0x14135e1e0;
        static undefined8(*OptionalCount)() = (undefined8(*)())0x141365290;
        static uint(*OptionalAt)(undefined*, int) = (uint(*)(undefined*, int))0x141365280;
        static bool(*IsMasterRank)(undefined8, undefined8, undefined8, undefined8) = (bool(*)(undefined8, undefined8, undefined8, undefined8))0x1419885c0;
        static void* GlobalOptionalQuestList = (void*)0x145073258;
        static bool(*StarCategoryCheck)(int, int, int) = (bool(*)(int, int, int))0x140f347b0;
        namespace QuestData {
            static void* ResourceVtable = (void*)0x143442ac0;
            static undefined8(*ResourceFunc)() = (undefined8(*)())0x141a22de0;
            static void* ResourcePtr = (void*)0x14506ba18;
        }
        namespace QuestNoList {
            static void* ResourceVtable = (void*)0x142fdde48;
            static undefined8(*ResourceFunc)() = (undefined8(*)())0x140479500;
            static void* ResourcePtr = (void*)0x144d17e20;
        }
        static undefined8(*QuestBase)() = (undefined8(*)())0x14506F240;
        static undefined* (*NextQuest)() = (undefined * (*)())0x14114C656;
    }
    namespace Player {
        static void* BasePtr = (void*)0x145073e80;
        static undefined* (*GetPlayer)(undefined*) = (undefined * (*)(undefined*))0x141ba9280;
        static undefined(*PlayerBasePlot)() = (undefined(*)())0x145073ED0;
        static undefined(*ActionTrigger)(undefined*, uint) = (undefined(*)(undefined*, uint))0x14226E938;
        static void(*Effects)(undefined*, int, int) = (void(*)(undefined*, int, int))0x140ae3aa0;
        static undefined(*Visual)() = (undefined(*)())0x141FBB736;
        static void* PlayerDataPlot = (void*)0x1452253A8;
        static void* PlayerDataHandlePlot = (void*)0x14522C850;
        static void(*CallLmt)(undefined*, int, int) = (void(*)(undefined*, int, int))0x141C1CBD0;
        static void(*ActionFrameSpeed)(undefined8*) = (void(*)(undefined8*))0x1422455D0;
        static void(*HookCoordinateChange)() = (void(*)())0x141F80AB2;
        static void(*HookCoordinateChange2)(undefined8*, undefined8*) = (void(*)(undefined8*, undefined8*))0x140EFA699;
        static void* PlayerArchive = (void*)0x145073E80;
        static undefined(*HitPtr)(undefined*, undefined*) = (undefined(*)(undefined*, undefined*))0x141F6E0DB;
    }
    namespace Otomo {
        static undefined(*OtomoBasePlot)() = (undefined(*)())0x144FFF7A8;
    }
    namespace EnvironmentalBiological {
        static undefined* (*ctor)(undefined*, undefined4, undefined4) = (undefined * (*)(undefined*, undefined4, undefined4))0x141D442C0;
        static void(*dtor)(undefined8*) = (void(*)(undefined8*))0x141D446D0;
    }
    namespace World {
        static undefined8(*WaypointZLocal)(float*, float*) = (undefined8(*)(float*, float*))0x141DBF55F;
        static undefined8(*MapClockLocal)(float*, float) = (undefined8(*)(float*, float))0x140AE9EA0;
        static undefined(*GameClock)() = (undefined(*)())0x145073D60;
        static undefined(*GameClockControl)(float) = (undefined(*)(float))0x141B9EB37;
        static undefined(*Message)() = (undefined(*)())0x144D18278;
        static undefined8(*ActionFrameSpeed)(undefined8*) = (undefined8(*)(undefined8*))0x142245590;
        static void* Assembly = (void*)0x145224BF8;
    }
    namespace Shlp {
        static void* (*GetShlp)(void*, int) = (void* (*)(void*, int))0x140F86090;
        static void(*CallShlp)(void*, void*, void*, void*) = (void(*)(void*, void*, void*, void*))0x141ABA910;
        static void(*ctor)(void*) = (void(*)(void*))0x14210e080;
        static void(*dtor)(void*) = (void(*)(void*))0x1422448E0;
    }
    namespace Savefile {
        static bool(*CheckFlag)(longlong, uint) = (bool(*)(longlong, uint))0x14136bdf0;
    }
    namespace Monster {
        static void(*dtor)(undefined8*) = (void(*)(undefined8*))0x141cbff80;
        static void* vptr = (void*)0x1434a7a20;
        namespace DamageBehavior {
            static undefined8(*NextAction)() = (undefined8(*)())0x1413a6bf0;
        }
        static bool(*LaunchAction)(undefined*, uint) = (bool(*)(undefined*, uint))0x141ce0a50;
        static undefined* (*ctor)(undefined*, undefined4, undefined4) = (undefined * (*)(undefined*, undefined4, undefined4))0x141cbd6a0;
        namespace SoftenTimers {
            static undefined8(*WoundPartLocal)() = (undefined8(*)())0x140aff490;
        }
        static void(*BehaviorControl)(undefined*, int) = (void(*)(undefined*, int))0x141CE0A50;
    }
    namespace Weapon {
        namespace UI {
            static undefined8(*CalcAwakenedElement)() = (undefined8(*)())0x1419900b0;
            static uint(*CalcRawBloat)(uint, uint) = (uint(*)(uint, uint))0x141990080;
            static void* RawBloatMultipliers = (void*)0x143410b78;
            namespace Unkn {
                static undefined8(*ConditionalElementalBloat)() = (undefined8(*)())0x14192cc00;
            }
            static undefined8(*CalcElementalBloat)() = (undefined8(*)())0x1419900a0;
        }
        static void* (*ChangeWeapon)(void*, int, int) = (void* (*)(void*, int, int))0x141F76CE0;
        static void* (*CompleteChangeWeapon)(void*, int, int) = (void* (*)(void*, int, int))0x14205E050;
        static void(*Hit)() = (void(*)())0x141F8E391;
        static undefined(*WeaponOrnaments)() = (undefined(*)())0x1421300f1;
        static undefined(*MainWeaponPtr)() = (undefined(*)())0x142135B29;
        static undefined(*SecondaryWeaponPtr)() = (undefined(*)())0x14216ECFD;
    }
    namespace EmSetter {
        static undefined8(*CreateMonster)() = (undefined8(*)())0x141a7f4e0;
    }
    namespace List {
        static void(*IncreaseCapacity)(void*, ulonglong) = (void(*)(void*, ulonglong))0x140249d10;
    }
    namespace GameVersion {
        static undefined8(*CalcNum)() = (undefined8(*)())0x1418e61d0;
        static void* StringPtr = (void*)0x143f4d1d0;
    }
    namespace String {
        static undefined8(*Format__)(undefined8, undefined8, undefined8, undefined8, undefined) = (undefined8(*)(undefined8, undefined8, undefined8, undefined8, undefined))0x1404649a0;
        static undefined8(*Format)() = (undefined8(*)())0x140306110;
    }
    namespace Chat {
        static undefined8(*BuildShowGameMessage_)() = (undefined8(*)())0x1419b5430;
        static void* MainPtr = (void*)0x14506d340;
        static void(*ShowGameMessage)(undefined*, undefined*, float, uint, undefined1) = (void(*)(undefined*, undefined*, float, uint, undefined1))0x141a671f0;
        namespace Unkn {
            static undefined8(*CallBuild)() = (undefined8(*)())0x1411c0c30;
        }
        static undefined8(*BuildShowGameMessage)() = (undefined8(*)())0x1419b54d0;
    }
    namespace Damage {
        static bool(*ApplySoftenBuildup)(longlong, longlong, float) = (bool(*)(longlong, longlong, float))0x1402c84d0;
    }
    namespace File {
        static undefined8(*LoadResource)() = (undefined8(*)())0x14223ef10;
    }
    namespace QuestBoard {
        static undefined8(*FilterQuestList)() = (undefined8(*)())0x141157940;
    }
    namespace GamePad {
        static undefined8(*XboxPadPtr)() = (undefined8(*)())0x145224A98;
    }


}
