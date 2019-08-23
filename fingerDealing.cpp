
#include "fingerDealing.h"


#if 0
HRESULT CaptureSample()
{
	HRESULT hr = S_OK;
	WINBIO_SESSION_HANDLE sessionHandle = NULL;
	WINBIO_UNIT_ID unitId = 0;
	WINBIO_REJECT_DETAIL rejectDetail = 0;
	PWINBIO_BIR sample = NULL;
	SIZE_T sampleSize = 0;

	// Connect to the system pool. 
	hr = WinBioOpenSession(
		WINBIO_TYPE_FINGERPRINT,    // Service provider
		WINBIO_POOL_PRIVATE,         // Pool type
		WINBIO_FLAG_DEFAULT,            // Access: Capture raw data
		NULL,                       // Array of biometric unit IDs
		0,                          // Count of biometric unit IDs
		WINBIO_DB_DEFAULT,          // Default database
		&sessionHandle              // [out] Session handle
	);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioOpenSession failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Capture a biometric sample.
	wprintf_s(L"\n Calling WinBioCaptureSample - Swipe sensor...\n");
	hr = WinBioCaptureSample(
		sessionHandle,
		WINBIO_PURPOSE_VERIFY,
		WINBIO_DATA_FLAG_PROCESSED,
		&unitId,
		&sample,
		&sampleSize,
		&rejectDetail
	);
	if (FAILED(hr))
	{
		if (hr == WINBIO_E_BAD_CAPTURE)
		{
			wprintf_s(L"\n Bad capture; reason: %d\n", rejectDetail);
		}
		else
		{
			wprintf_s(L"\n WinBioCaptureSample failed. hr = 0x%x\n", hr);
		}
		goto e_Exit;
	}

	wprintf_s(L"\n Swipe processed - Unit ID: %d\n", unitId);
	wprintf_s(L"\n Captured %d bytes.\n", sampleSize);


e_Exit:
	if (sample != NULL)
	{
		WinBioFree(sample);
		sample = NULL;
	}

	if (sessionHandle != NULL)
	{
		WinBioCloseSession(sessionHandle);
		sessionHandle = NULL;
	}

	wprintf_s(L"\n Press any key to exit...");
	_getch();

	return hr;
}


