#ifndef __ELCPORT_H__
#define __ELCPORT_H__

// Find Teacher
#ifdef _TOPDOMAIN
	#define ELC_FINDTEACHER_ADDR	"224.50.50.41"
	#define ELC_FINDTEACHER_PORT	4704

	// Com Layer
	#define ELC_COMLAYER_ADDR		"224.50.50.42"
	#define ELC_COMLAYER_PORT		4705
#else
#define ELC_FINDTEACHER_ADDR	"224.40.40.41"
#define ELC_FINDTEACHER_PORT	4804

// Com Layer
#define ELC_COMLAYER_ADDR		"224.40.40.42"
#define ELC_COMLAYER_PORT		4805
#endif

// TCP Port
#define ELC_TCPMODE_PORT		4806

// License Server
#define ELC_LICENSE_ADDR		"224.20.20.20"
#define ELC_LICENSE_PORT		4808

// Quiz Grade Port
#define ELC_QUIZGRADE_COMIP		"224.60.60.43"
#define ELC_QUIZGRADE_PORT		4809

#endif //__ELCPORT_H__