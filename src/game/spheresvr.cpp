
#if !defined(_WIN32) || defined(_LIBEV)
	#include "../sphere/linuxev.h"
	#include "../sphere/UnixTerminal.h"
#endif

#if !defined(pid_t)
	#define pid_t int
#endif

#ifdef _WIN32
	#include "../sphere/ntservice.h"	// g_Service
	#include <process.h>	// getpid()
#endif

#include "../common/CException.h"
#include "../common/CUOInstall.h"
#include "../common/sphereversion.h"	// sphere version
#include "../network/network.h" // network thread
#include "../network/PingServer.h"
#include "../sphere/asyncdb.h"
#include "clients/CAccount.h"
#include "items/CItemMap.h"
#include "items/CItemMessage.h"
#include "../common/CLog.h"
#include "CScriptProfiler.h"
#include "CServer.h"
#include "CWorld.h"
#include "spheresvr.h"


bool WritePidFile(int iMode = 0)
{
	lpctstr	file = SPHERE_FILE ".pid";
	FILE	*pidFile;

	if ( iMode == 1 )		// delete
	{
		return ( STDFUNC_UNLINK(file) == 0 );
	}
	else if ( iMode == 2 )	// check for .pid file
	{
		pidFile = fopen(file, "r");
		if ( pidFile )
		{
			g_Log.Event(LOGM_INIT, SPHERE_FILE ".pid already exists. Secondary launch or unclean shutdown?\n");
			fclose(pidFile);
		}
		return true;
	}
	else
	{
		pidFile = fopen(file, "w");
		if ( pidFile )
		{
			pid_t spherepid = STDFUNC_GETPID();

			// pid_t is always an int, except on MinGW, where it is int on 32 bits and long long on 64 bits.
			#if defined(_WIN32) && !defined(_MSC_VER)
				fprintf(pidFile, "%" PRIdSSIZE_T "\n", spherepid);
			#else
				fprintf(pidFile, "%d\n", spherepid);
			#endif
			fclose(pidFile);
			return true;
		}
		g_Log.Event(LOGM_INIT, "Cannot create pid file!\n");
		return false;
	}
}

int CEventLog::VEvent( dword wMask, lpctstr pszFormat, va_list args )
{
	if ( pszFormat == NULL || pszFormat[0] == '\0' )
		return 0;

	TemporaryString pszTemp;
	size_t len = vsnprintf(pszTemp, (SCRIPT_MAX_LINE_LEN - 1), pszFormat, args);
	if ( ! len ) strncpy(pszTemp, pszFormat, (SCRIPT_MAX_LINE_LEN - 1));

	// This get rids of exploits done sending 0x0C to the log subsytem.
	// tchar *	 pFix;
	// if ( ( pFix = strchr( pszText, 0x0C ) ) )
	//	*pFix = ' ';

	return EventStr(wMask, pszTemp);
}

lpctstr const g_Stat_Name[STAT_QTY] =	// not sorted obviously.
{
	"STR",
	"INT",
	"DEX",
	"FOOD",
	"KARMA",
	"FAME",
};

lpctstr g_szServerDescription =	SPHERE_TITLE " Version " SPHERE_VERSION " " SPHERE_VER_FILEOS_STR	" by www.spherecommunity.net";

int g_szServerBuild = 0;

size_t CObjBase::sm_iCount = 0;	// UID table.
llong llTimeProfileFrequency = 1000;	// time profiler

// game servers stuff.
CWorld		g_World;	// the world. (we save this stuff)
CServer		g_Serv;		// current state stuff not saved.
CResource	g_Cfg;
CUOInstall g_Install;
CVerDataMul	g_VerData;
CExpression g_Exp;		// Global script variables.
CLog		g_Log;
CEventLog * g_pLog = &g_Log;
CAccounts	g_Accounts;	// All the player accounts. name sorted CAccount
CSStringList	g_AutoComplete;	// auto-complete list
CScriptProfiler g_profiler;		// script profiler
CUOMapList	g_MapList;			// global maps information


lpctstr GetTimeMinDesc( int minutes )
{
	tchar	*pTime = Str_GetTemp();

	int minute = minutes % 60;
	int hour = ( minutes / 60 ) % 24;

	lpctstr pMinDif;
	if ( minute <= 14 )
//		pMinDif = "";
		pMinDif = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_QUARTER_FIRST);
	else if ( ( minute >= 15 ) && ( minute <= 30 ) )
