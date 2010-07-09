#include "typedefs.h"
#include "ata_aci_send.h"
#include "ata_api.h"
#include "aci_cmd.h"
#include "aci_lst.h"
#include "aci_prs.h"
#include "aci_cmh.h"
#include "ati_cmd.h"
#define MAX_CMD_LEN 512
#include "ati_int.h"
#include "line_edit.h"
#include "Debug.h"


T_ATI_RSLT atPlusCMGL (char *cl, UBYTE srcId);
T_ATI_RSLT atPlusCMGS (char *cl, UBYTE srcId);
T_ATI_RSLT atPlusCMSS (char *cl, UBYTE srcId);
T_ATI_RSLT atPlusCMGW (char *cl, UBYTE srcId);
T_ATI_RSLT atPlusCMGF (char *cl, UBYTE srcId);
EXTERN BOOL _g_ati_trc_enabled;
EXTERN AciCmdVars at;
extern void updateScreen(void);
extern BYTE ccd_init(void);

extern void tweetRecvSMS(char *number, char *text);

static int s_awaitingSMS = -1;
static int msg_cb = 0;

struct SMSMsg
{
	char txtnumber[20];
	char txtmsg[200];
} msgList[2];

void ati_response_cb(UBYTE src_id, T_ATI_OUTPUT_TYPE output_type, UBYTE *output, USHORT output_len)
{
	char *p,*c;

	if (!strcmp((char *)output, "OK"))
	{
		s_awaitingSMS = -1;
		if (msgList[0].txtnumber[0])
			tweetRecvSMS(msgList[0].txtnumber, msgList[0].txtmsg);
		if (msgList[1].txtnumber[0])
			tweetRecvSMS(msgList[1].txtnumber, msgList[1].txtmsg);
	}	

	if (s_awaitingSMS == -1)
	{
		s_awaitingSMS = 0;
		memset(msgList, 0, sizeof(*msgList));
	}
#ifdef EMO_DEBUG
	emo_printf("ANDREY: ati_response_cb(%d, %d, %x, %d) = [%s]", src_id, output_type, output, output_len, output);
#endif
	if(msg_cb) {
	 	strcpy(msgList[s_awaitingSMS % 2].txtmsg, (char*)output);
#ifdef EMO_DEBUG
		emo_printf("CMW: Message - <%s> Number - <%s>", msgList[s_awaitingSMS % 2].txtmsg, msgList[s_awaitingSMS % 2].txtnumber);
#endif
		msg_cb=0;
		s_awaitingSMS = (s_awaitingSMS + 1) % 2;
		return;
	}
  p=strchr((char *)output,',');
	if(!p) return;
  p=strchr(++p,',');
	if(!p) return;
  p=strchr(++p,'"');
	if(!p) return;
  p++;
  c=strchr(p,'"');
	if(!c)return;
  *c=0;

  strcpy(msgList[s_awaitingSMS % 2].txtnumber, p);

	msg_cb=1;
	return;
}


