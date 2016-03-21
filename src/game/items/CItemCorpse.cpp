
#include "../common/grayproto.h"
#include "../chars/CChar.h"
#include "../chars/CCharNPC.h"
#include "../CServTime.h"
#include "../CWorld.h"
#include "CItem.h"
#include "CItemCorpse.h"


CItemCorpse::CItemCorpse( ITEMID_TYPE id, CItemBase * pItemDef ) :
	CItemContainer( id, pItemDef )
{
	ADDTOCALLSTACK("CItemCorpse::CItemCorpse");
}

CItemCorpse::~CItemCorpse()
{
	DeletePrepare();	// Must remove early because virtuals will fail in child destructor.
}

CChar *CItemCorpse::IsCorpseSleeping() const
{
	ADDTOCALLSTACK("CItemCorpse::IsCorpseSleeping");
	// Is this corpse really a sleeping person ?
	// CItemCorpse
	if ( !IsType(IT_CORPSE) )
	{
		DEBUG_ERR(("Corpse (0%x) doesn't have type T_CORPSE! (it has %d)\n", (DWORD)GetUID(), GetType()));
		return NULL;
	}

	CChar *pCharCorpse = m_uidLink.CharFind();
	if ( pCharCorpse && pCharCorpse->IsStatFlag(STATF_Sleeping) && !GetTimeStamp().IsTimeValid() )
		return pCharCorpse;

	return NULL;
}

int CItemCorpse::GetWeight(WORD amount) const
{
	UNREFERENCED_PARAMETER(amount);
	// GetAmount is messed up.
	// true weight == container item + contents.
	return( 1 + CContainer::GetTotalWeight());
}


bool CChar::CheckCorpseCrime( const CItemCorpse *pCorpse, bool fLooting, bool fTest )
{
	ADDTOCALLSTACK("CChar::CheckCorpseCrime");
	// fLooting = looting as apposed to carving.
	// RETURN: true = criminal act !

	if ( !pCorpse || !g_Cfg.m_fLootingIsACrime )
		return false;

	CChar *pCharGhost = pCorpse->m_uidLink.CharFind();
	if ( !pCharGhost || pCharGhost == this )
		return false;

	if ( pCharGhost->Noto_GetFlag(this) == NOTO_GOOD )
	{
		if ( !fTest )
		{
			// Anyone saw me doing this?
			CheckCrimeSeen(SKILL_NONE, pCharGhost, pCorpse, fLooting ? g_Cfg.GetDefaultMsg(DEFMSG_LOOTING_CRIME) : NULL);
			Noto_Criminal();
		}
		return true;
	}
	return false;
}

CItemCorpse *CChar::FindMyCorpse( bool ignoreLOS, int iRadius ) const
{
	ADDTOCALLSTACK("CChar::FindMyCorpse");
	// If they are standing on their own corpse then res the corpse !
	CWorldSearch Area(GetTopPoint(), iRadius);
	for (;;)
	{
		CItem *pItem = Area.GetItem();
		if ( !pItem )
			break;
		if ( !pItem->IsType(IT_CORPSE) )
			continue;
		CItemCorpse *pCorpse = dynamic_cast<CItemCorpse*>(pItem);
		if ( !pCorpse || (pCorpse->m_uidLink != GetUID()) )
			continue;
		if ( pCorpse->m_itCorpse.m_BaseID != m_prev_id )	// not morphed type
			continue;
		if ( !ignoreLOS && !CanSeeLOS(pCorpse) )
			continue;
		return pCorpse;
	}
	return NULL;
}

