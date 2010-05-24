
#define SET_UI_RINGTONE 1
#define SET_UI_VOLUME   2
#define SET_UI_VIBRATE  3

/* Midi Types
Classic  --- 1 -- /original.mid
type2    --- 2 -- /type2.mid
type3    --- 3 -- /type3.mid
type4    --- 4 -- /type4.mid
type5    --- 5 -- /type5.mid
*/

static const char *g_TypeName[] = {"Classic", "type2", "type3", "type4", "type5"};
static const int g_TypeValue[] = {1, 2, 3, 4, 5};
#define VOLUME_OFF_VALUE 9

/*Volume
Quiet    --- 5
Normal   --- 7
Loud     --- 8
Off      --- VOLUME_OFF_VALUE
*/

static const char *g_VolumeName[] = {"Quiet", "Normal", "Loud", "Off"};
static const int g_VolumeValue[] = {5, 7, 8, VOLUME_OFF_VALUE};
#define VIBRATE_OFF_VALUE 5

/*Vibrate
Low       --- 1
Medium    --- 2
High      --- 4
Off       --- VIBRATE_OFF_VALUE
*/

static const int g_VibrateValue[] = {1, 2, 4, VIBRATE_OFF_VALUE};

void SetSettingValue(const int UIType, const int value);

void Sounds_PlayMidi(void);

void Sounds_StartVibrate(unsigned int timeout);
void flash_led(void);
