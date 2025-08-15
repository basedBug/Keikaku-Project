#include "globals/globals.h"

// Mutex for protecting the websocket access
//SemaphoreHandle_t wsMutex = NULL;

MessageBufferHandle_t datahandlerToWsMessageBuffer;
MessageBufferHandle_t wsToDatahandlerTaskMessageBuffer;
const size_t BUFFER_SIZE = 2048;
const size_t MAX_MSG_SIZE = 1024;

