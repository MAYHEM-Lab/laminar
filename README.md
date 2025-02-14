# Laminar -- Log-based Dataflow

Laminar is a simple API for implementing dataflow programs that use append-only logs and triggered computations as their only underlying storage abstraction.  At present, Laminar uses [CSPOT](https://github.com/MAYHEM-Lab/cspot) to implement this functionality.

## Installing Laminar

First, download and install CSPOT.  Ubuntu 20.04 is the currently preferred distribution.  CSPOT includes an installation script for this distro.  Note that the installation script installs Docker and requires sudo permissions to install the CSPOT libraries and to run its self-tests using Docker.

Next, issue the following commands in the top level Laminar directory:

```
	mkdir build
	cd build
	cmake ..
	make
```
The build will create a number of Laminar test programs and benchmarks and put
them in laminar/build/bin.  

## Testing a Laminar Installation

The current implementation of Laminar depends on
[CSPOT](https://github.com/MAYHEM-Lab/cspot).  The following 
example demonstrates how to test a Laminar installation 
that has the source code for Laminar (and the source code for CSPOT)
located the ubuntu user's home directory.  That is

```
/home/ubuntu/laminar
/home/ubuntu/cspot
```

contain the source tree for Laminar and CSPOT respectively.

Also, this example uses the IP addresses 
```
192.168.10.2
192.168.10.3
```
as the host IP addresses for two test hosts.  To test Laminar, replace these
IP addresses with the IP addresses on which Laminar and CSPOT are installed.

Finally, this example assumes that Laminar has built 
correctly, and that CSPOT has been built and installed correctly.
Also, for two-machine testing, that ports 50000 to 60000 are open for
bidirectional TCP connectivity between the two machines.

### Step 1 -- copy CSPOT service code to Laminar namespace

This testing example uses
```
/home/ubuntu/laminar/build/bin
```
as a Laminar namespace.  Laminar and CSPOT are statically linked so that the
contents of the build/bin directory can be copied to any directory on any
machine running a compatible kernel.  The fully qualified pathname to that
directory is the Laminar namespace.

To use the build/bin directory as the Laminar namespace execute

```
cp /home/ubuntu/cspot/build/bin/woofc* /home/ubuntu/laminar/build/bin
```

### Step 2 -- Start the CSPOT service for the Laminar namespace

To start the CSPOT service for the Laminar namespace type

```
cd /home/laminar/build/bin
./woofc-namespace-platform -b spawn >& namepsace.log &
```
Make sure that the platform started correct (i.e. there was not a permissions
problem).  The command

```ps auxww | grep woofc | grep -v grep
```

The output should show a woofc-namespace-platform process, a woofc-container
process, and a number of woofc-forker-helper processes.  The absolute path for
woofc-container and the woofc-forker-helper processes should be the same as
the namespace path (/home/ubuntu/laminar/build/bin in this example).

### Step 3 -- Configure the Laminar simple\_laminar\_example application

To test single machine functionality, edit the source file
simple\_laminar\_example.cpp located in
```
/home/ubuntu/tests/simple_laminar_example.cpp
```
and change line 19 (which is an add\_host() directive to
be
```
add_host(1, "192.168.10.2", "/home/ubuntu/laminar/build/bin/");
```
substituting the IP address for the host on which this test is being executed
for 192.168.10.2.  Note that the trailing "/" in the namespace path
"/home/ubuntu/laminar/build/bin/" must be present.
Also, "localhost" or "127.0.0.1" can be used as the IP address, but using
localhost typically does not test local port connectivity.

Rebuild the simple\_laminar\_example test program.
```
cd /home/laminar/build
make
```
Check the output of make to ensure that simple\_laminar\_example built correctly.

### Step 4 -- Run the single machine test

To run the test application type
```
cd /home/ubuntu/laminar/build/bin
./simple_laminar_example
```

The output should look like
```
./simple_laminar_example
WooFOpen: couldn't open woof: /home/ubuntu/laminar/build/bin/lmr.SETUP_ST_WF_TYPE
digraph G {
	node [shape="record", style="rounded"];
	subgraph cluster_1 { 
		label="Subgraph #1";
		node_1_1 [label="{{<0> 0|<1> 1}|<out>[ARITHMETIC :ADDITION]\nNode #1}"];
		node_1_2 [label="{{<0> 0|<1> 1}|<out>[ARITHMETIC :MULTIPLICATION]\nNode #2}"];
		node_1_3 [label="{<out>[INTERNAL :OPERAND]\nNode #3}"];
		node_1_4 [label="{<out>[INTERNAL :OPERAND]\nNode #4}"];
		node_1_5 [label="{<out>[INTERNAL :OPERAND]\nNode #5}"];
	}
	node_1_3:out -> node_1_1:0;
	node_1_4:out -> node_1_1:1;
	node_1_1:out -> node_1_2:0;
	node_1_5:out -> node_1_2:1;
}
fire_operand: output_woof: lmr-1.OUT_WF_TYPE.3, value: 1.000000
fire_operand: output_woof: lmr-1.OUT_WF_TYPE.4, value: 2.000000
fire_operand: output_woof: lmr-1.OUT_WF_TYPE.5, value: 3.000000
Result: 9
```

The "WooFOpen" warning is normal when this application has not been run
before in this namespace.  The digraph output can be input to
[DOT](https://graphviz.org/download/)
to generate a graphical dataflow representation of the program.   The last three lines
show the inputs and result from the calculation (1+2)*3.  

### Testing Laminar between two hosts

The following example assumes that Laminar and CSPOT have been successfully
installed on the two hosts 192.168.10.2 and 192.168.10.3 and that ports 50000
to 60000 are open for bidirectional TCP connectivity between the hosts.
Substitute the host IP addresses of the test hosts for these example IP
addresses.

This example also assume that the single machine test (described above) is
successful on host 192.168.10.2.

### Configure the distribute\_laminar\_example application on both hosts

On host 192.169.10.2 edit the file
```
/home/ubuntu/laminar/tests/distributed_simple_laminar_example.cpp
```
and change line 23 which should be "int curr\_host\_id = 2;"
to be
```
int curr_host_id = 1;
```
This change will make 192.168.10.2 HOST1 in the application's deployment.  The
change lines 25 and 26 (both of which should be add_host() directives) to be
```
add_host(1, "192.168.10.2", "/home/laminar/build/bin/");
add_host(2, "192.168.10.3", "/home/laminar/build/bin/");
```
Again, the trailing "/" must be present in both namespace paths.

Rebuild the application on 192.168.10.2.
```
cd /home/ubuntu/laminar/build
make
```

On 192.169.10.3 repeat this same process but set (or leave as set)
```
int curr_host_id = 2;
```
which sets HOST2 to be 192.168.10.3 in the application's deployment.
That is, the source code in distributed\_simple\_laminar\_example.cpp
should be identical except for line 23 should initialize curr\_host\_id to be
1 on 192.168.10.2 and to be 2 on 192.168.10.3.

Again, rebuild the application after editing the source code on 192.168.10.3.
```
cd /home/ubuntu/laminar/build
make
```

### Test the two host deployment

First ensure that the CSPOT services are running for the
/home/ubuntu/laminar/build/bin namespaces on both 192.168.10.2 and
192.168.10.3.  If they are not, start them as described above.

Next, ensure that the Laminar state from any previous test attempts (including
the single machine test described above) has been
removed.  On both 192.168.10.2 and 192.168.10.3 type
```
cd /home/ubuntu/laminar/build/bin
rm -f lmr*
```

On 192.168.10.3 (HOST2) type
```
cd /home/ubuntu/laminar/build/bin
./distributed_simple_laminar_example
```
The output should be
```
WooFOpen: couldn't open woof: /sharedfs/laminar-namespace/lmr.SETUP_ST_WF_TYPE
```
and the application should be waiting (the shell command prompt should not
return).  Then on 192.169.10.2 
run the same command sequence.

The output on 192.168.10.2 should be
```
WooFOpen: couldn't open woof: /home/ubuntu/laminar/build/bin/lmr.SETUP_ST_WF_TYPE
fire_operand: output_woof: lmr-1.OUT_WF_TYPE.3, value: 11.000000
fire_operand: output_woof: lmr-1.OUT_WF_TYPE.4, value: 2.000000
fire_operand: output_woof: lmr-1.OUT_WF_TYPE.5, value: 3.000000
Completed input 
```
and on 192.168.10.3 the application should print
```
Result: 39
```
and complete.


 