// Create the char corpse when i die (STATF_DEAD) or fall asleep (STATF_Sleeping)
// Summoned (STATF_Conjured) and some others creatures have no corpse.
CItemCorpse * CChar::MakeCorpse( bool fFrontFall )
{
	ADDTOCALLSTACK("CChar::MakeCorpse");

	WORD wFlags = static_cast<WORD>(m_TagDefs.GetKeyNum("DEATHFLAGS", true));
	if (wFlags & DEATH_NOCORPSE)
		return( NULL );
	if (IsStatFlag(STATF_Conjured) && !(wFlags & (DEATH_NOCONJUREDEFFECT|DEATH_HASCORPSE)))
	{
		Effect(EFFECT_XYZ, ITEMID_FX_SPELL_FAIL, this, 1, 30);
		return( NULL );
	}

	CItemCorpse *pCorpse = dynamic_cast<CItemCorpse *>(CItem::CreateScript(ITEMID_CORPSE, this));
	if (pCorpse == NULL)	// weird internal error
		return( NULL );

	TCHAR *pszMsg = Str_GetTemp();
	sprintf(pszMsg, g_Cfg.GetDefaultMsg(DEFMSG_MSG_CORPSE_OF), GetName());
	pCorpse->SetName(pszMsg);
	pCorpse->SetHue(GetHue());
	pCorpse->SetCorpseType(GetDispID());
	pCorpse->SetAttr(ATTR_MOVE_NEVER);
	pCorpse->m_itCorpse.m_BaseID = m_prev_id;	// id the corpse type here !
	pCorpse->m_itCorpse.m_facing_dir = m_dirFace;
	pCorpse->m_uidLink = GetUID();

	// TO-DO: Fix corpses always turning to the same dir (DIR_N) after resend it to clients

	if (fFrontFall)
		pCorpse->m_itCorpse.m_facing_dir = static_cast<DIR_TYPE>(m_dirFace|0x80);

	int iDecayTimer = -1;	// never decay
	if (IsStatFlag(STATF_DEAD))
	{
		iDecayTimer = (m_pPlayer) ? g_Cfg.m_iDecay_CorpsePlayer : g_Cfg.m_iDecay_CorpseNPC;
		pCorpse->SetTimeStamp(CServTime::GetCurrentTime().GetTimeRaw());	// death time
		if (Attacker_GetLast())
			pCorpse->m_itCorpse.m_uidKiller = Attacker_GetLast()->GetUID();
		else
			pCorpse->m_itCorpse.m_uidKiller.InitUID();
	}
	else	// sleeping (not dead)
	{
		pCorpse->SetTimeStamp(0);
		pCorpse->m_itCorpse.m_uidKiller = GetUID();
	}

	if ((m_pNPC && m_pNPC->m_bonded) || IsStatFlag(STATF_Conjured|STATF_Sleeping))
		pCorpse->m_itCorpse.m_carved = 1;	// corpse of bonded and summoned creatures (or sleeping players) can't be carved

	if ( !(wFlags & DEATH_NOLOOTDROP) )		// move non-newbie contents of the pack to corpse
		DropAll( pCorpse );

	pCorpse->SetKeyNum("OVERRIDE.MAXWEIGHT", g_Cfg.Calc_MaxCarryWeight(this) / 10);		// set corpse maxweight to prevent weird exploits like when someone place many items on an player corpse just to make this player get stuck on resurrect
	pCorpse->MoveToDecay(GetTopPoint(), iDecayTimer);
	return( pCorpse );
}

// We are creating a char from the current char and the corpse.
// Move the items from the corpse back onto us.
bool CChar::RaiseCorpse( CItemCorpse * pCorpse )
{
	ADDTOCALLSTACK("CChar::RaiseCorpse");

	if ( !pCorpse )
		return false;

	if ( pCorpse->GetCount() > 0 )
	{
		CItemContainer *pPack = GetPackSafe();
		CItem *pItemNext = NULL;
		for ( CItem *pItem = pCorpse->GetContentHead(); pItem != NULL; pItem = pItemNext )
		{
			pItemNext = pItem->GetNext();
			if ( pItem->IsType(IT_HAIR) || pItem->IsType(IT_BEARD) )	// hair on corpse was copied!
				continue;

			if ( pItem->GetContainedLayer() )
				ItemEquip(pItem);
			else if ( pPack )
				pPack->ContentAdd(pItem);
		}

		pCorpse->ContentsDump( GetTopPoint());		// drop left items on ground
	}

	UpdateAnimate((pCorpse->m_itCorpse.m_facing_dir & 0x80) ? ANIM_DIE_FORWARD : ANIM_DIE_BACK, true, true);
	pCorpse->Delete();
	return true;
}