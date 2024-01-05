# Integration Testing

## Reasoning
Asserting equivalent functionality can be an alternative to a matching assembly decompilation, and in many cases, can be more efficient. A strategy that just focuses on equivalent functionality allows the researcher to spend more time understanding and writing new code rather than searching for permutations to achieve a less than ideal assembly instruction sequence.

## Strategies
In most cases, tests should be written to match RAM, scratchpad, and GTE contexts. However, there are some functions where I have found that this isn't possible yet - for example dealing with interrupt code. For those situations, you can try to write some mocking functionality or just assert what select areas of memory are the same by deep cloning objects. This isn't really ideal because of the work involved, but it is worth it if it means verifying a correct decompilation.