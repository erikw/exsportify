#ifndef SESSION_H

#define SESSION_H

extern sp_session_callbacks session_callbacks;

void session_logged_in(sp_session *session, sp_error error);

#endif
