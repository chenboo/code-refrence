#pragma once 

#include "math.h"

enum ELC_AUDIO_MIXER_TYPE
{
	ELCAMT_PLAYBACK = 1,
	ELCAMT_VOICE,

	ELCAMT_MIC,
	ELCAMT_MIXER,
	ELCAMT_LINEIN_1,
	ELCAMT_LINEIN_2,
};

enum ELC_XI_AUDIO_DEVICE_TYPE {
	ELC_XADT_NONE			= 0x00000000,
	ELC_XADT_VGA_CARD		= 0x00000001,
	ELC_XADT_6CVBS_CARD		= 0x00000002,
	ELC_XADT_XA001AU		= 0x00000004,
	ELC_XADT_XVA			= 0x00000008,
};


#define VOLUME_100_TO_65535(x)		\
	if (x < 0) x = 0;				\
	else if (x > 100) x = 100;		\
	x = x * 65535 / 100; 

#define VOLUME_65535_TO_100(x)		\
	if (x < 0) x = 0;				\
	else if (x > 65535) x = 65535;	\
	x = x * 100 / 65535;


#define ELC_MAX_VOLUME					100
#define ELC_MIN_VOLUME					0


// Device Name On XP
// XVA 
#define ELC_XVA_PLAYBACK_NAME_XP			_T("Playback (XVA Adapter)")
#define ELC_XVA_LOOPBACK_NAME_XP			_T("Loopback (XVA Adapter)")
// XA001AU
#define ELC_XA001AU_PLAYBACK_NAME_XP		_T("Playback (XA001AU Adapter)")
#define ELC_XA001AU_LOOPBACK_NAME_XP		_T("Loopback (XA001AU Adapter)")
#define ELC_XA001AU_VOICE_PLAYBACK_NAME_XP	_T("Voice (XA001AU Adapter)")
#define ELC_XA001AU_RECORD_NAME_XP			_T("Record (XA001AU Adapter)")

// Device Pnp ID 
// XVA 
#define ELC_XVA_ID							_T("root#media")
#define ELC_XVA_PLAYBACK					_T("\\playback")
#define ELC_XVA_LOOPBACK					_T("\\loopback")
// XA001AU
#define ELC_XA001AU_HARDWARE_ID				_T("usb#vid_ffad&pid_a001")
#define ELC_XA001AU_PLAYBACK				_T("\\playback")
#define ELC_XA001AU_VOICE_PLAYBACK			_T("\\voiceplayback")
#define ELC_XA001AU_RECORD					_T("\\record")
#define ELC_XA001AU_LOOPBACK				_T("\\loopback")

// XI Video Adapter
#define XI_VGA_CARD_IDENTIFIER				_T("pci#ven_1172&dev_0004")
#define XI_6CVBS_CARD_IDENTIFIER			_T("pci#ven_1172&dev_a006")

#define XI_6CVBS_CARD_NAME					_T("XI006AE")
#define XI_VGA_CARD_NAME					_T("XI102AE")


// Screen Capture
#define TD_SCREEN_CAPTURE_IDENTIFIER		_T("Universal Networks Screen Capture")


// voice box jack status, define in "XA001AUProp.h"
// #define JACK_HEADPHONE_OUT					0x01 
// #define JACK_MICROPHONE_IN					0x02
// #define JACK_LINE_IN							0x04
#define JACK_QUERY_ERROR						0xFFFF

#define XVA_REAL_PLAYBACK_READY					0x01
#define XVA_MIC_READY							0x02
#define XVA_QUERY_ERROR							0xFFFF

#define VOICE_STATUS_MASK						0xFFFF

#define VOICE_BOX_MODE							0x10000000
#define XVA_MODE								0x20000000
#define VOICE_MODE_MASK							0xF0000000


// value : 0 ~ 100
// return value : -100db ~ 0db
inline int linear2db(int value) {
	double dValue = (double)value;
	if (dValue <= 0) dValue = 0.001;
	else if (dValue > 100) dValue = 100.0;
	return (int)((double)20.0 * log10((double)((double)dValue / (double)100.0)));
}

// value : -100db ~ 0db
// return : 0 ~ 100
inline int db2linear(int value) {
	return (int)(pow(10.0, (double)value / 20) * 100);
}

// value : 0 ~ 100
// return value : -10000 ~ 0
inline int Convert2DSVolume(int value)
{
	value *= 100;
	if (value < 100) value = 100;
	else if (value > 10000) value = 10000;

	value = (int)(log10((double)value / 31.62) * 4000.0) - 10000;
	if (value < -10000) value = -10000;
	else if (value > 0) value = 0;

	return value;
}