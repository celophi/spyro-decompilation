#include <PSYQ/LIBAPI.h>
#include <symbols.h>

/// @brief Initializes the memory cards.
/// @note
///     - Address: 0x80012460
///     - Hook: InitializeMemoryCards.s
///     - Test: InitializeMemoryCardsTest.c
///     - Test Status: Broken (Problem with looping initialization?)
void InitializeMemoryCards()
{
    _InitializeMemoryCards_u0 = 0;
    _InitializeMemoryCards_u1 = 0;

    InitCARD(0);
    StartCARD();
    _bu_init();
}