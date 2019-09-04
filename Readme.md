# Setup

```shell
mkdir build
cd build
cmake ..
make -j4
```

# Usage

./bin/GrEdPa {OPTIONS}

    GrEdPa: Implementations of two local search algorithms , LS-G and LS-F, on
    Balanced Edge Partition Problem

  OPTIONS:

      -h, --help                        Display this help menu
      -f[benchmark], --file=[benchmark] Path to benchmark
      -o[output file], --ouput=[output
      file]                             Path to output file
      -i[initial partition file],
      --initial=[initial partition
      file]                             Path to initial partition file
      -p[part number], --part=[part
      number]                           The number of parts you want to
                                        partition into
      -s[seed], --seed=[seed]           Seed due to the randomness
      -a[alpha], --alpha=[alpha]        The balance factor
      -l[Local search method],
      --local=[Local search method]     The local search method, supported
                                        options: MoveBlockG and MoveBlockF
      -t[Time limitation], --time=[Time
      limitation]                       The cut down time in second

# Format

## Benchmark

The first line of benchmark file should contain the number of vertices and the number of edges.
The remaining lines should contain the edge lists.
Note, the indices of vertices should be 1-based.

## Initial Partition File

The input contains $p$ parts.
For each part, the first line should contain the size of this part and the remaining lines should contain the edges lists of this part.
