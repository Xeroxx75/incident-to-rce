# Root causes and remediation

## Root cause 1: attacker-controlled format string

The `ECHO` implementation uses the user-controlled string as the format argument twice:

```c
len = snprintf(*reponse, 0, echo) + 1;
snprintf(*reponse, len, echo);
```

Both calls must use a constant format. Fixing only the second call still leaves the first call able to read variadic arguments while calculating the output length.

The corrected pattern is:

```c
len = snprintf(NULL, 0, "%s", echo) + 1;
*reponse = malloc((size_t)len);
if (*reponse == NULL) {
    return;
}
snprintf(*reponse, (size_t)len, "%s", echo);
```

The input is now data, not a format program.

## Root cause 2: inclusive copy at the buffer boundary

Changing `<=` to `<` prevents the out-of-bounds write, but it changes the handling of an input whose length equals the buffer size and can leave a string unterminated before later `strlen` calls.

For a string buffer that must contain its terminator, reject an input that consumes the full capacity and copy the terminator explicitly:

```c
size_t len = strnlen(unsafeBuffer, BUFFERLENGTH);
if (len >= BUFFERLENGTH) {
    return -BUFFERTOOLONG;
}

memcpy(msg.safeBuffer, unsafeBuffer, len + 1);
msg.len = (int)len;
```

An equivalent implementation may retain a loop, but the accepted length, copy count, and NUL-termination rule must be expressed together and tested at lengths 199 and 200.

## Defense in depth

The code fixes remove the two primitives used by this exploit. Additional controls reduce the impact of future defects:

- keep the stack non-executable with NX;
- compile with stack protection such as `-fstack-protector-strong`;
- build position-independent executables with PIE where supported;
- enable RELRO and fortified libc calls;
- use `size_t` for buffer lengths and check all allocation and I/O results;
- run the service as a dedicated least-privilege account with restricted data access;
- keep the protocol parser strict about binary input and line boundaries;
- add static analysis and compiler warnings to the build;
- monitor for format-string probes, long NOP sequences, and binary payloads on a text protocol.

These controls are complementary. Hardening does not replace fixing the unsafe format and bounds logic.
