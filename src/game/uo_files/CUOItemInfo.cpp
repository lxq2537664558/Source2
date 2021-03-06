#include "../../common/CException.h"
#include "../../common/CUOInstall.h"
#include "../../common/CLog.h"
#include "CUOItemInfo.h"
#include "CUOIndexRec.h"
#include "CUOItemTypeRec.h"
#include "CUOTerrainTypeRec.h"
#include "uofiles_macros.h"

CUOItemInfo::CUOItemInfo( ITEMID_TYPE id )
{
    if ( id >= ITEMID_MULTI )
    {
        // composite/multi type objects.
        m_flags = 0;
        m_weight = 0xFF;
        m_layer = LAYER_NONE;
        m_dwAnim = 0;
        m_height = 0;
        strcpy( m_name, ( id <= ITEMID_SHIP6_W ) ? "ship" : "structure" );
        return;
    }

    VERFILE_TYPE filedata;
    dword offset;
    CUOIndexRec Index;
    VERFILE_FORMAT format;
    if ( g_VerData.FindVerDataBlock( VERFILE_TILEDATA, (id + TERRAIN_QTY) / UOTILE_BLOCK_QTY, Index ) )
    {
        filedata = VERFILE_VERDATA;
        format = VERFORMAT_ORIGINAL;
        offset = Index.GetFileOffset() + 4 + (sizeof(CUOItemTypeRec) * (id % UOTILE_BLOCK_QTY));
        ASSERT( Index.GetBlockLength() >= sizeof(CUOItemTypeRec) );
    }
    else
    {
        filedata = VERFILE_TILEDATA;
        format = g_Install.GetMulFormat(filedata);

        switch (format)
        {
            case VERFORMAT_HIGHSEAS: // high seas format (CUOItemTypeRec_HS)
                offset = UOTILE_TERRAIN_SIZE2 + 4 + (( id / UOTILE_BLOCK_QTY ) * 4 ) + ( id * sizeof(CUOItemTypeRec_HS));
                break;

            case VERFORMAT_ORIGINAL: // original format (CUOItemTypeRec)
            default:
                offset = UOTILE_TERRAIN_SIZE + 4 + (( id / UOTILE_BLOCK_QTY ) * 4 ) + ( id * sizeof(CUOItemTypeRec));
                break;
        }
    }

    if ( g_Install.m_File[filedata].Seek( offset, SEEK_SET ) != offset )
    {
        throw CSError(LOGL_CRIT, CSFile::GetLastError(), "CTileItemType.ReadInfo: TileData Seek");
    }

    switch (format)
    {
        case VERFORMAT_HIGHSEAS: // high seas format (CUOItemTypeRec_HS)
            if ( g_Install.m_File[filedata].Read( static_cast <CUOItemTypeRec_HS *>(this), sizeof(CUOItemTypeRec_HS)) <= 0 )
                throw CSError(LOGL_CRIT, CSFile::GetLastError(), "CTileItemType.ReadInfo: TileData Read");
            break;

        case VERFORMAT_ORIGINAL: // old format (CUOItemTypeRec)
        default:
        {
            CUOItemTypeRec record;
            if ( g_Install.m_File[filedata].Read( static_cast <CUOItemTypeRec *>(&record), sizeof(CUOItemTypeRec)) <= 0 )
                throw CSError(LOGL_CRIT, CSFile::GetLastError(), "CTileTerrainType.ReadInfo: TileData Read");

            m_flags = record.m_flags;
            m_weight = record.m_weight;
            m_layer = record.m_layer;
            m_dwAnim = record.m_dwAnim;
            m_height = record.m_height;
            m_wUnk19 = record.m_wUnk14;
            m_dwUnk11 = record.m_dwUnk6;
            m_dwUnk5 = 0;
            strcpylen(m_name, record.m_name, CountOf(m_name));
            break;
        }
    }
}

ITEMID_TYPE CUOItemInfo::GetMaxTileDataItem()
{
    ADDTOCALLSTACK("CUOItemInfo::GetMaxTileDataItem");

    CSFile* pTileData = g_Install.GetMulFile(VERFILE_TILEDATA);
    ASSERT(pTileData != NULL);

    dword dwLength = (dword)pTileData->GetLength();	// length of file
    dword dwEntrySize = 0;							// size of tiledata entry
    dword dwOffset = 0;								// offset to tiledata items

    VERFILE_FORMAT format = g_Install.GetMulFormat(VERFILE_TILEDATA);
    switch (format)
    {
        case VERFORMAT_HIGHSEAS: // high seas format (CUOItemTypeRec_HS)
            dwEntrySize = sizeof(CUOItemTypeRec_HS);
            dwOffset = UOTILE_TERRAIN_SIZE2 + 4;
            break;

        case VERFORMAT_ORIGINAL: // original format (CUOItemTypeRec)
        default:
            dwEntrySize = sizeof(CUOItemTypeRec);
            dwOffset = UOTILE_TERRAIN_SIZE + 4;
            break;
    }

    ASSERT(dwEntrySize > 0);
    ASSERT(dwOffset < dwLength);

    // items are sorted in blocks of 32 with 4 byte padding between, so determine how
    // many blocks will fit in the file to find how many items there could be
    dwLength -= dwOffset;
    dword dwBlocks = (dwLength / ((UOTILE_BLOCK_QTY * dwEntrySize) + 4)) + 1;
    return static_cast<ITEMID_TYPE>(dwBlocks * UOTILE_BLOCK_QTY);
}
