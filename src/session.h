#ifndef SESSION_H

#define SESSION_H

extern sp_session_callbacks session_callbacks;

void session_logged_in(sp_session *session, sp_error error);

void session_notify_main_thread(sp_session *session);

#endif
