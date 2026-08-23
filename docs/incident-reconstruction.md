# Incident reconstruction

## Scope

The analysis starts from two suspicious TCP streams involving the service protocol. The protocol was intended to carry text commands, but the traces contain format-string probes, binary data, and a shell session.

## Trace chronology

### 1. Reconnaissance through `ECHO`

The attacker sends inputs such as:

```text
ECHO %x%x%x%x %x
```

The server replies with hexadecimal words that include values in the binary and stack address ranges. This confirms that the user-controlled string is interpreted as a format string rather than as data.

The numbered form was then used to identify the position of attacker-controlled bytes on the stack:

```text
ECHO %50$08xAAAA
ECHO %51$08xAAAA
ECHO %52$08xAAAA
ECHO %53$08xAAAA
ECHO %54$08xAAAA
```

The important observation is not that `%x` prints “the address of an integer”. It consumes a variadic argument as an integer and prints its value in hexadecimal. The disclosed values are useful because they reveal stack words and, at later offsets, attacker-controlled input.

### 2. Exploitation stream

The later stream contains a large binary request with a NOP sled, x86 code, `/bin/sh`, structured bytes after the 200-byte boundary, and a final line terminator. Immediately after the binary request, the stream changes from the application protocol to shell commands.

The observed sequence is consistent with:

1. stack disclosure and calibration;
2. delivery of a binary payload;
3. stack corruption and control-flow redirection;
4. execution of a shell attached to the socket.

### 3. Post-exploitation activity

The recorded shell session executed system identification and directory listing commands, then accessed application data. The publication keeps only the activity categories and sanitized command examples. It does not redistribute the original account database or customer-like records.

## Evidence boundary

The captured application payload after the network headers is 214 bytes. Its high-level structure is:

| Payload offset | Content | Interpretation |
| ---: | --- | --- |
| `0-63` | `0x90` repeated | NOP sled |
| `64-183` | 120 bytes of x86 code | Shellcode |
| `184-190` | `/bin/sh` | Shellcode data |
| `191-199` | `A` padding | Fills the 200-byte destination |
| `200-203` | `0a 00 00 00` | Boundary bytes; the newline is replaced by NUL before sanitization |
| `204-207` | `0d 00 00 00` | Little-endian value 13 |
| `208-211` | `9b a1 40 84` | Observed partial pointer/control bytes |
| `212-213` | `ff 0a` | Remaining address/line termination bytes in the captured stream |

The Python reproduction used during the original test generated a 215-byte request. That difference is preserved as an evidence distinction rather than hidden by forcing the two artifacts into one format.

## What the trace proves

The trace proves the transition from format-string probing to binary payload delivery and then to an interactive shell. The C source and GDB observations explain why the boundary write makes that transition possible.
