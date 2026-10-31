# ECE361-Computer-Networks

This repository contains lab assignments completed as part of **ECE361: Computer Networks** at the University of Toronto. The labs focus on network programming in C using UNIX sockets, covering both connection-oriented and connectionless communication.

## Labs

### Lab 1 – File Transfer (UDP)

Implemented a client-server file transfer application using **UDP sockets** in C.

The implementation covers:
- UDP socket communication between a client and server.
- File transfer using packet fragmentation with a maximum payload of 1000 bytes.
- Packet serialization and deserialization to support binary file data.
- Stop-and-wait acknowledgement using ACK/NACK packets.
- Round-trip time (RTT) measurement and retransmission timeouts.
- Simulated packet loss to test reliable file delivery.

### Lab 4 – Text Conferencing (TCP)

Implemented a multi-client text conferencing application using **TCP sockets** in C.

The implementation covers:
- Client login and authentication through a central server.
- Conference session creation, joining, and leaving.
- Message broadcasting to all clients within a session.
- Online user and available session queries.
- Simultaneous handling of user input and incoming server messages.
- Additional conferencing features beyond the core requirements.

## Technologies

- C
- UNIX/Linux
- TCP and UDP sockets
- Berkeley Socket API
- Makefiles

## Compilation and Execution

Each lab includes the source files and Makefile required to compile its client and server programs. Refer to the individual lab directories for compilation and execution instructions.

## Academic Context

Completed as part of ECE361 – Computer Networks at the University of Toronto.

Course materials and assignment specifications were provided by the ECE361 teaching team. All rights to the original assignment specifications and course materials belong to their respective owners.
