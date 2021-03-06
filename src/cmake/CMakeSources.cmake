# Main game files.
SET (game_SRCS
game/CBase.cpp
game/CBase.h
game/CContainer.cpp
game/CContainer.h
game/CObjBase.cpp
game/CObjBase.h
game/CPathFinder.cpp
game/CPathFinder.h
game/CResourceCalc.cpp
game/CResource.cpp
game/CResourceDef.cpp
game/CResource.h
game/CScriptProfiler.h
game/CSector.cpp
game/CSector.h
game/CSectorEnviron.h
game/CSectorEnviron.cpp
game/CServer.cpp
game/CServer.h
game/CServerDef.cpp
game/CServerDef.h
game/CServerTime.cpp
game/CServerTime.h
game/CWebPage.cpp
game/CWorld.cpp
game/CWorld.h
game/CWorldImport.cpp
game/CWorldMap.cpp
game/game_enums.h
game/game_macros.h
game/spheresvr.cpp
game/spheresvr.h
game/triggers.h
game/triggers.cpp
)
SOURCE_GROUP (game FILES ${game_SRCS})

SET (items_SRCS
game/items/CItem.cpp
game/items/CItem.h
game/items/CItemBase.cpp
game/items/CItemBase.h
game/items/CItemCommCrystal.cpp
game/items/CItemCommCrystal.h
game/items/CItemContainer.cpp
game/items/CItemContainer.h
game/items/CItemCorpse.cpp
game/items/CItemCorpse.h
game/items/CItemMap.cpp
game/items/CItemMap.h
game/items/CItemMemory.cpp
game/items/CItemMemory.h
game/items/CItemMessage.cpp
game/items/CItemMessage.h
game/items/CItemMulti.cpp
game/items/CItemMulti.h
game/items/CItemMultiCustom.cpp
game/items/CItemMultiCustom.h
game/items/CItemPlant.cpp
game/items/CItemScript.cpp
game/items/CItemScript.h
game/items/CItemShip.cpp
game/items/CItemShip.h
game/items/CItemSpawn.cpp
game/items/CItemSpawn.h
game/items/CItemScript.cpp
game/items/CItemScript.h
game/items/CItemStone.cpp
game/items/CItemStone.h
game/items/CItemVendable.cpp
game/items/CItemVendable.h
)
SOURCE_GROUP (game\\items FILES ${items_SRCS})

SET (chars_SRCS
game/chars/CCharact.cpp
game/chars/CCharBase.cpp
game/chars/CChar.cpp
game/chars/CChar.h
game/chars/CCharAttacker.cpp
game/chars/CCharBase.h
game/chars/CCharFight.cpp
game/chars/CCharMemory.cpp
game/chars/CCharNotoriety.cpp
game/chars/CCharNPC.cpp
game/chars/CCharNPC.h
game/chars/CCharNPCAct.cpp
game/chars/CCharNPCAct_Fight.cpp
game/chars/CCharNPCAct_Magic.cpp
game/chars/CCharNPCAct_Vendor.cpp
game/chars/CCharNPCPet.cpp
game/chars/CCharNPCStatus.cpp
game/chars/CCharPlayer.cpp
game/chars/CCharPlayer.h
game/chars/CCharRefArray.h
game/chars/CCharRefArray.cpp
game/chars/CCharSkill.cpp
game/chars/CCharSpell.cpp
game/chars/CCharStat.cpp
game/chars/CCharStatus.cpp
game/chars/CCharUse.cpp
)
SOURCE_GROUP (game\\chars FILES ${chars_SRCS})

SET (clients_SRCS
game/clients/CAccount.cpp
game/clients/CAccount.h
game/clients/CChat.cpp
game/clients/CChat.h
game/clients/CChatChannel.cpp
game/clients/CChatChannel.h
game/clients/CChatChanMember.cpp
game/clients/CChatChanMember.h
game/clients/CClient.cpp
game/clients/CClientDialog.cpp
game/clients/CClientEvent.cpp
game/clients/CClientGMPage.cpp
game/clients/CClient.h
game/clients/CClientLog.cpp
game/clients/CClientMsg.cpp
game/clients/CClientTarg.cpp
game/clients/CClientTooltip.h
game/clients/CClientTooltip.cpp
game/clients/CClientUse.cpp
game/clients/CGMPage.cpp
game/clients/CGMPage.h
game/clients/CParty.cpp
game/clients/CParty.h
)
SOURCE_GROUP (game\\clients FILES ${clients_SRCS})

