/* Minimal corrected patterns. */

void doEcho(safeMessage *msg, char **response, int *finished) {
    int len;
    char *echo = msg->safeBuffer + msg->debut;

    len = snprintf(NULL, 0, "%s", echo) + 1;
    *response = malloc((size_t)len);
    if (*response == NULL) {
        return;
    }
    snprintf(*response, (size_t)len, "%s", echo);
    *finished = 0;
}

int sanitizeBuffer(char *unsafeBuffer, char **response, int *finished) {
    safeMessage msg;
    size_t len = strnlen(unsafeBuffer, BUFFERLENGTH);

    if (len >= BUFFERLENGTH) {
        return -BUFFERTOOLONG;
    }

    memcpy(msg.safeBuffer, unsafeBuffer, len + 1);
    msg.len = (int)len;
    msg.debut = 0;
    return parseCommand(&msg, response, finished);
}
