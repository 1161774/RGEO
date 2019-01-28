#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"


#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"


extern const char* BUZZER;




#define NOTE_C0	16	//16.35
#define NOTE_Cs0	17	//17.32
#define NOTE_Df0 	17	//17.32
#define NOTE_D0	18	//18.35
#define NOTE_Ds0	19	//19.45
#define NOTE_Ef0 	19	//19.45
#define NOTE_E0	21	//20.6
#define NOTE_F0	22	//21.83
#define NOTE_Fs0	23	//23.12
#define NOTE_Gf0 	23	//23.12
#define NOTE_G0	25	//24.5
#define NOTE_Gs0	26	//25.96
#define NOTE_Af0 	26	//25.96
#define NOTE_A0	28	//27.5
#define NOTE_As0	29	//29.14
#define NOTE_Bf0 	29	//29.14
#define NOTE_B0	31	//30.87
#define NOTE_C1	33	//32.7
#define NOTE_Cs1	35	//34.65
#define NOTE_Df1 	35	//34.65
#define NOTE_D1	37	//36.71
#define NOTE_Ds1	39	//38.89
#define NOTE_Ef1 	39	//38.89
#define NOTE_E1	41	//41.2
#define NOTE_F1	44	//43.65
#define NOTE_Fs1	46	//46.25
#define NOTE_Gf1 	46	//46.25
#define NOTE_G1	49	//49
#define NOTE_Gs1	52	//51.91
#define NOTE_Af1 	52	//51.91
#define NOTE_A1	55	//55
#define NOTE_As1	58	//58.27
#define NOTE_Bf1 	58	//58.27
#define NOTE_B1	62	//61.74
#define NOTE_C2	65	//65.41
#define NOTE_Cs2	69	//69.3
#define NOTE_Df2 	69	//69.3
#define NOTE_D2	73	//73.42
#define NOTE_Ds2	78	//77.78
#define NOTE_Ef2 	78	//77.78
#define NOTE_E2	82	//82.41
#define NOTE_F2	87	//87.31
#define NOTE_Fs2	93	//92.5
#define NOTE_Gf2 	93	//92.5
#define NOTE_G2	98	//98
#define NOTE_Gs2	104	//103.83
#define NOTE_Af2 	104	//103.83
#define NOTE_A2	110	//110
#define NOTE_As2	117	//116.54
#define NOTE_Bf2 	117	//116.54
#define NOTE_B2	123	//123.47
#define NOTE_C3	131	//130.81
#define NOTE_Cs3	139	//138.59
#define NOTE_Df3 	139	//138.59
#define NOTE_D3	147	//146.83
#define NOTE_Ds3	156	//155.56
#define NOTE_Ef3 	156	//155.56
#define NOTE_E3	165	//164.81
#define NOTE_F3	175	//174.61
#define NOTE_Fs3	185	//185
#define NOTE_Gf3 	185	//185
#define NOTE_G3	196	//196
#define NOTE_Gs3	208	//207.65
#define NOTE_Af3 	208	//207.65
#define NOTE_A3	220	//220
#define NOTE_As3	233	//233.08
#define NOTE_Bf3 	233	//233.08
#define NOTE_B3	247	//246.94
#define NOTE_C4	262	//261.63
#define NOTE_Cs4	277	//277.18
#define NOTE_Df4 	277	//277.18
#define NOTE_D4	294	//293.66
#define NOTE_Ds4	311	//311.13
#define NOTE_Ef4 	311	//311.13
#define NOTE_E4	330	//329.63
#define NOTE_F4	349	//349.23
#define NOTE_Fs4	370	//369.99
#define NOTE_Gf4 	370	//369.99
#define NOTE_G4	392	//392
#define NOTE_Gs4	415	//415.3
#define NOTE_Af4 	415	//415.3
#define NOTE_A4	440	//440
#define NOTE_As4	466	//466.16
#define NOTE_Bf4 	466	//466.16
#define NOTE_B4	494	//493.88
#define NOTE_C5	523	//523.25
#define NOTE_Cs5	554	//554.37
#define NOTE_Df5 	554	//554.37
#define NOTE_D5	587	//587.33
#define NOTE_Ds5	622	//622.25
#define NOTE_Ef5 	622	//622.25
#define NOTE_E5	659	//659.25
#define NOTE_F5	698	//698.46
#define NOTE_Fs5	740	//739.99
#define NOTE_Gf5 	740	//739.99
#define NOTE_G5	784	//783.99
#define NOTE_Gs5	831	//830.61
#define NOTE_Af5 	831	//830.61
#define NOTE_A5	880	//880
#define NOTE_As5	932	//932.33
#define NOTE_Bf5 	932	//932.33
#define NOTE_B5	988	//987.77
#define NOTE_C6	1047	//1046.5
#define NOTE_Cs6	1109	//1108.73
#define NOTE_Df6 	1109	//1108.73
#define NOTE_D6	1175	//1174.66
#define NOTE_Ds6	1245	//1244.51
#define NOTE_Ef6 	1245	//1244.51
#define NOTE_E6	1319	//1318.51
#define NOTE_F6	1397	//1396.91
#define NOTE_Fs6	1480	//1479.98
#define NOTE_Gf6 	1480	//1479.98
#define NOTE_G6	1568	//1567.98
#define NOTE_Gs6	1661	//1661.22
#define NOTE_Af6 	1661	//1661.22
#define NOTE_A6	1760	//1760
#define NOTE_As6	1865	//1864.66
#define NOTE_Bf6 	1865	//1864.66
#define NOTE_B6	1976	//1975.53
#define NOTE_C7	2093	//2093
#define NOTE_Cs7	2217	//2217.46
#define NOTE_Df7 	2217	//2217.46
#define NOTE_D7	2349	//2349.32
#define NOTE_Ds7	2489	//2489.02
#define NOTE_Ef7 	2489	//2489.02
#define NOTE_E7	2637	//2637.02
#define NOTE_F7	2794	//2793.83
#define NOTE_Fs7	2960	//2959.96
#define NOTE_Gf7 	2960	//2959.96
#define NOTE_G7	3136	//3135.96
#define NOTE_Gs7	3322	//3322.44
#define NOTE_Af7 	3322	//3322.44
#define NOTE_A7	3520	//3520
#define NOTE_As7	3729	//3729.31
#define NOTE_Bf7 	3729	//3729.31
#define NOTE_B7	3951	//3951.07
#define NOTE_C8	4186	//4186.01
#define NOTE_Cs8	4435	//4434.92
#define NOTE_Df8 	4435	//4434.92
#define NOTE_D8	4699	//4698.63
#define NOTE_Ds8	4978	//4978.03
#define NOTE_Ef8 	4978	//4978.03
#define NOTE_E8	5274	//5274.04
#define NOTE_F8	5588	//5587.65
#define NOTE_Fs8	5920	//5919.91
#define NOTE_Gf8 	5920	//5919.91
#define NOTE_G8	6272	//6271.93
#define NOTE_Gs8	6645	//6644.88
#define NOTE_Af8 	6645	//6644.88
#define NOTE_A8	7040	//7040
#define NOTE_As8	7459	//7458.62
#define NOTE_Bf8 	7459	//7458.62
#define NOTE_B8	7902	//7902.13



void BuzzerTask();