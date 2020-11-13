# MAVLink_Sample_UDP
Purpose
========
MAVLink study on UDP socket at localhost "127.0.0.1".

Reference
========
* MAVLink website: https://mavlink.io/en/mavgen_c/example_c_udp.html
* Socket tutorial: http://zake7749.github.io/2015/03/17/SocketProgramming/


Source
========
* Example source: https://github.com/mavlink/mavlink/tree/master/examples/linux
* MAVLink Library: https://mavlink.io/en/#prebuilt_libraries
* My Source: https://github.com/lucky00122/MAVLink_Sample_UDP.git

Step
========
1. Download My Source and build
```
$ git clone https://github.com/lucky00122/MAVLink_Sample_UDP.git
$ cd MAVLink_Sample_UDP/
$ make
$ ./result
```

2. It will listen for data on the localhost IP address, port 14551
3. Open QGroundControl on the same machine and it will start broadcasting its HEARTBEAT on port 14551

4. Clean the code
```
$ make clean
```
