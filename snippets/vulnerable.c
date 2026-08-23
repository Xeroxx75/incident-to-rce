/* Minimal excerpts from the analyzed service. */

void doEcho(safeMessage *msg, char **response, int *finished) {
    int len;
    char *echo = msg->safeBuffer + msg->debut;

    len = snprintf(*response, 0, echo) + 1;
    *response = malloc((size_t)len);
    snprintf(*response, (size_t)len, echo);
    *finished = 0;
}

int sanitizeBuffer(char *unsafeBuffer, char **response, int *finished) {
    safeMessage msg;

    msg.len = strlen(unsafeBuffer);
    msg.src = unsafeBuffer;
    msg.dst = msg.safeBuffer;

    if (msg.len > BUFFERLENGTH) {
        return -BUFFERTOOLONG;
    }

    for (msg.i = 0; msg.i <= msg.len; msg.i++) {
        *(msg.dst++) = *(msg.src++);
    }

    return parseCommand(&msg, response, finished);
}
