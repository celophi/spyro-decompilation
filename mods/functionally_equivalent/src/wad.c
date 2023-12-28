#include <wad.h>

WA4S3_Head * WA4S3_MakeAbsoluteAddresses(WA4S3_Head *section)
{
    int modelCount = section->Count;
    if (modelCount < 0) 
    {
        section->FF_1 += (int)section;
        section->FF_3 += (int)section;
        section->FF_2 += (int)section;

        // Prevent negative addressing?
        section = (WA4S3_Head *)((uint)section & 0x7FFFFFFF);
        return section;
    }
  
    for (int i = 0; i < 8; i++)
    {
        if (section->Padding[i] != 0) 
        {
            section->Padding[i] += ((uint)section & 0x7FFFFFFF);
        }
    }

    section->Offset += (int)section;

    if (modelCount <= 0)
    {
        return section;
    }

    int *ref = &section->ModelOffsets;

    for (int i = 0; i < modelCount; i++)
    {
        if (ref[i] == -1) 
        {
            continue;
        }
        
        WA4S3_Metadata* metadata = (WA4S3_Metadata*)((int)section + ref[i]);
        ref[i] = (int)metadata;

        metadata->U13 += section->Offset;
        metadata->U14 += section->Offset;

        if (metadata->U15 != 0) 
        {
            metadata->U15 += section->Offset;
            metadata->U16 += section->Offset;
        }

        // Check option - if true, then double entries; otherwise single
        if (metadata->Option == 0) 
        {
            WA4S3_TablePair *tableEntryPtr = (WA4S3_TablePair *)((byte*)metadata + sizeof(WA4S3_Metadata));

            for (ushort loop = 0; loop < metadata->SubCount; loop++)
            {
                tableEntryPtr->First += (section->Offset & 0x1FFFFFU);
                uint *second = &tableEntryPtr->Second;
                
                if ((*second & 0xFFFF) != 0) 
                {
                    *second += ((section->Offset - (int)second) + 4 >> 2 & 0xFFFFU);
                }

                tableEntryPtr += 1;
            }
        }
        else 
        {
            metadata->U12 += section->Offset;
            uint *tableEntry2 = (uint *)((byte*)metadata + sizeof(WA4S3_Metadata));

            for (ushort loop = 0; loop < metadata->SubCount; loop++)
            {
                uint first = *tableEntry2;

                // high bits represent one uint.
                uint effective = first & 0xFFE00000;

                *tableEntry2 = effective + ((int)((first & 0x1FFFFF) + section->Offset) >> 1 & 0x1FFFFFU);
                tableEntry2 += 1;
            }
        }
    }

    return section;
}