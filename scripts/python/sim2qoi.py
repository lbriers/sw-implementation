#!/usr/bin/python3

ifname = "../../firmware/course_example_1/simulation_output.dat"
ofname = "../../firmware/course_example_1/sim_output.qoi"

ifh = open(ifname, "r")
ofh = open(ofname, "wb")

line_number = 0
for line in ifh:
    line_number += 1
    if line_number % 2 == 0:  # Process only even-numbered lines
        line = line.rstrip()
        line_int = int(line, 2)
        ofh.write(line_int.to_bytes(1, byteorder="big"))

ifh.close()
ofh.close()