//		pMinDif = "a quarter past";
		pMinDif = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_QUARTER_SECOND);
	else if ( ( minute >= 30 ) && ( minute <= 45 ) )
		//pMinDif = "half past";
		pMinDif = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_QUARTER_THIRD);
	else
	{
//		pMinDif = "a quarter till";
		pMinDif = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_QUARTER_FOURTH);
		hour = ( hour + 1 ) % 24;
	}
/*
	static lpctstr const sm_ClockHour[] =
	{
		"midnight",
		"one",
		"two",
		"three",
		"four",
		"five",
		"six",
		"seven",
		"eight",
		"nine",
		"ten",
		"eleven",
		"noon"
	};
*/
	lpctstr sm_ClockHour[] =
	{
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_ZERO),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_ONE),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_TWO),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_THREE),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_FOUR),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_FIVE),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_SIX),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_SEVEN),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_EIGHT),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_NINE),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_TEN),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_ELEVEN),
 		g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_HOUR_TWELVE),
	};

	lpctstr pTail;
	if ( hour == 0 || hour==12 )
		pTail = "";
	else if ( hour > 12 )
	{
		hour -= 12;
		if ((hour>=1)&&(hour<6))
			pTail = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_13_TO_18);
//			pTail = " o'clock in the afternoon";
		else if ((hour>=6)&&(hour<9))
			pTail = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_18_TO_21);
//			pTail = " o'clock in the evening.";
		else
			pTail = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_21_TO_24);
//			pTail = " o'clock at night";
	}
	else
	{
		pTail = g_Cfg.GetDefaultMsg(DEFMSG_CLOCK_24_TO_12);
//		pTail = " o'clock in the morning";
	}

	sprintf( pTime, "%s %s %s", pMinDif, sm_ClockHour[hour], pTail );
	return pTime;
}

size_t FindStrWord( lpctstr pTextSearch, lpctstr pszKeyWord )
{
	// Find any of the pszKeyWord in the pTextSearch string.
	// Make sure we look for starts of words.

	size_t j = 0;
	for ( size_t i = 0; ; i++ )
	{
		if ( pszKeyWord[j] == '\0' || pszKeyWord[j] == ',')
		{
			if ( pTextSearch[i]== '\0' || ISWHITESPACE(pTextSearch[i]))
				return( i );
			j = 0;
		}
		if ( pTextSearch[i] == '\0' )
		{
			pszKeyWord = strchr(pszKeyWord, ',');
			if (pszKeyWord)
			{
				++pszKeyWord;
				i = 0;
				j = 0;
			}
			else
			return 0;
		}
		if ( j == 0 && i > 0 )
		{
			if ( IsAlpha( pTextSearch[i-1] ))	// not start of word ?
				continue;
		}
		if ( toupper( pTextSearch[i] ) == toupper( pszKeyWord[j] ))
			j++;
		else
			j = 0;
	}
}

//*******************************************************************
//	Main server loop

Main::Main()
	: AbstractSphereThread("Main", IThread::RealTime)
{
	m_profile.EnableProfile(PROFILE_NETWORK_RX);
	m_profile.EnableProfile(PROFILE_CLIENTS);
	//m_profile.EnableProfile(PROFILE_NETWORK_TX);
	m_profile.EnableProfile(PROFILE_CHARS);
	m_profile.EnableProfile(PROFILE_ITEMS);
	m_profile.EnableProfile(PROFILE_MAP);
	m_profile.EnableProfile(PROFILE_NPC_AI);
	m_profile.EnableProfile(PROFILE_SCRIPTS);
#ifndef _MTNETWORK
	//m_profile.EnableProfile(PROFILE_DATA_TX);
	m_profile.EnableProfile(PROFILE_DATA_RX);
#else
#ifndef MTNETWORK_INPUT
	m_profile.EnableProfile(PROFILE_DATA_RX);
#endif
#ifndef MTNETWORK_OUTPUT
	m_profile.EnableProfile(PROFILE_DATA_TX);
#endif
#endif
}

void Main::onStart()
{
	AbstractSphereThread::onStart();
	SetExceptionTranslator();
}

void Main::tick()
{
	Sphere_OnTick();
}

