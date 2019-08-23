//#pragma once
#include <Windows.h>
#include <stdio.h>
#include <Conio.h>
#include <WinBio.h>

HRESULT CaptureSample();

HRESULT EnrollSysPool(
	BOOL discardEnrollment,
	WINBIO_BIOMETRIC_SUBTYPE subFactor);

HRESULT EnumEnrollments();

HRESULT EnumDatabases();