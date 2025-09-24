# Benchmark Recipes

**Table of Contents**
* [General Setup](#general-setup)
* [Datasets Setup](#datasets-setup)
* [K-Means Mapreduce](#k-means-mapreduce)
* [Grep Mapreduce](#grep-mapreduce)
* [Federated Logistic Regression](#federated-logistic-regression)
* [Riot STATS](#riot-stats)

## General Setup

1. Go through [laminar VM setup](vm-setup-centos7.md) for setting up the VM the first time.
2. The recipes here assume that the woof path (CSPOT namespace location) being used is `/home/centos/laminar`. Please make required changes to the `set_host()` calls in the benchmark programs if some other location is used. Also modify the commands being used below ti reflect the same.
3. In `cspot-apps/dataflow_multiwoof/type_system/include/type_system/type_system/ts_types.h`, make sure that `#define NOPRIM` is commented out or undefined. If you make a change, remember to re-build cspot-apps and copy the handler binaries to the woof-path.
4. Consider using the [bashrc functions](bashrc-functions.md) if you are going to try multiple executions and builds.

## Datasets Setup

1. Request Chandra for access to the `wtb.cs.ucsb.edu` machine to fetch datasets. Have an ssh public/private key pair at the ready.
2. Inside the woof path (CSPOT namespace location), create a directory for datasets `cd ~/laminar && mkdir data_local`.
3. From your local machine, scp your private key (for accessing the datasets) to the `data_local` directory.
4. Instruction for downloading a dataset is in the respective benchmark's recipe.

NOTE: All datasets (original and subsets) can be viewed at `ubuntu@wtb.cs.ucsb.edu:/backup6/racelab/laminar_datasets/`.

## K-Means Mapreduce
Assuming cspot-apps is built and binaries are ready:
- `cd ~/laminar && cp ~/cspot-apps/build/bin/k_means_mapreduce .`

For a single run of the benchmark:
1. `sudo ./woofc-namespace-platform >& namespace.log &` (or run only woofc-namespace-platform in a separate terminal)
2. `sudo ./k_means_mapreduce [-m <number_of_mappers>] [-p <data_points_per_mapper>]` (The default is 10 mappers and 50000 points per mapper)
3. `sudo rm -f lmr*`
4. `sudo pkill woofc`

For running multiple configurations and getting stats, use the `~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/do-kmeans.sh` script.
1. Update the do-kmeans.sh script to contain only the required configurations. See `mappers=(...)` in the script.
2. If not already given, give execute permissions to the script.
3. `cd ~/laminar`
4. `sudo ~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/do-kmeans.sh`

## Grep Mapreduce
Fetch the dataset:
1. `cd ~/laminar/data_local`
2. `scp -i <private_key> -r centos@wtb.cs.ucsb.edu:/backup6/racelab/laminar_datasets/benchmarks/wikipedia .`

NOTE: The number of files to be searched is set at 400 by default. If you wish to just try the program, reduce the number `NUM_DATA_FILES` in `cspot-apps/dataflow_multiwoof/benchmarks/grep_mapreduce.cpp` for a faster execution.

Assuming cspot-apps is built and binaries are ready:
- `cd ~/laminar && cp ~/cspot-apps/build/bin/grep_mapreduce .`

For a single run of the benchmark:
1. `sudo ./woofc-namespace-platform >& namespace.log &` (or run only woofc-namespace-platform in a separate terminal)
2. `sudo ./grep_mapreduce <match_pattern> [-r <number_of_regex_mappers>] [-i <number_of_inverse_mappers>]` (The default is 8 regex mappers and 1 inverse mapper. For results, we used the match pattern: lam)
3. `sudo rm -f lmr*`
4. `sudo pkill woofc`

For running multiple configurations and getting stats, use the `~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/do-grep.sh` script.
1. Update the do-grep.sh script to contain only the required configurations. See `regex_mappers=(...)` and `inv_mappers=(...)` in the script.
2. If not already given, give execute permissions to the script.
3. `cd ~/laminar`
4. `sudo ~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/do-grep.sh`

### Distributing the grep benchmark
- The grep mapreduce benchmark can be distributed over 2 machines.
- There are 2 jobs in the pipeline: (1) word match, (2) frequency sort
- To run each job on a different machine, first uncomment the second `add_host()` call and change the IP's in both the `add_host()` calls to the private IP's of the 2 machines (or public IP's with traffic rules configured). Note that the same code should exist on both machines.
- For host 1, let the `set_host(1)` remain as is; for host 2, change it to `set_host(2)`. Also, change the `int curr_host` and `int job_2_host` to 2.
- Run the program on host 2, and then on host 1.

## Federated Logistic Regression
Assuming cspot-apps is built and binaries are ready:
- `cd ~/laminar && cp ~/cspot-apps/build/bin/logistic_regression_mapreduce .`

For a single run of the benchmark:
1. `sudo ./woofc-namespace-platform >& namespace.log &`
2. `sudo ./logistic_regression_mapreduce [-m <number_of_workers>] [-p <data_points_per_worker>]` (The default is 10 workers and 50000 points per worker)
3. `sudo rm -f lmr*`
4. `sudo pkill woofc`

For running multiple configurations and getting stats, use the `~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/do-regression.sh` script.
1. Update the do-kmeans.sh script to contain only the required configurations. See `mappers=(...)` in the script.
2. `cd ~/laminar`
3. `sudo ~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/do-regression.sh`

## RIoT STATS
1. `cd ~/laminar/data_local`
2. `scp -i <private_key> -r centos@wtb.cs.ucsb.edu:/backup6/racelab/laminar_datasets/benchmarks/mhealth .`

Assuming cspot-apps is built and binaries are ready:
- `cd ~/laminar && cp ~/cspot-apps/build/bin/riot_stats .`

Benchmark configuration
- The file `operation_system/src/operations/riot_benchmark/riot_config.h` contains the configuration for various nodes of the benchmark. Make sure the `DATA_PATH` is correctly set to the woof_path (CSPOT namespace) that you are using.

For a single run of the benchmark:
1. `sudo ./woofc-namespace-platform >& namespace.log &`
2. `sudo ./riot_stats [-m <messages_per_second>] [-d <duration_in_seconds>]` (The default is 500 messages per second for 10 seconds)
3. If your total messages (messages_per_second * duration_in_seconds) exceeds 20, run: `python3 ~/cspot-apps/dataflow_multiwoof/benchmarks/scripts/process_riot_stats.py` to get latency and throughput statistics.
4. `sudo rm -f lmr*`
5. `sudo pkill woofc`

NOTE: The program takes a long time to run the first time in a CSPOT run because it installs GNU-Plot in the container. Sometimes this can be VERY slow. If you do not kill CSPOT, and re-run riot_stats after cleaning the woofs, there won't be an installation overhead. To bypass this completely, comment out the 6 lines of code under the comment `// Install gnuplot in the CSPOT container`.

### Distributing the riot STATS benchmark
- Every node in the STATS pipeline can be put on a different node if day has such a vibe.
- The RIoTBench paper uses the following configuration:
    - An 8-core machine for controller (message injector) and the sink.
    - 3 4-core VMs utilizing a total of 10 cores for the remaining tasks. (exact distribution unknown)
- In our case, having the SenML Parse and Sink nodes running in the same CSPOT namespace (and container) gives the most accurate latency calculation since these nodes add the start and end timestamps. Throughput calculation is independent of the deployment.
- First, add all the hosts with `add_host()` as indicated in the program.
- Modify the `int hosts[]` array to allot a host to each node. Positions are marked with comments to indicate which node will use the host number at that position.
- Make sure to change the `set_host()` on each host manually before building the program.
- For every machine, check if the `DATA_PATH` in `riot_config.h` is pointing to the correct location. If any change is made here, remember to re-build cspot-apps.
- Run the program on all other hosts, and at the end, run it on host 1.
