// MisePointer.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>

// #define WIN7

#ifdef WIN7

#include <mmdeviceapi.h>
#include <endpointvolume.h>

bool ChangeVolume(double nVolume,bool bScalar)
{
 
    HRESULT hr=NULL;
    bool decibels = false;
    bool scalar = false;
    double newVolume=nVolume;
 
    CoInitialize(NULL);
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
                          __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    IMMDevice *defaultDevice = NULL;
 
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;
 
    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), 
         CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    defaultDevice->Release();
    defaultDevice = NULL;
 
    // -------------------------
    float currentVolume = 0;
    endpointVolume->GetMasterVolumeLevel(&currentVolume);
    //printf("Current volume in dB is: %f\n", currentVolume);

    hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
    //CString strCur=L"";
    //strCur.Format(L"%f",currentVolume);
    //AfxMessageBox(strCur);

    // printf("Current volume as a scalar is: %f\n", currentVolume);
    if (bScalar==false)
    {
        hr = endpointVolume->SetMasterVolumeLevel((float)newVolume, NULL);
    }
    else if (bScalar==true)
    {
        hr = endpointVolume->SetMasterVolumeLevelScalar((float)newVolume, NULL);
    }
    endpointVolume->Release();
 
    CoUninitialize();
 
    return FALSE;
}
#endif
int _tmain(int argc, _TCHAR* argv[])
{
	
	int command = 0;
	int x = 100, y = 100;
	int step = 10;
	double voljum = 0;
	
	POINT cxy;

	DCB dcb;
	HANDLE hCom;
	BOOL fSuccess;

	hCom = CreateFile( _T("COM1"),
				GENERIC_READ | GENERIC_WRITE,
				0,    // must be opened with exclusive-access
				NULL, // no security attributes
				OPEN_EXISTING, // must use OPEN_EXISTING
				0,    // not overlapped I/O
				NULL  // hTemplate must be NULL for comm devices
				);

	if (hCom == INVALID_HANDLE_VALUE) 
	{
		// Handle the error.
		printf ("Connecting to COM port failed, error %d.\n", GetLastError());
		return 1;
	}else{
		printf("Connecting to COM port success \n");
	}

	// Build on the current configuration, and skip setting the size
	// of the input and output buffers with SetupComm.

	fSuccess = GetCommState(hCom, &dcb);

	if (!fSuccess) 
	{
		// Handle the error.
		printf ("GetCommState failed with error %d.\n", GetLastError());
		return 1;
	}else{
		printf("CommState OK \n");
	}

	// Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.

	dcb.BaudRate = CBR_57600;     // set the baud rate
	dcb.ByteSize = 8;             // data size, xmit, and rcv
	dcb.Parity = ODDPARITY;        // no parity bit
	dcb.StopBits = ONESTOPBIT;    // one stop bit

	fSuccess = SetCommState(hCom, &dcb);

	if (!fSuccess) 
	{
		printf ("SetCommState failed with error %d.\n", GetLastError());
		return 1;
	}

	printf("Now listening remote!\n");

	INPUT pp;
	pp.type = INPUT_MOUSE;
	pp.mi.mouseData = 0;
	pp.mi.time = 0;

	/**/
	DWORD bytesRead;
	unsigned char buff[1] = {0};
	/**/
	while(1){
		ReadFile(hCom, buff, 1, &bytesRead, NULL);

		int temp = 0;

		temp |= (1 & (buff[0] >> 0)) << 0;
		temp |= (1 & (buff[0] >> 1)) << 1;
		temp |= (1 & (buff[0] >> 2)) << 2;
		temp |= (1 & (buff[0] >> 3)) << 3;
		temp |= (1 & (buff[0] >> 4)) << 4;
		temp |= (1 & (buff[0] >> 5)) << 5;
		temp |= (1 & (buff[0] >> 6)) << 6;
		temp |= (1 & (buff[0] >> 7)) << 7;

		buff[0] = 0;
		buff[1] = 0;

		command = temp;

		switch(command){
		case 64: // UP
			GetCursorPos(&cxy);
			y=cxy.y-step; printf("Cortinates X=%i - Y=%i\n", x, y); SetCursorPos(cxy.x , y);
			break;
		case 66: // DOWN
			GetCursorPos(&cxy);
			y=cxy.y+step; printf("Cortinates X=%i - Y=%i\n", x, y); SetCursorPos(cxy.x , y);
			break;
		case 21: // LEFT
			GetCursorPos(&cxy);
			x=cxy.x-step; printf("Cortinates X=%i - Y=%i\n", x, y); SetCursorPos(x , cxy.y);
			break;
		case 22: // RIGHT
			GetCursorPos(&cxy);
			x=cxy.x+step; printf("Cortinates X=%i - Y=%i\n", x, y); SetCursorPos(x , cxy.y);
			break;
		case 16:
			pp.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			SendInput(1, &pp, sizeof(pp));

			pp.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			SendInput(1, &pp, sizeof(pp));  printf("Right button clicked\n");
				break;
		case 17:
			pp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1, &pp, sizeof(pp));

			pp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			SendInput(1, &pp, sizeof(pp));  printf("Left button clicked\n");
			break;
		case 13:
			CloseHandle(hCom);
			printf("Shutting down!\n");
			exit(0);
			break;
#ifdef WIN7
		case 3:
				if(!(voljum >= 0.9)){
					voljum = (voljum+0.1);
					ChangeVolume(voljum, true);  printf("Volume %f\n", voljum);
				}
			break;
		case 6:
			if(!(voljum <= 0.1)){
					voljum = (voljum-0.1);
					ChangeVolume(voljum, true); printf("Volume %f\n", voljum);
			}
			break;
#endif
		case 7:
			step = 1; printf("Step size %i\n", step);
			break;
		case 8:
			step = 10; printf("Step size %i\n", step);
			break;
		case 9:
			step = 100; printf("Step size %i\n", step);
			break;
		case 110:
			pp.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			pp.mi.dx = x;
			pp.mi.dx = y;
			SendInput(1, &pp, sizeof(pp)); printf("Left button down & holding\n");
			break;
		case 108:
			pp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
			pp.mi.dx = x;
			pp.mi.dx = y;
			SendInput(1, &pp, sizeof(pp));  printf("Left button up & released\n");
			break;
		case 2:
			//GetProcessList();
			break;
		case 104:
			break;
		default:
			// printf("Comm %i \n", command); Sleep(350);
			break;
		}
	}
	CloseHandle(hCom);
	printf("Shutting down!\n");
	return 0;
}