bool Main::shouldExit()
{
	if (g_Serv.m_iExitFlag != 0)
		return true;
	return AbstractSphereThread::shouldExit();
}

Main g_Main;
extern PingServer g_PingServer;
extern CDataBaseAsyncHelper g_asyncHdb;
#if !defined(_WIN32) || defined(_LIBEV)
	extern LinuxEv g_NetworkEvent;
#endif

//*******************************************************************

int Sphere_InitServer( int argc, char *argv[] )
{
	const char *m_sClassName = "Sphere";
	EXC_TRY("Init");
	ASSERT(MAX_BUFFER >= sizeof(CCommand));
	ASSERT(MAX_BUFFER >= sizeof(CEvent));
	ASSERT(sizeof(int) == sizeof(dword));	// make this assumption often.
	ASSERT(sizeof(ITEMID_TYPE) == sizeof(dword));
	ASSERT(sizeof(word) == 2 );
	ASSERT(sizeof(dword) == 4 );
	ASSERT(sizeof(nword) == 2 );
	ASSERT(sizeof(ndword) == 4 );
	ASSERT(sizeof(CUOItemTypeRec) == 37 );	// is byte packing working ?

#ifdef _WIN32
	if ( ! QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER *>(&llTimeProfileFrequency)) )
		llTimeProfileFrequency = 1000;

#if defined(_MSC_VER) && !defined(_NIGHTLYBUILD)
	// We don't need an exception translator for the Debug build, since that build would, generally, be used with a debugger.
	// We don't want that for Release build either because, in order to call _set_se_translator, we should set the /EHa
	//	compiler flag, which slows down code a bit.
	EXC_SET("setting exception catcher");
	SetExceptionTranslator();
#endif
#endif // _WIN32

#ifndef _DEBUG
	// Same as for SetExceptionTranslator, Debug build doesn't need a purecall handler.
	EXC_SET("setting purecall handler");
	SetPurecallHandler();
#endif

	EXC_SET("loading");
	if ( !g_Serv.Load() )
		return -3;

	if ( argc > 1 )
	{
		EXC_SET("cmdline");
		if ( !g_Serv.CommandLine(argc, argv) )
			return -1;
	}

	WritePidFile(2);

	EXC_SET("sockets init");
	if ( !g_Serv.SocketsInit() )
		return -9;
	EXC_SET("load world");
	if ( !g_World.LoadAll() )
		return -8;

	//	load auto-complete dictionary
	EXC_SET("auto-complete");
	{
		CSFileText	dict;
		if ( dict.Open(SPHERE_FILE ".dic", OF_READ|OF_TEXT|OF_DEFAULTMODE) )
		{
			tchar * pszTemp = Str_GetTemp();
			size_t count = 0;
			while ( !dict.IsEOF() )
			{
				dict.ReadString(pszTemp, SCRIPT_MAX_LINE_LEN-1);
				if ( *pszTemp )
				{
					tchar *c = strchr(pszTemp, '\r');
					if ( c != NULL )
						*c = '\0';

					c = strchr(pszTemp, '\n');
					if ( c != NULL )
						*c = '\0';

					if ( *pszTemp != '\0' )
					{
						count++;
						g_AutoComplete.AddTail(pszTemp);
					}
				}
			}
			g_Log.Event(LOGM_INIT, "Auto-complete dictionary loaded (contains %" PRIuSIZE_T " words).\n", count);
			dict.Close();
		}
	}
	g_Serv.SetServerMode(SERVMODE_Run);	// ready to go.

	// Display EF/OF Flags
	g_Cfg.PrintEFOFFlags();

	EXC_SET("finilizing");
	g_Log.Event(LOGM_INIT, "%s", g_Serv.GetStatusString(0x24));
	g_Log.Event(LOGM_INIT, "Startup complete. items=%" PRIuSIZE_T ", chars=%" PRIuSIZE_T "\n", g_Serv.StatGet(SERV_STAT_ITEMS), g_Serv.StatGet(SERV_STAT_CHARS));

#ifdef _WIN32
	g_Log.Event(LOGM_INIT, "Press '?' for console commands.\n");
#endif

	// Trigger server start
	g_Serv.r_Call("f_onserver_start", &g_Serv, NULL);
	return 0;

	EXC_CATCH;

	EXC_DEBUG_START;
	g_Log.EventDebug("cmdline argc=%d starting with %p (argv1='%s')\n", argc, static_cast<void *>(argv), ( argc > 2 ) ? argv[1] : "");
	EXC_DEBUG_END;
	return -10;
}

