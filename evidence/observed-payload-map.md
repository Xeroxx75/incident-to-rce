# Observed payload map

This is a sanitized map of the application payload observed after the packet headers. It is intentionally not a raw PCAP or full packet dump.

| Payload offset | Bytes or pattern | Meaning |
| ---: | --- | --- |
| `0-63` | `90` repeated 64 times | NOP sled |
| `64-183` | `eb 71 5d ... e8 8a ff ff ff` | 120-byte x86 shellcode |
| `184-190` | `2f 62 69 6e 2f 73 68` | `/bin/sh` |
| `191-199` | `41` repeated 9 times | Padding |
| `200-203` | `0a 00 00 00` | Boundary/reset sequence |
| `204-207` | `0d 00 00 00` | Little-endian 13 |
| `208-211` | `9b a1 40 84` | Observed control bytes |
| `212-213` | `ff 0a` | Remaining suffix bytes |
