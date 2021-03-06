/**
* @file CItemSpawn.h
*
*/

#pragma once
#ifndef _INC_CITEMSPAWN_H
#define _INC_CITEMSPAWN_H

#include "CItem.h"


class CCharBase;

class CItemSpawn : public CItem
{
private:
	static lpctstr const sm_szLoadKeys[];
	static lpctstr const sm_szVerbKeys[];
	CUID m_obj[UINT8_MAX];	///< Storing UIDs of the created items/chars.

public:
	static const char *m_sClassName;

	/* I don't want to inherit SetAmount, GetAmount and m_iAmount from the parent CItem class. I need to redefine them for CItemSpawn class
	*	so that when i set AMOUNT to the spawn item, i don't really set the "item amount/quantity" property, but the "spawn item AMOUNT" property.
	*	This way, even if there is a stackable spawn item (default in Enhanced Client), i won't increase the item stack quantity and i can't pick
	*	from pile the spawn item. Plus, since the max amount of spawnable objects per single spawn item is the max size of a byte, we can change
	*	the data type accepted/returned.
	*/
	void SetAmount(byte iAmount);
	byte GetAmount();
	byte m_iAmount;				// Amount of objects to spawn.
	byte m_currentSpawned;		// Amount of current objects already spawned. Get it from scripts via COUNT property (read-only).

								/**
								* @brief Overrides onTick for this class.
								*
								* Setting time again
								* stoping if more2 >= amount
								* more1 Resource Check
								* resource (item/char) generation
								*/
	void OnTick( bool fExec );

	/**
	* @brief Removes everything created by this spawn, if still belongs to the spawn.
	*/
	void KillChildren();

	/**
	* @brief Setting display ID based on Character's Figurine or the default display ID if this is an IT_SPAWN_ITEM.
	*/
	CCharBase * SetTrackID();

	/**
	* @brief Generate a *pDef item from this spawn.
	*
	* @param pDef resource to create
	*/
	void GenerateItem( CResourceDef * pDef );

	/**
	* @brief Generate a *pDef char from this spawn.
	*
	* @param pDef resource to create
	*/
	void GenerateChar( CResourceDef * pDef );

	/**
	* @brief Gets the total count of items or chars created which are still considered as 'spawned' by this spawn.
	*
	* @return the count of items/chars.
	*/
	byte GetCount();

	/**
	* @brief Removing one UID in Spawn's m_obj[].
	*
	* @param UID of the obj to remove.
	*/
	void DelObj( CUID uid );

	/**
	* @brief Storing one UID in Spawn's m_obj[].
	*
	* @param UID of the obj to add.
	*/
	void AddObj( CUID uid );

	/**
	* @brief Test if the character from more1 exists.
	*
	* @param ID of the char to check.
	*/
	inline CCharBase * TryChar( CREID_TYPE &id );

	/**
	* @brief Test if the item from more1 exists.
	*
	* @param ID of the item to check.
	*/
	inline CItemBase * TryItem( ITEMID_TYPE &id );

	/**
	* @brief Get a proper RESOURCE_ID from the id provided.
	*
	* @return a valid RESOURCE_ID.
	*/
	CResourceDef * FixDef();

	/**
	* @brief Gets the name of the resource created (item or char).
	*
	* @return the name of the resource.
	*/
	int GetName(tchar * pszOut) const;

	CItemSpawn(ITEMID_TYPE id , CItemBase * pItemDef);
	virtual ~CItemSpawn();
	virtual bool r_WriteVal(lpctstr pszKey, CSString & sVal, CTextConsole * pSrc);
	virtual bool  r_LoadVal(CScript & s);
	virtual void  r_Write(CScript & s);
};

#endif // _INC_CITEMSPAWN_H