void Sphere_ExitServer()
{
	// Trigger server quit
	g_Serv.r_Call("f_onserver_exit", &g_Serv, NULL);

	g_Serv.SetServerMode(SERVMODE_Exiting);

#ifndef _MTNETWORK
	g_NetworkOut.waitForClose();
#else
	g_NetworkManager.stop();
#endif
	g_Main.waitForClose();
	g_PingServer.waitForClose();
	g_asyncHdb.waitForClose();
#if !defined(_WIN32) || defined(_LIBEV)
	if ( g_Cfg.m_fUseAsyncNetwork != 0 )
		g_NetworkEvent.waitForClose();
#endif

	g_Serv.SocketsClose();
	g_World.Close();

	lpctstr Reason;
	switch ( g_Serv.m_iExitFlag )
	{
		case -10:	Reason = "Unexpected error occurred";			break;
		case -9:	Reason = "Failed to bind server IP/port";		break;
		case -8:	Reason = "Failed to load worldsave files";		break;
		case -3:	Reason = "Failed to load server settings";		break;
		case -1:	Reason = "Shutdown via commandline";			break;
#ifdef _WIN32
		case 1:		Reason = "X command on console";				break;
#else
		case 1:		Reason = "Terminal closed by SIGHUP signal";	break;
#endif
		case 2:		Reason = "SHUTDOWN command executed";			break;
		case 4:		Reason = "Service shutdown";					break;
		case 5:		Reason = "Console window closed";				break;
		case 6:		Reason = "Proccess aborted by SIGABRT signal";	break;
		default:	Reason = "Server shutdown complete";			break;
	}

	g_Log.Event(LOGM_INIT|LOGL_FATAL, "Server terminated: %s (code %d)\n", Reason, g_Serv.m_iExitFlag);
	g_Log.Close();
}

int Sphere_OnTick()
{
	// Give the world (CMainTask) a single tick. RETURN: 0 = everything is fine.
	const char *m_sClassName = "Sphere";
	EXC_TRY("Tick");
#ifdef _WIN32
	EXC_SET("service");
	g_Service.OnTick();
#endif
	EXC_SET("ships_tick");
	g_Serv.ShipTimers_Tick();

	EXC_SET("world");
	if (g_Cfg.m_bMySql && g_Cfg.m_bMySqlTicks)
		g_World.OnTickMySQL();
	else
		g_World.OnTick();

	// process incoming data
	EXC_SET("network-in");
#ifndef _MTNETWORK
	g_NetworkIn.tick();
#else
	g_NetworkManager.processAllInput();
#endif

	EXC_SET("server");
	g_Serv.OnTick();

	// push outgoing data
#ifndef _MTNETWORK
	if (g_NetworkOut.isActive() == false)
	{
		EXC_SET("network-out");
		g_NetworkOut.tick();
	}
#else
	EXC_SET("network-tick");
	g_NetworkManager.tick();

	EXC_SET("network-out");
	g_NetworkManager.processAllOutput();
#endif

	EXC_CATCH;
	return g_Serv.m_iExitFlag;
}
//*****************************************************

static void Sphere_MainMonitorLoop()
{
	const char *m_sClassName = "Sphere";
	// Just make sure the main loop is alive every so often.
	// This should be the parent thread. try to restart it if it is not.
	while ( ! g_Serv.m_iExitFlag )
	{
		EXC_TRY("MainMonitorLoop");

		if ( g_Cfg.m_iFreezeRestartTime <= 0 )
		{
			DEBUG_ERR(("Freeze Restart Time cannot be cleared at run time\n"));
			g_Cfg.m_iFreezeRestartTime = 10;
		}

		EXC_SET("Sleep");
		// only sleep 1 second at a time, to avoid getting stuck here when closing
		// down with large m_iFreezeRestartTime values set
		for (int i = 0; i < g_Cfg.m_iFreezeRestartTime; ++i)
		{
			if ( g_Serv.m_iExitFlag )
				break;

#ifdef _WIN32
			NTWindow_OnTick(1000);
#else
			Sleep(1000);
#endif
		}

		EXC_SET("Checks");
		// Don't look for freezing when doing certain things.
		if ( g_Serv.IsLoading() || ! g_Cfg.m_fSecure || g_Serv.IsValidBusy() )
			continue;

		EXC_SET("Check Stuck");
#ifndef _DEBUG
		if (g_Main.checkStuck() == true)
			g_Log.Event(LOGL_CRIT, "'%s' thread hang, restarting...\n", g_Main.getName());
#endif
		EXC_CATCH;
	}

}