BOOL ATCMD(UBYTE *chars, USHORT len)
{
  BOOL  S3_CR_found = FALSE;
  UBYTE i;
  T_ATI_SRC_PARAMS *src_params = NULL;
  static UBYTE     cnf_src_id = 0;
  T_LEDIT_ATCMD   *cmd = NULL;
#ifdef EMO_DEBUG
  emo_printf("ANDREY: ATCMD()");
#endif
  if (!cnf_src_id)
  {
    cnf_src_id = ati_init (ATI_SRC_TYPE_TST, (T_ATI_RESULT_CB *)ati_response_cb, NULL);
    if (!cnf_src_id)
    {
#ifdef EMO_DEBUG
      emo_printf("[ERR] ANDREY: ATCMD: cannot create source");
#endif
      return (FALSE);
    }
#ifdef EMO_DEBUG
    emo_printf("ANDREY: ATCMD: srcId=%d", cnf_src_id);
#endif
  }
  src_params = (T_ATI_SRC_PARAMS *)find_element (ati_src_list, cnf_src_id, search_ati_src_id);

  if (src_params EQ NULL)
  {
#ifdef EMO_DEBUG
    emo_printf("[ERR] ANDREY: ATCMD: source ID=%d not found",
                    cnf_src_id);
#endif
    return (FALSE);
  }
  if (src_params->cmd_state EQ CMD_RUNNING)
  {
    if ((ledit_get_current (src_params->src_id, &cmd) EQ LEDIT_CMPL) AND cmd)
    {
#ifdef EMO_DEBUG
      emo_printf("[WRN] ANDREY: ATCMD: command=%s is running", cmd->name);
#endif
	}
    else
    {
#ifdef EMO_DEBUG
      emo_printf("[ERR] ANDREY: ATCMD: command not available !"); /* then we have a real problem */
#endif
    }
    //return (FALSE);
  }

  for (i=0; i < len; i++)
  {
    if( chars[i] EQ '\r' ) /*  */
    {
      S3_CR_found = TRUE;
      break;
    }
  }

  if (S3_CR_found EQ FALSE)
  {
    /*
     * tell line edit that the line termination is not default '\r'
     */
    T_LEDIT line;
    line.S3  = 0x00; /* <== NULL terminated cmd line */
    line.S4  = at.S[4];
    line.S5  = at.S[5];
    line.smsEnd = 0x1a;
    line.atE = ati_user_output_cfg[src_params->src_id].atE;
    ledit_set_config (src_params->src_id, line);
    len = len+1;
  }

  ati_execute (src_params->src_id, chars, len);

  /* wait for command to be processed before allowing new command */
  return (FALSE);
}

void recv_sms(void) {
	char buf[256] = "AT+CMGL=\"ALL\"";
#ifdef EMO_DEBUG
	emo_printf("ANDREY: showing all %s", buf);
#endif
	ATCMD((UBYTE *)buf, strlen(buf));
}

void ATI_RESPONSE(UBYTE src_id, T_ATI_OUTPUT_TYPE output_type, UBYTE *output, USHORT output_len)
{
#ifdef EMO_DEBUG
	emo_printf("ANDREY: ATI_RESPONSE(%d, %d, %x, %d) = [%s]", src_id, output_type, output, output_len, output);
#endif
}

static int last_line = 0;
#define ANDREYTRACE do { \
  emo_printf("ANDREY: Got to %d in %s", __LINE__, __FILE__); \
  if (last_line < __LINE__) { \
    last_line = __LINE__; \
    return; \
  } \
} while (0)

void handleSmsSend(const char *pszNumber, const char *pszMsg)
{
	static int x = 0;
	static UBYTE aci_id = 0;
	BYTE ccret;
        T_ATI_RSLT ret;
        char szNumber[256] = "";
	char szMsg[256] = "";

//ANDREYTRACE;

	if (!x)
	{
		++x;
		aci_id = ati_init(ATI_SRC_TYPE_LC, (T_ATI_RESULT_CB *)ATI_RESPONSE, 0);
		ccret = ccd_init();

#ifdef EMO_DEBUG
		emo_printf("ANDREY: ccd_init() = %d", ccret);
#endif
	}

//ANDREYTRACE;
	sprintf(szNumber, "\"%s\",129", pszNumber);

	sprintf(szMsg, "%s\n\nSent from my Peek\x1a", pszMsg);

	//char smsMsg[] = "\"5165473093\",129;How easy was that?\x1a";
#ifdef EMO_DEBUG
	emo_printf("ANDREY: sending sms: (%s, %s)", szNumber, szMsg);
#endif

	ret = atPlusCMGS(szNumber, aci_id);
#ifdef EMO_DEBUG
	emo_printf("ANDREY: sent sms: atPlusCMGS(%s, %d) = %d", szNumber, aci_id, ret);
#endif

	ret = atPlusCMGS(szMsg, aci_id);
#ifdef EMO_DEBUG
	emo_printf("ANDREY: sent sms: atPlusCMGS(%s, %d) = %d", szMsg, aci_id, ret);
#endif
}
