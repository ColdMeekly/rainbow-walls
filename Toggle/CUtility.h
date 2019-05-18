#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

class CUtility
{
public:
	PROCESSENTRY32	__gameProcess;
	HANDLE			__handleProcess;
	HWND			__HWNDcsgo;
	MODULEENTRY32	modClient;
	MODULEENTRY32	modEngine;
	DWORD			__dwOverlay;
	DWORD			__dwVGui;
	DWORD			__dwLibCef;
	DWORD			__dwSteam;

	static DWORD	findProcessByName(const char *, PROCESSENTRY32 *);
	static DWORD	getThreadByProcess(DWORD);
	static bool		getModuleNamePointer(const char*, DWORD, MODULEENTRY32*);

	VOID			runSetDebugPrivs();
	VOID			runProcess();

	// Schooling - template functions MUST be defined in header files or linker errors ensue.
	template<class T>
	T	Read(DWORD address) const {
		T temp;
		ReadProcessMemory(this->__handleProcess, (LPVOID)address, &temp, sizeof(T), NULL);
		return temp;
	}

	template<class T>
	void	Write(DWORD address, T value) const {
		WriteProcessMemory(this->__handleProcess, (LPVOID)address, &value, sizeof(T), NULL);
	}

	DWORD* FindPattern(MODULEENTRY32, byte*, const char*, int, size_t) const;
};
extern CUtility Util;
namespace Utility
{
	struct Vector3 {
		float x, y, z;

		Vector3()
		{
			x = y = z = 0.0f;
		}

		Vector3(float X, float Y, float Z)
		{
			x = X; y = Y; z = Z;
		}

		Vector3(float XYZ)
		{
			x = XYZ; y = XYZ; z = XYZ;
		}

		Vector3(float* v)
		{
			x = v[0]; y = v[1]; z = v[2];
		}

		Vector3(const float* v)
		{
			x = v[0]; y = v[1]; z = v[2];
		}

		inline Vector3& operator=(const Vector3& v)
		{
			x = v.x; y = v.y; z = v.z; return *this;
		}

		inline Vector3& operator=(const float* v)
		{
			x = v[0]; y = v[1]; z = v[2]; return *this;
		}

		inline float& operator[](int i)
		{
			return ((float*)this)[i];
		}

		inline float operator[](int i) const
		{
			return ((float*)this)[i];
		}

		inline Vector3& operator+=(const Vector3& v)
		{
			x += v.x; y += v.y; z += v.z; return *this;
		}

		inline Vector3& operator-=(const Vector3& v)
		{
			x -= v.x; y -= v.y; z -= v.z; return *this;
		}

		inline Vector3& operator*=(const Vector3& v)
		{
			x *= v.x; y *= v.y; z *= v.z; return *this;
		}

		inline Vector3& operator/=(const Vector3& v)
		{
			x /= v.x; y /= v.y; z /= v.z; return *this;
		}

		inline Vector3& operator+=(float v)
		{
			x += v; y += v; z += v; return *this;
		}

		inline Vector3& operator-=(float v)
		{
			x -= v; y -= v; z -= v; return *this;
		}

		inline Vector3& operator*=(float v)
		{
			x *= v; y *= v; z *= v; return *this;
		}

		inline Vector3& operator/=(float v)
		{
			x /= v; y /= v; z /= v; return *this;
		}

		inline Vector3 operator-() const
		{
			return Vector3(-x, -y, -z);
		}

