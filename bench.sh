#!/bin/bash

for CPUS in 30 31 32
do
    g++ -std=c++17 -Wall -Wextra -pthread -Iinclude -DHEAVY -DCPUS=$CPUS -O3 src/main.cpp -o build/bench-heavy-ts
    g++ -std=c++17 -Wall -Wextra -pthread -Iinclude -DHEAVY -DTTS -DCPUS=$CPUS -O3 src/main.cpp -o build/bench-heavy-tts
    g++ -std=c++17 -Wall -Wextra -pthread -Iinclude -DHEAVY -DTTS -DTTS_PAD -DCPUS=$CPUS -O3 src/main.cpp -o build/bench-heavy-tts-pad
    g++ -std=c++17 -Wall -Wextra -pthread -Iinclude -DCPUS=$CPUS -O3 src/main.cpp -o build/bench-short-ts
    g++ -std=c++17 -Wall -Wextra -pthread -Iinclude -DTTS -DCPUS=$CPUS -O3 src/main.cpp -o build/bench-short-tts
    g++ -std=c++17 -Wall -Wextra -pthread -Iinclude -DTTS -DTTS_PAD -DCPUS=$CPUS -O3 src/main.cpp -o build/bench-short-tts-pad

    for ITER in {1..100}
    do
        ./build/bench-heavy-ts >> "benches/heavy/ts/${CPUS}_bench.txt"
        ./build/bench-heavy-tts >> "benches/heavy/tts/${CPUS}_bench.txt"
        ./build/bench-heavy-tts-pad >> "benches/heavy/tts_pad/${CPUS}_bench.txt"
        ./build/bench-short-ts >> "benches/short/ts/${CPUS}_bench.txt"
        ./build/bench-short-tts >>  "benches/short/tts/${CPUS}_bench.txt"
        ./build/bench-short-tts-pad >> "benches/short/tts_pad/${CPUS}_bench.txt"
    done
done
