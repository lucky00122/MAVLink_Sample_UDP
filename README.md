# MAVLink_Sample_UDP
Purpose:<br>
MAVLink study on UDP socket at localhost "127.0.0.1".

Reference:<br>
MAVLink website: https://mavlink.io/en/mavgen_c/example_c_udp.html<br>
Socket tutorial: http://zake7749.github.io/2015/03/17/SocketProgramming/


Source:<br>
Example source:<br>
			https://github.com/mavlink/mavlink/tree/master/examples/linux<br>
		MAVLink Library:<br>
			https://mavlink.io/en/#prebuilt_libraries<br>
		My Source:<br>
			https://github.com/lucky00122/MAVLink_Sample_UDP.git

1. Download My Source and build:<br>
cd mavlink_udp/<br>
make<br>
./result<br>

2. It will listen for data on the localhost IP address, port 14551<br>
3. Open QGroundControl on the same machine and it will start broadcasting its HEARTBEAT on port 14551<br>

4. Clean the code:<br>
make clean<br>
