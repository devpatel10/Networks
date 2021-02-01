#!/bin/sh

# Name : Patel Devarshi Chandrakant
# Roll No : 18CS10040
# ASSIGNMENT 3 | Computer Networks Laboratory


#  PART B

# Creating namespaces
sudo ip netns add R
sudo ip netns add H1
sudo ip netns add H2
sudo ip netns add H3

# Creating veths' and linking them with each other according to question
sudo ip link add veth1 type veth peer name veth2
sudo ip link add veth4 type veth peer name veth5
sudo ip link add veth3 type veth peer name veth6

# Linking veths' with their respective namespaces
sudo ip link set veth1 netns H1
sudo ip link set veth6 netns H2
sudo ip link set veth5 netns H3
sudo ip link set veth2 netns R
sudo ip link set veth3 netns R
sudo ip link set veth4 netns R

# IP Addressing of veth and setting them to 'up'
sudo ip -n H1 addr add 10.0.10.40/24 dev veth1
sudo ip -n H1 link set dev veth1 up
sudo ip -n H2 addr add 10.0.20.40/24 dev veth6
sudo ip -n H2 link set dev veth6 up
sudo ip -n H3 addr add 10.0.30.40/24 dev veth5
sudo ip -n H3 link set dev veth5 up
sudo ip -n R addr add 10.0.10.1/24 dev veth2
sudo ip -n R link set dev veth2 up
sudo ip -n R addr add 10.0.20.1/24 dev veth3
sudo ip -n R link set dev veth3 up
sudo ip -n R addr add 10.0.30.1/24 dev veth4
sudo ip -n R link set dev veth4 up

# Creating bridge
sudo ip netns exec R brctl addbr bridgeR

# Addying interfaces to bridge
sudo ip netns exec R brctl addif bridgeR veth2 veth3 veth4

# Setting bridge to 'up'
sudo ip -n R link set dev bridgeR up

# Turning on IP Forwarding at namespace R 
sudo ip netns exec R sysctl -w net.ipv4.ip_forward=1

# Default Routing of namespaces H1,H2,H3
sudo ip -n H1 route add default via 10.0.10.40
sudo ip -n H2 route add default via 10.0.20.40
sudo ip -n H3 route add default via 10.0.30.40

#   To clear namespaces execute
#    sudo ip -all netns delete
