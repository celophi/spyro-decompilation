#include <wad.h>
#include <symbols.h>
#include <draw_stuff.h>

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

/// @brief Creates absolute addresses for an array of DWORDs.
/// @param address Base address to apply to every value.
/// @param count Number of DWORDs in the table.
static void MakeAbsoluteAddresses(WA4S2_T1* address, int count)
{
    int* cursor = &address->TableStart;
    for (int i = 0; i < count; i++)
    {
        *cursor = (int)address + *cursor + 4;
        cursor++;
    }
}

/// @brief Creates absolute addresses for an array of DWORDs.
/// @param address Base address to apply to every value.
/// @param count Number of DWORDs in the table.
static void MakeAbsoluteAddresses2(WA4S2_T2* address, int count)
{
    int* cursor = &address->TableStart;
    for (int i = 0; i < count; i++)
    {
        *cursor = (int)address + *cursor + 4;
        cursor++;
    }
}

static int* HandleOptionA(int* offset)
{
    if (*offset < 5) 
        {
            _ReadWA3S2_u5 = 0;
            _ReadWA3S2_u6 = 0;
            _ReadWA3S2_u7 = 0;
        }
        else 
        {
            _ReadWA3S2_u5 = offset[2];
            int* w04 = offset + 3;
            
            _ReadWA3S2_u6 = (int)w04;

            for (int i = 0; i < _ReadWA3S2_u5; i++)
            {
                *w04 = (int)offset + *w04 + 4;
                w04++;
            }

            w04 = (int *)((int)offset + offset[1] + 4);
            _ReadWA3S2_u8 = *w04;
            w04 = w04 + 1;
            _ReadWA3S2_u7 = (int)w04;

            for (int i = 0; i < _ReadWA3S2_u8; i++)
            {
                *w04 = (int)offset + *w04 + 4 + offset[1];
                w04++;
            }
        }

        offset = (int *)((int)offset + *offset);

        _ReadWA3S2_u9 = offset[1];
        int* w04 = offset + 2;
        _ReadWA3S2_u10 = (int)w04;

        for (int i = 0; i < _ReadWA3S2_u9; i++)
        {
            *w04 = (int)offset + *w04 + 4;
            w04++;
        }

        offset = (int *)((int)offset + *offset);
         w04 = offset + 1;
        _ReadWA3S2_u11 = w04;
        offset[3] = (int)w04 + offset[3];
        _ReadWA3S2_u11[3] = (int)w04 + _ReadWA3S2_u11[3];
        _ReadWA3S2_u11[4] = (int)w04 + _ReadWA3S2_u11[4];
        _ReadWA3S2_u11[5] = (int)w04 + _ReadWA3S2_u11[5];
        _ReadWA3S2_u11[6] = (int)w04 + _ReadWA3S2_u11[6];
        offset = (int *)((int)offset + *offset);

        return offset;
}

static int* HandleOptionB(int* offset)
{
    int **w05;

        _ReadWA3S2_u14 = *offset;
        int **w06 = (int **)(offset + 1);
        int loop = 0;

        if (0 < _ReadWA3S2_u14) 
        {
            offset = &_ReadWA3S2_u15;
            do 
            {
                *offset = (int)w06;
                w05 = w06 + (int)w06[1] * 3 + 8;
                *w06 = (int *)w05;
                w06 = w05 + 5;
                *(int **)(*(int *)*offset + 16) = w05[6];
                **(int ***)*offset = w05[7];
                w05 = w05 + 8;
                *(int ***)(*(int *)*offset + 4) = w05;
                int w01 = 0;

                if (0 < **(int **)*offset) 
                {
                    do {
                        *w05 = (int *)((int)w06 + (int)(*w05 + 1));
                        w01 = w01 + 1;
                        w05 = w05 + 1;
                    } while (w01 < **(int **)*offset);
                }

                offset = (int *)((int **)offset + 1);
                loop = loop + 1;
                w06 = (int **)((int)w06 + (int)*w06);
            } while (loop < _ReadWA3S2_u14);
        }

        w05 = w06 + 2;
        for (offset = w06[1]; 0 < (int)offset; offset = (int *)((int)offset + -1)) 
        {
            (&_ReadWA3S2_u16)[*(short *)w05] = (int)w05;
            w05 = (int **)((int)w05 + *(short *)((int)w05 + 2) + 4);
        }

        offset = (int *)((int)w06 + (int)*w06);
        _ReadWA3S2_u17 = *offset;
        OG_WA4S2_Loop((WA4S2_U0 *)(offset + 1),1);
        offset = (int *)((int)offset + *offset);

        return offset;
}

int * ReadWA4S2(WA4S2 *wa4s2, int option)
{
    // [4] => 26
    _ReadWA4S2_Count = wa4s2->count;

    // &[8] => 20 40 3D 10, this must be some entry starting point.
    _ReadWA4S2_U1_Addr = &wa4s2->u1;
                    
    // &[424] => 416 + 8 (for the u1 starting point)
    _ReadWA4S2_EndU = _ReadWA4S2_U1_Addr + _ReadWA4S2_Count * 4;


                    /* WA3S2->Offset // just add offset to beginning of WA3S2 */
    int* offset = (int *)((int)&wa4s2->offset + wa4s2->offset);

    WA4S2_T1* t1 = (WA4S2_T1*)offset;

    _ReadWA4S2_counter = t1->Count;
    _WA4S2_Table1 = &t1->TableStart;

     /* Create absolute addresses. Apply a base address in RAM like 0x8xxxxxx to the value looped over. Replace the data with the result. */
    
    MakeAbsoluteAddresses(t1, _ReadWA4S2_counter);

                    /* Now deal with data[0x04]. The way this works, the offset is applied to the position of where it is defined in the WAD: &offset2 + offset2. */
                    /* First load this is false */
    offset = (int *)((int)offset + *offset);

    // skipped on first load.
    if (option == 0) 
    {
        offset = HandleOptionA(offset);
    }

    WA4S2_T2* table2 = (WA4S2_T2*)offset;

    // Assign drawing environment RGB
    _WadDrawEnvRgb.R = table2->RGB.R;
    _WadDrawEnvRgb.G = table2->RGB.G;
    _WadDrawEnvRgb.B = table2->RGB.B;

    _WA4S2_Table2 = &table2->TableStart;
    _WA4S2_Table2Count = table2->Count; // 62

    /* Build 2nd absolute reference table. */
    MakeAbsoluteAddresses2(table2, _WA4S2_Table2Count);
    

    offset = (int *)((int)offset + *offset);

    
    // skipped on first load.
    if (option == 0) 
    {
        offset = HandleOptionB(offset);
    }

    return offset;
}