HRESULT EnrollSysPool(
	BOOL discardEnrollment,
	WINBIO_BIOMETRIC_SUBTYPE subFactor)
{
	HRESULT hr = S_OK;
	WINBIO_IDENTITY identity = { 0 };
	WINBIO_SESSION_HANDLE sessionHandle = NULL;
	WINBIO_UNIT_ID unitId = 0;
	WINBIO_REJECT_DETAIL rejectDetail = 0;
	BOOLEAN isNewTemplate = TRUE;

	// Connect to the system pool. 
	hr = WinBioOpenSession(
		WINBIO_TYPE_FINGERPRINT,    // Service provider
		WINBIO_POOL_SYSTEM,         // Pool type
		WINBIO_FLAG_DEFAULT,        // Configuration and access
		NULL,                       // Array of biometric unit IDs
		0,                          // Count of biometric unit IDs
		NULL,                       // Database ID
		&sessionHandle              // [out] Session handle
	);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioOpenSession failed. ");
		wprintf_s(L"hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Locate a sensor.
	wprintf_s(L"\n Swipe your finger on the sensor...\n");
	hr = WinBioLocateSensor(sessionHandle, &unitId);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioLocateSensor failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Begin the enrollment sequence. 
	wprintf_s(L"\n Starting enrollment sequence...\n");
	hr = WinBioEnrollBegin(
		sessionHandle,      // Handle to open biometric session
		subFactor,          // Finger to create template for
		unitId              // Biometric unit ID
	);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioEnrollBegin failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Capture enrollment information by swiping the sensor with
	// the finger identified by the subFactor argument in the 
	// WinBioEnrollBegin function.
	for (int swipeCount = 1;; ++swipeCount)
	{
		wprintf_s(L"\n Swipe the sensor to capture %s sample.",
			(swipeCount == 1) ? L"the first" : L"another");

		hr = WinBioEnrollCapture(
			sessionHandle,  // Handle to open biometric session
			&rejectDetail   // [out] Failure information
		);

		wprintf_s(L"\n Sample %d captured from unit number %d.",
			swipeCount,
			unitId);

		if (hr == WINBIO_I_MORE_DATA)
		{
			wprintf_s(L"\n    More data required.\n");
			continue;
		}
		if (FAILED(hr))
		{
			if (hr == WINBIO_E_BAD_CAPTURE)
			{
				wprintf_s(L"\n  Error: Bad capture; reason: %d",
					rejectDetail);
				continue;
			}
			else
			{
				wprintf_s(L"\n WinBioEnrollCapture failed. hr = 0x%x", hr);
				goto e_Exit;
			}
		}
		else
		{
			wprintf_s(L"\n    Template completed.\n");
			break;
		}
	}

	// Discard the enrollment if the appropriate flag is set.
	// Commit the enrollment if it is not discarded.
	if (discardEnrollment == TRUE)
	{
		wprintf_s(L"\n Discarding enrollment...\n\n");
		hr = WinBioEnrollDiscard(sessionHandle);
		if (FAILED(hr))
		{
			wprintf_s(L"\n WinBioLocateSensor failed. hr = 0x%x\n", hr);
		}
		goto e_Exit;
	}
	else
	{
		wprintf_s(L"\n Committing enrollment...\n");
		hr = WinBioEnrollCommit(
			sessionHandle,      // Handle to open biometric session
			&identity,          // WINBIO_IDENTITY object for the user
			&isNewTemplate);    // Is this a new template

		if (FAILED(hr))
		{
			wprintf_s(L"\n WinBioEnrollCommit failed. hr = 0x%x\n", hr);
			goto e_Exit;
		}
	}


e_Exit:
	if (sessionHandle != NULL)
	{
		WinBioCloseSession(sessionHandle);
		sessionHandle = NULL;
	}

	wprintf_s(L" Press any key to continue...");
	_getch();

	return hr;
}

#endif

HRESULT EnumEnrollments()
{
	// Declare variables.
	HRESULT hr = S_OK;
	WINBIO_IDENTITY identity = { 0 };
	WINBIO_SESSION_HANDLE sessionHandle = NULL;
	WINBIO_UNIT_ID unitId = 0;
	PWINBIO_BIOMETRIC_SUBTYPE subFactorArray = NULL;
	WINBIO_BIOMETRIC_SUBTYPE SubFactor = 0;
	SIZE_T subFactorCount = 0;
	WINBIO_REJECT_DETAIL rejectDetail = 0;
	WINBIO_BIOMETRIC_SUBTYPE subFactor = WINBIO_SUBTYPE_NO_INFORMATION;

	// Connect to the system pool. 
	hr = WinBioOpenSession(
		WINBIO_TYPE_FINGERPRINT,    // Service provider
		WINBIO_POOL_SYSTEM,         // Pool type
		WINBIO_FLAG_DEFAULT,        // Configuration and access
		NULL,                       // Array of biometric unit IDs
		0,                          // Count of biometric unit IDs
		NULL,                       // Database ID
		&sessionHandle              // [out] Session handle
	);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioOpenSession failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Locate the biometric sensor and retrieve a WINBIO_IDENTITY object.
	wprintf_s(L"\n Calling WinBioIdentify - Swipe finger on sensor...\n");
	hr = WinBioIdentify(
		sessionHandle,              // Session handle
		&unitId,                    // Biometric unit ID
		&identity,                  // User SID
		&subFactor,                 // Finger sub factor
		&rejectDetail               // Rejection information
	);
	wprintf_s(L"\n Swipe processed - Unit ID: %d\n", unitId);
	if (FAILED(hr))
	{
		if (hr == WINBIO_E_UNKNOWN_ID)
		{
			wprintf_s(L"\n Unknown identity.\n");
		}
		else if (hr == WINBIO_E_BAD_CAPTURE)
		{
			wprintf_s(L"\n Bad capture; reason: %d\n", rejectDetail);
		}
		else
		{
			wprintf_s(L"\n WinBioEnumBiometricUnits failed. hr = 0x%x\n", hr);
		}
		goto e_Exit;
	}

	// Retrieve the biometric sub-factors for the template.
	hr = WinBioEnumEnrollments(
		sessionHandle,              // Session handle
		unitId,                     // Biometric unit ID
		&identity,                  // Template ID
		&subFactorArray,            // Subfactors
		&subFactorCount             // Count of subfactors
	);
	if (FAILED(hr))
	{
		wprintf_s(L"\n WinBioEnumEnrollments failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Print the sub-factor(s) to the console.
	wprintf_s(L"\n Enrollments for this user on Unit ID %d:", unitId);
	for (SIZE_T index = 0; index < subFactorCount; ++index)
	{
		SubFactor = subFactorArray[index];
		switch (SubFactor)
		{
		case WINBIO_ANSI_381_POS_RH_THUMB:
			wprintf_s(L"\n   RH thumb\n");
			break;
		case WINBIO_ANSI_381_POS_RH_INDEX_FINGER:
			wprintf_s(L"\n   RH index finger\n");
			break;
		case WINBIO_ANSI_381_POS_RH_MIDDLE_FINGER:
			wprintf_s(L"\n   RH middle finger\n");
			break;
		case WINBIO_ANSI_381_POS_RH_RING_FINGER:
			wprintf_s(L"\n   RH ring finger\n");
			break;
		case WINBIO_ANSI_381_POS_RH_LITTLE_FINGER:
			wprintf_s(L"\n   RH little finger\n");
			break;
		case WINBIO_ANSI_381_POS_LH_THUMB:
			wprintf_s(L"\n   LH thumb\n");
			break;
		case WINBIO_ANSI_381_POS_LH_INDEX_FINGER:
			wprintf_s(L"\n   LH index finger\n");
			break;
		case WINBIO_ANSI_381_POS_LH_MIDDLE_FINGER:
			wprintf_s(L"\n   LH middle finger\n");
			break;
		case WINBIO_ANSI_381_POS_LH_RING_FINGER:
			wprintf_s(L"\n   LH ring finger\n");
			break;
		case WINBIO_ANSI_381_POS_LH_LITTLE_FINGER:
			wprintf_s(L"\n   LH little finger\n");
			break;
		default:
			wprintf_s(L"\n   The sub-factor is not correct\n");
			break;
		}

	}

e_Exit:
	if (subFactorArray != NULL)
	{
		WinBioFree(subFactorArray);
		subFactorArray = NULL;
	}

	if (sessionHandle != NULL)
	{
		WinBioCloseSession(sessionHandle);
		sessionHandle = NULL;
	}

	wprintf_s(L"\n Press any key to exit...");
	_getch();

	return hr;
}

# if 0
VOID DisplayGuid(__in PWINBIO_UUID Guid);

HRESULT EnumDatabases()
{
	// Declare variables.
	HRESULT hr = S_OK;
	PWINBIO_STORAGE_SCHEMA storageSchemaArray = NULL;
	SIZE_T storageCount = 0;
	SIZE_T index = 0;

	// Enumerate the databases.
	hr = WinBioEnumDatabases(
		WINBIO_TYPE_FINGERPRINT,    // Type of biometric unit
		&storageSchemaArray,        // Array of database schemas
		&storageCount);            // Number of database schemas
	if (FAILED(hr))
	{
		wprintf_s(L"\nWinBioEnumDatabases failed. hr = 0x%x\n", hr);
		goto e_Exit;
	}

	// Display information for each database.
	wprintf_s(L"\nDatabases:\n");
	for (index = 0; index < storageCount; ++index)
	{
		wprintf_s(L"\n[%d]: \tBiometric factor: 0x%08x\n",
			index,
			storageSchemaArray[index].BiometricFactor);

		wprintf_s(L"\tDatabase ID: ");
		DisplayGuid(&storageSchemaArray[index].DatabaseId);
		wprintf_s(L"\n");

		wprintf_s(L"\tData format: ");
		DisplayGuid(&storageSchemaArray[index].DataFormat);
		wprintf_s(L"\n");

		wprintf_s(L"\tAttributes:  0x%08x\n",
			storageSchemaArray[index].Attributes);

		wprintf_s(L"\tFile path:   %ws\n",
			storageSchemaArray[index].FilePath);

		wprintf_s(L"\tCnx string:  %ws\n",
			storageSchemaArray[index].ConnectionString);

		wprintf_s(L"\n");
	}

e_Exit:
	if (storageSchemaArray != NULL)
	{
		WinBioFree(storageSchemaArray);
		storageSchemaArray = NULL;
	}

	wprintf_s(L"\nPress any key to exit...");
	_getch();

	return hr;
}


//------------------------------------------------------------------------
// The following function displays a GUID to the console window.
//
VOID DisplayGuid(__in PWINBIO_UUID Guid)
{
	wprintf_s(
		L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		Guid->Data1,
		Guid->Data2,
		Guid->Data3,
		Guid->Data4[0],
		Guid->Data4[1],
		Guid->Data4[2],
		Guid->Data4[3],
		Guid->Data4[4],
		Guid->Data4[5],
		Guid->Data4[6],
		Guid->Data4[7]
	);
}

#endif