		inline Vector3 operator+(const Vector3& v) const
		{
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		inline Vector3 operator-(const Vector3& v) const
		{
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		inline Vector3 operator*(const Vector3& v) const
		{
			return Vector3(x * v.x, y * v.y, z * v.z);
		}

		inline Vector3 operator/(const Vector3& v) const
		{
			return Vector3(x / v.x, y / v.y, z / v.z);
		}

		inline Vector3 operator+(float v) const
		{
			return Vector3(x + v, y + v, z + v);
		}

		inline Vector3 operator-(float v) const
		{
			return Vector3(x - v, y - v, z - v);
		}

		inline Vector3 operator*(float v) const
		{
			return Vector3(x * v, y * v, z * v);
		}

		inline Vector3 operator/(float v) const
		{
			return Vector3(x / v, y / v, z / v);
		}

		inline float Length() const
		{
			return sqrtf(x * x + y * y + z * z);
		}

		inline float LengthSqr() const
		{
			return (x * x + y * y + z * z);
		}

		inline float LengthXY() const
		{
			return sqrtf(x * x + y * y);
		}

		inline float LengthXZ() const
		{
			return sqrtf(x * x + z * z);
		}

		inline float DistTo(const Vector3& v) const
		{
			return (*this - v).Length();
		}

		inline float Dot(const Vector3& v) const
		{
			return (x * v.x + y * v.y + z * v.z);
		}

		inline Vector3 Cross(const Vector3& v) const
		{
			return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}

		inline bool IsZero() const
		{
			return (x > -0.01f && x < 0.01f
				&&	y > -0.01f && y < 0.01f
				&&	z > -0.01f && z < 0.01f);
		}
	};

	struct GlowObjectManager
	{
		uint32_t *pObjects;
		uint32_t  ObjectCount;
	};

	struct glowDefStruct
	{
		uintptr_t*			m_hEntity;
		Utility::Vector3	m_vGlowColor;
		float				m_flGlowAlpha;

		char				unknown[4]; //pad 
		float				flUnk; //confirmed to be treated as a float while reversing glow functions 
		float				m_flBloomAmount;
		float				localplayeriszeropoint3;


		bool				m_bRenderWhenOccluded;
		bool				m_bRenderWhenUnoccluded;
		bool				m_bFullBloomRender;
		char				unknown1[1]; //pad 


		int					m_nFullBloomStencilTestValue; // 0x28 
		int					iUnk; //appears like it needs to be zero  
		int					m_nSplitScreenSlot; //Should be -1 

		// Linked list of free slots 
		int					m_nNextFreeSlot;
	};

	struct playerClass {
		char _0xF0[112];
		unsigned long m_clrRender;
		char _0xFC[124];
		int m_iTeamNum;
		char _0x100[8];
		int m_iHealth;
		int m_fFlags;
		Utility::Vector3 m_vecViewOffset;
		Utility::Vector3 m_vecVelocity;
		char _0x134[12];
		Utility::Vector3 m_vec3ViewAngle;
		Utility::Vector3 m_vecOrigin;
		char _0x258[8];
		int m_hOwnerEntity;
		char _0x25B[268];
		unsigned char m_MoveType;
		char _0x318[2];
		unsigned char m_lifeState;
		char _0x440[188];
		unsigned long m_Collision;
		char _0x470[292];
		unsigned long m_rgflCoordinateFrame;
		char _0x939[44];
		unsigned long m_CollisionGroup;
		char _0x97C[1221];
		unsigned char m_bSpotted;
		char _0x1A84[66];
		unsigned char m_bSpottedByMask;
		char _0x1B88[4359];
		int m_iCompetitiveRanking;
		char _0x267C[256];
		int m_iCompetitiveWins;
		char _0x2698[2800];
		unsigned long m_nForceBone;
		char _0x2980[24];
		unsigned long m_dwBoneMatrix;
		char _0x2984[740];
		float m_flC4Blow;
		float m_flTimerLength;
		char _0x2998[12];
		float m_flDefuseLength;
		float m_flDefuseCountDown;
		char _0x2DE8[32];
		int m_hOwner;
		char _0x2EE8[1064];
		int m_hMyWeapons;
		char _0x2F9A[252];
		int m_hActiveWeapon;
		char _0x2F9C[174];
		unsigned char m_iItemDefinitionIndex;
		char _0x2FAC[1];
		int m_iEntityQuality;
		char _0x2FB0[12];
		unsigned long m_Local;
		unsigned char m_iItemIDHigh;
		char _0x301C[7];
		int m_iAccountID;
		char _0x3028[96];
		Utility::Vector3 m_aimPunchAngle;
		int m_aimPunchAngleVel;
		unsigned long m_szCustomName;
		char _0x31A4[368];
		unsigned long m_OriginalOwnerXuidLow;
		unsigned long m_OriginalOwnerXuidHigh;
		unsigned long m_nFallbackPaintKit;
		unsigned long m_nFallbackSeed;
		float m_flFallbackWear;
		unsigned long m_nFallbackStatTrak;
		char _0x31D8[16];
		unsigned long m_thirdPersonViewAngles;
		char _0x3208[12];
		int m_iFOVStart;
		char _0x3228[44];
		float m_flNextPrimaryAttack;
		char _0x3234[28];
		int m_iState;
		char _0x3275[8];
		int m_iClip1;
		char _0x32E0[61];
		unsigned char m_bInReload;
		char _0x334C[106];
		float m_fAccuracyPenalty;
		char _0x3360[104];
		int m_iObserverMode;
		char _0x3404[16];
		int m_hObserverTarget;
		char _0x3588[160];
		unsigned long m_nTickBase;
		char _0x388E[384];
		unsigned long m_szLastPlaceName;
		char _0x3898[770];
		unsigned char m_bIsScoped;
		char _0x38A4[9];
		unsigned char m_bIsDefusing;
		char _0xA2C0[11];
		unsigned char m_bGunGameImmunity;
		char _0xA304[27163];
		int m_iShotsFired;
		char _0xA308[64];
		float m_flFlashMaxAlpha;
		float m_flFlashDuration;
		char _0xB240[20];
		int m_iGlowIndex;
		char _0xB24C[3868];
		unsigned char m_bHasHelmet;
		char _0xB25C[11];
		unsigned long m_ArmorValue;
		char _0xB2B8[12];
		unsigned char m_bHasDefuser;
	};

}