//******************************************************
void dword_q_sort(dword numbers[], dword left, dword right)
{
	dword	pivot, l_hold, r_hold;

	l_hold = left;
	r_hold = right;
	pivot = numbers[left];
	while (left < right)
	{
		while ((numbers[right] >= pivot) && (left < right)) right--;
		if (left != right)
		{
			numbers[left] = numbers[right];
			left++;
		}
		while ((numbers[left] <= pivot) && (left < right)) left++;
		if (left != right)
		{
			numbers[right] = numbers[left];
			right--;
		}
	}
	numbers[left] = pivot;
	pivot = left;
	left = l_hold;
	right = r_hold;
	if (left < pivot) dword_q_sort(numbers, left, pivot-1);
	if (right > pivot) dword_q_sort(numbers, pivot+1, right);
}

void defragSphere(char *path)
{
	ASSERT(path != NULL);

	CSFileText inf;
	CSFile ouf;
	char z[256], z1[256], buf[1024];
	size_t i;
	dword uid(0);
	char *p(NULL), *p1(NULL);
	size_t dBytesRead;
	size_t dTotalMb;
	size_t mb10(10*1024*1024);
	size_t mb5(5*1024*1024);
	bool bSpecial;
	dword dTotalUIDs;

	char	c,c1,c2;
	dword	d;

	//	NOTE: Sure I could use CVarDefArray, but it is extremely slow with memory allocation, takes hours
	//		to read and save the data. Moreover, it takes less memory in this case and does less convertations.
#define	MAX_UID	5000000L	// limit to 5mln of objects, takes 5mln*4 = 20mb
	dword	*uids;

	g_Log.Event(LOGM_INIT,	"Defragmentation (UID alteration) of " SPHERE_TITLE " saves.\n"
		"Use it on your risk and if you know what you are doing since it can possibly harm your server.\n"
		"The process can take up to several hours depending on the CPU you have.\n"
		"After finished, you will have your '" SPHERE_FILE "*.scp' files converted and saved as '" SPHERE_FILE "*.scp.new'.\n");

	uids = (dword*)calloc(MAX_UID, sizeof(dword));
	for ( i = 0; i < 3; i++ )
	{
		strcpy(z, path);
		if ( i == 0 ) strcat(z, SPHERE_FILE "statics" SPHERE_SCRIPT);
		else if ( i == 1 ) strcat(z, SPHERE_FILE "world" SPHERE_SCRIPT);
		else strcat(z, SPHERE_FILE "chars" SPHERE_SCRIPT);

		g_Log.Event(LOGM_INIT, "Reading current UIDs: %s\n", z);
		if ( !inf.Open(z, OF_READ|OF_TEXT|OF_DEFAULTMODE) )
		{
			g_Log.Event(LOGM_INIT, "Cannot open file for reading. Skipped!\n");
			continue;
		}
		dBytesRead = dTotalMb = 0;
		while ( !feof(inf.m_pStream) )
		{
			fgets(buf, sizeof(buf), inf.m_pStream);
			dBytesRead += strlen(buf);
			if ( dBytesRead > mb10 )
			{
				dBytesRead -= mb10;
				dTotalMb += 10;
				g_Log.Event(LOGM_INIT, "Total read %" PRIuSIZE_T " Mb\n", dTotalMb);
			}
			if (( buf[0] == 'S' ) && ( strstr(buf, "SERIAL=") == buf ))
			{
				p = buf + 7;
				p1 = p;
				while ( *p1 && ( *p1 != '\r' ) && ( *p1 != '\n' ))
					p1++;
				*p1 = 0;

				//	prepare new uid
				*(p-1) = '0';
				*p = 'x';
				p--;
				uids[uid++] = strtoul(p, &p1, 16);
			}
		}
		inf.Close();
	}
	dTotalUIDs = uid;
	g_Log.Event(LOGM_INIT, "Totally having %u unique objects (UIDs), latest: 0%x\n", uid, uids[uid-1]);

	g_Log.Event(LOGM_INIT, "Quick-Sorting the UIDs array...\n");
	dword_q_sort(uids, 0, dTotalUIDs-1);

	for ( i = 0; i < 5; i++ )
	{
		strcpy(z, path);
		if ( !i ) strcat(z, SPHERE_FILE "accu.scp");
		else if ( i == 1 ) strcat(z, SPHERE_FILE "chars" SPHERE_SCRIPT);
		else if ( i == 2 ) strcat(z, SPHERE_FILE "data" SPHERE_SCRIPT);
		else if ( i == 3 ) strcat(z, SPHERE_FILE "world" SPHERE_SCRIPT);
		else if ( i == 4 ) strcat(z, SPHERE_FILE "statics" SPHERE_SCRIPT);
		g_Log.Event(LOGM_INIT, "Updating UID-s in %s to %s.new\n", z, z);
		if ( !inf.Open(z, OF_READ|OF_TEXT|OF_DEFAULTMODE) )
		{
			g_Log.Event(LOGM_INIT, "Cannot open file for reading. Skipped!\n");
			continue;
		}
		strcat(z, ".new");
		if ( !ouf.Open(z, OF_WRITE|OF_CREATE|OF_DEFAULTMODE) )
		{
			g_Log.Event(LOGM_INIT, "Cannot open file for writing. Skipped!\n");
			continue;
		}
		dBytesRead = dTotalMb = 0;
		while ( inf.ReadString(buf, sizeof(buf)) )
		{
			uid = (dword)strlen(buf);
			if (uid > (CountOf(buf) - 3))
				uid = CountOf(buf) - 3;

			buf[uid] = buf[uid+1] = buf[uid+2] = 0;	// just to be sure to be in line always
							// NOTE: it is much faster than to use memcpy to clear before reading
			bSpecial = false;
			dBytesRead += uid;
			if ( dBytesRead > mb5 )
			{
				dBytesRead -= mb5;
				dTotalMb += 5;
				g_Log.Event(LOGM_INIT, "Total processed %" PRIuSIZE_T " Mb\n", dTotalMb);
			}
			p = buf;

			//	Note 28-Jun-2004
			//	mounts seems having ACTARG1 > 0x30000000. The actual UID is ACTARG1-0x30000000. The
			//	new also should be new+0x30000000. need investigation if this can help making mounts
			//	not to disappear after the defrag
			if (( buf[0] == 'A' ) && ( strstr(buf, "ACTARG1=0") == buf ))		// ACTARG1=
				p += 8;
			else if (( buf[0] == 'C' ) && ( strstr(buf, "CONT=0") == buf ))			// CONT=
				p += 5;
			else if (( buf[0] == 'C' ) && ( strstr(buf, "CHARUID=0") == buf ))		// CHARUID=
				p += 8;
			else if (( buf[0] == 'L' ) && ( strstr(buf, "LASTCHARUID=0") == buf ))	// LASTCHARUID=
				p += 12;
			else if (( buf[0] == 'L' ) && ( strstr(buf, "LINK=0") == buf ))			// LINK=
				p += 5;
			else if (( buf[0] == 'M' ) && ( strstr(buf, "MEMBER=0") == buf ))		// MEMBER=
			{
				p += 7;
				bSpecial = true;
			}
			else if (( buf[0] == 'M' ) && ( strstr(buf, "MORE1=0") == buf ))		// MORE1=
				p += 6;
			else if (( buf[0] == 'M' ) && ( strstr(buf, "MORE2=0") == buf ))		// MORE2=
				p += 6;
			else if (( buf[0] == 'S' ) && ( strstr(buf, "SERIAL=0") == buf ))		// SERIAL=
				p += 7;
			else if ((( buf[0] == 'T' ) && ( strstr(buf, "TAG.") == buf )) ||		// TAG.=
					 (( buf[0] == 'R' ) && ( strstr(buf, "REGION.TAG") == buf )))
			{
				while ( *p && ( *p != '=' )) p++;
				p++;
			}
			else if (( i == 2 ) && strchr(buf, '='))	// spheredata.scp - plain VARs
			{
				while ( *p && ( *p != '=' )) p++;
				p++;
			}
			else p = NULL;

			//	UIDs are always hex, so prefixed by 0
			if ( p && ( *p != '0' )) p = NULL;

			//	here we got potentialy UID-contained variable
			//	check if it really is only UID-like var containing
			if ( p )
			{
				p1 = p;
				while ( *p1 &&
					((( *p1 >= '0' ) && ( *p1 <= '9' )) ||
					 (( *p1 >= 'a' ) && ( *p1 <= 'f' )))) p1++;
				if ( !bSpecial )
				{
					if ( *p1 && ( *p1 != '\r' ) && ( *p1 != '\n' )) // some more text in line
						p = NULL;
				}
			}

			//	here we definitely know that this is very uid-like
			if ( p )
			{
				c = *p1;

				*p1 = 0;
				//	here in p we have the current value of the line.
				//	check if it is a valid UID

				//	prepare converting 0.. to 0x..
				c1 = *(p-1);
				c2 = *p;
				*(p-1) = '0';
				*p = 'x';
				p--;
				uid = strtoul(p, &p1, 16);
				p++;
				*(p-1) = c1;
				*p = c2;
				//	Note 28-Jun-2004
				//	The search algourytm is very simple and fast. But maybe integrate some other, at least /2 algorythm
				//	since has amount/2 tryes at worst chance to get the item and never scans the whole array
				//	It should improve speed since defragmenting 150Mb saves takes ~2:30 on 2.0Mhz CPU
				{
					dword	dStep = dTotalUIDs/2;
					d = dStep;
					for (;;)
					{
						dStep /= 2;

						if ( uids[d] == uid )
						{
							uid = d | (uids[d]&0xF0000000);	// do not forget attach item and special flags like 04..
							break;
						}
						else if ( uids[d] < uid ) d += dStep;
						else d -= dStep;

						if ( dStep == 1 )
						{
							uid = 0xFFFFFFFFL;
							break; // did not find the UID
						}
					}
				}

				//	Search for this uid in the table
/*				for ( d = 0; d < dTotalUIDs; d++ )
				{
					if ( !uids[d] )	// end of array
					{
						uid = 0xFFFFFFFFL;
						break;
					}
					else if ( uids[d] == uid )
					{
						uid = d | (uids[d]&0xF0000000);	// do not forget attach item and special flags like 04..
						break;
					}
				}*/

				//	replace UID by the new one since it has been found
				*p1 = c;
				if ( uid != 0xFFFFFFFFL )
				{
					*p = 0;
					strcpy(z, p1);
					sprintf(z1, "0%x", uid);
					strcat(buf, z1);
					strcat(buf, z);
				}
			}
			//	output the resulting line
			ouf.Write(buf, strlen(buf));
		}
		inf.Close();
		ouf.Close();
	}
	free(uids);
	g_Log.Event(LOGM_INIT,	"Defragmentation complete.\n");
}