SET (uofiles_SRCS
game/uo_files/CUOMapList.h
game/uo_files/CUOMapList.cpp
game/uo_files/CUOItemInfo.h
game/uo_files/CUOItemInfo.cpp
game/uo_files/CUOHuesRec.h
game/uo_files/CUOHuesRec.cpp
game/uo_files/CUOIndexRec.h
game/uo_files/CUOIndexRec.cpp
game/uo_files/CUOItemTypeRec.h
game/uo_files/CUOMapBlock.h
game/uo_files/CUOMapMeter.h
game/uo_files/CUOMapMeter.cpp
game/uo_files/CUOMultiItemRec.h
game/uo_files/CUOMultiItemRec.cpp
game/uo_files/CUOStaticItemRec.h
game/uo_files/CUOStaticItemRec.cpp
game/uo_files/CUOTerrainInfo.h
game/uo_files/CUOTerrainInfo.cpp
game/uo_files/CUOTerrainTypeRec.h
game/uo_files/CUOVersionBlock.h
game/uo_files/CUOVersionBlock.cpp
game/uo_files/uofiles_enums.h
game/uo_files/uofiles_enums_itemid.h
game/uo_files/uofiles_enums_creid.h
game/uo_files/uofiles_macros.h
game/uo_files/uofiles_types.h
)
SOURCE_GROUP (game\\uo_files FILES ${uofiles_SRCS})

# Files containing 'background work'
SET (common_SRCS
common/CacheableScriptFile.cpp
common/CacheableScriptFile.h
common/CDataBase.cpp
common/CDataBase.h
common/CEncrypt.cpp
common/CEncrypt.h
common/CException.cpp
common/CException.h
common/CExpression.cpp
common/CExpression.h
common/CLog.cpp
common/CLog.h
common/CServerMap.cpp
common/CServerMap.h
common/CUID.cpp
common/CUID.h
common/CUIDExtra.h
common/CMD5.cpp
common/CMD5.h
common/CObjBaseTemplate.cpp
common/CObjBaseTemplate.h
common/common.cpp
common/common.h
common/CRect.cpp
common/CRect.h
common/CRegion.cpp
common/CRegion.h
common/CResourceBase.cpp
common/CResourceBase.h
common/CScript.cpp
common/CScript.h
common/CScriptObj.cpp
common/CScriptObj.h
common/CSectorTemplate.cpp
common/CSectorTemplate.h
common/CSocket.cpp
common/CSocket.h
common/CsvFile.cpp
common/CsvFile.h
common/CTextConsole.cpp
common/CTextConsole.h
common/CUOInstall.cpp
common/CUOInstall.h
common/CVarDefMap.cpp
common/CVarDefMap.h
common/CVarFloat.cpp
common/CVarFloat.h
common/datatypes.h
common/sphereproto.h
common/sphereversion.h
common/ListDefContMap.cpp
common/ListDefContMap.h
common/os_unix.h
common/os_windows.h
common/twofish/twofish2.cpp
common/regex/deelx.h
)
SOURCE_GROUP (common FILES ${common_SRCS})

# CrashDump files
SET (crashdump_SRCS
common/crashdump/crashdump.cpp
common/crashdump/crashdump.h
common/crashdump/mingwdbghelp.h
)
SOURCE_GROUP (common\\crashdump FILES ${crashdump_SRCS})

# LibEv files
SET (libev_SRCS
#common/libev/ev.c
#common/libev/ev.h
#common/libev/ev_config.h
#common/libev/ev_epoll.c
#common/libev/ev_kqueue.c
#common/libev/ev_poll.c
#common/libev/ev_port.c
#common/libev/ev_select.c
#common/libev/ev_vars.h
#common/libev/ev_win32.c
#common/libev/ev_wrap.h
#common/libev/ev++.h
#common/libev/event.c
#common/libev/event.h
common/libev/wrapper_ev.c
common/libev/wrapper_ev.h
)
SOURCE_GROUP (common\\libev FILES ${libev_SRCS})

