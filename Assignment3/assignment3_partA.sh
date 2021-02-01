#!/bin/sh


# Name : Patel Devarshi Chandrakant
# Roll No : 18CS10040
# ASSIGNMENT 3 | Computer Networks Laboratory


# PART A

# Creating Namespaces
sudo ip netns add ns1
sudo ip netns add ns2

# Creating veth and linking with eachother
sudo ip link add Veth0 type veth peer name Veth1

# Linking veth with respective namespaces
sudo ip link set Veth0 netns ns1
sudo ip link set Veth1 netns ns2

# IP addressing of veth and setting them up 
sudo ip -n ns1 addr add 10.1.1.0/24 dev Veth0
sudo ip -n ns1 link set dev Veth0 up
sudo ip -n ns2 addr add 10.1.2.0/24 dev Veth1
sudo ip -n ns2 link set dev Veth1 up

# Manual Routing ns1->ns2 
sudo ip -n ns1 route add 10.1.2.0/24 dev Veth0

# Manual Routing ns2->ns1
sudo ip -n ns2 route add 10.1.1.0/24 dev Veth1


# To clear namespaces execute
# sudo ip -all netns delete