#ifdef _WIN32
int Sphere_MainEntryPoint( int argc, char *argv[] )
#else
int _cdecl main( int argc, char * argv[] )
#endif
{
#ifndef _WIN32
	// Initialize nonblocking IO and disable readline on linux
	g_UnixTerminal.prepare();
#endif

	g_Serv.m_iExitFlag = Sphere_InitServer( argc, argv );
	if ( ! g_Serv.m_iExitFlag )
	{
		WritePidFile();

		// Start the ping server, this can only be ran in a separate thread
		if ( IsSetEF( EF_UsePingServer ) )
			g_PingServer.start();

#if !defined(_WIN32) || defined(_LIBEV)
		if ( g_Cfg.m_fUseAsyncNetwork != 0 )
			g_NetworkEvent.start();
#endif

#ifndef _MTNETWORK
		g_NetworkIn.onStart();
		if (IsSetEF( EF_NetworkOutThread ))
			g_NetworkOut.start();
#else
		g_NetworkManager.start();
#endif

		bool shouldRunInThread = ( g_Cfg.m_iFreezeRestartTime > 0 );

		if( shouldRunInThread )
		{
			g_Main.start();
			Sphere_MainMonitorLoop();
		}
		else
		{
			while( !g_Serv.m_iExitFlag )
			{
				g_Main.tick();
			}
		}
	}

#ifdef _WIN32
	NTWindow_DeleteIcon();
#endif

	Sphere_ExitServer();
	WritePidFile(true);
	return( g_Serv.m_iExitFlag );
}

#include "../tables/classnames.tbl"