# Sphere library files
SET (spherelibrary_SRCS
common/sphere_library/CSArray.cpp
common/sphere_library/CSArray.h
common/sphere_library/CSAssoc.cpp
common/sphere_library/CSAssoc.h
common/sphere_library/CSFile.cpp
common/sphere_library/CSFile.h
common/sphere_library/CSFileList.cpp
common/sphere_library/CSFileList.h
common/sphere_library/CSMemBlock.cpp
common/sphere_library/CSMemBlock.h
common/sphere_library/CSQueue.cpp
common/sphere_library/CSQueue.h
common/sphere_library/CSRand.cpp
common/sphere_library/CSRand.h
common/sphere_library/CSString.cpp
common/sphere_library/CSString.h
common/sphere_library/CSTime.cpp
common/sphere_library/CSTime.h
common/sphere_library/CSWindow.cpp
common/sphere_library/CSWindow.h
common/sphere_library/mutex.h
common/sphere_library/mutex.cpp
)
SOURCE_GROUP (common\\sphere_library FILES ${spherelibrary_SRCS})

# SQLite files
SET (sqlite_SRCS
common/sqlite/sqlite3.c
common/sqlite/sqlite3.h
common/sqlite/SQLite.cpp
common/sqlite/SQLite.h
)
SOURCE_GROUP (common\\sqlite FILES ${sqlite_SRCS})

# ZLib files
SET (zlib_SRCS
common/zlib/adler32.c
common/zlib/compress.c
common/zlib/crc32.c
common/zlib/crc32.h
common/zlib/deflate.c
common/zlib/deflate.h
common/zlib/gzclose.c
common/zlib/gzguts.h
common/zlib/gzlib.c
common/zlib/gzread.c
common/zlib/gzwrite.c
common/zlib/infback.c
common/zlib/inffast.c
common/zlib/inffast.h
common/zlib/inffixed.h
common/zlib/inflate.c
common/zlib/inflate.h
common/zlib/inftrees.c
common/zlib/inftrees.h
common/zlib/trees.c
common/zlib/trees.h
common/zlib/uncompr.c
common/zlib/zconf.h
common/zlib/zlib.h
common/zlib/zutil.c
common/zlib/zutil.h
)
SOURCE_GROUP (common\\zlib FILES ${zlib_SRCS})

# Network management files
SET (network_SRCS
network/network.cpp
network/network.h
network/packet.cpp
network/packet.h
network/receive.cpp
network/receive.h
network/send.cpp
network/send.h
network/PingServer.cpp
network/PingServer.h
)
SOURCE_GROUP (network FILES ${network_SRCS})

# Main program files: threads, console...
SET (sphere_SRCS
sphere/asyncdb.cpp
sphere/asyncdb.h
sphere/containers.h
sphere/linuxev.cpp
sphere/linuxev.h
sphere/ProfileData.cpp
sphere/ProfileData.h
sphere/ProfileTask.cpp
sphere/ProfileTask.h
sphere/strings.cpp
sphere/strings.h
sphere/threads.cpp
sphere/threads.h
sphere/ntservice.cpp
sphere/ntservice.h
sphere/ntwindow.cpp
sphere/UnixTerminal.cpp
sphere/UnixTerminal.h
sphere/SphereSvr.rc
)
SOURCE_GROUP (sphere FILES ${sphere_SRCS})

# Table definitions
SET (tables_SRCS
tables/CBaseBaseDef_props.tbl
tables/CChar_functions.tbl
tables/CChar_props.tbl
tables/CCharBase_props.tbl
tables/CCharNpc_props.tbl
tables/CCharPlayer_functions.tbl
tables/CCharPlayer_props.tbl
tables/CClient_functions.tbl
tables/CClient_props.tbl
tables/CSFile_functions.tbl
tables/CSFile_props.tbl
tables/CSFileObjContainer_functions.tbl
tables/CSFileObjContainer_props.tbl
tables/CItem_functions.tbl
tables/CItem_props.tbl
tables/CItemBase_props.tbl
tables/CItemStone_functions.tbl
tables/CItemStone_props.tbl
tables/classnames.tbl
tables/CObjBase_functions.tbl
tables/CObjBase_props.tbl
tables/CParty_functions.tbl
tables/CParty_props.tbl
tables/CScriptObj_functions.tbl
tables/CSector_functions.tbl
tables/CStoneMember_functions.tbl
tables/CStoneMember_props.tbl
tables/defmessages.tbl
tables/triggers.tbl
)
SOURCE_GROUP (tables FILES ${tables_SRCS})

# Misc doc and *.ini files
SET (docs_TEXT
../Changelog-56d-Nightlies.txt
sphere.ini
sphereCrypt.ini
)

INCLUDE_DIRECTORIES (
common/mysql/include/
)
