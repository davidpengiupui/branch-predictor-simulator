# branch-predictor-simulator
Gshare, bimodal and hybrid branch predictor simulator in C++

For this project, a branch predictor simulator is implemented. simulator can simulate various types of
branch predictor and configure various cache parameters like number of index bits for the BTB, number
of bits for the global branch history register, size of BTB, and set associativity for the BTB. The branch
predictors implemented are bimodal, gshare, hybrid enhancing these with a BTB. The replacement policy
for all is kept as LRU for simplicity. The miss rates for each of the predictor type is tabulated and is
studied for different BTB sizes. For each predictor careful analysis is done and the predictor size with the
best peformance is selected keeping in mind the cost incurred in terms of bits to maintain a particular
BTB size. The BTB size is fixed at 16 KB for calculations where the prediction is based on a set
associative tag array.
