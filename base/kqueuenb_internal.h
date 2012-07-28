
#define KQUEUE_NONBLOCK_INIT kqueue_nonblock=false

#define KEVENT_CALL(kfd, changelist, nchanges, eventlist, nevents) ({	\
			timespec ts;					\
									\
			kevent((kfd), (changelist), (nchanges),		\
			       (eventlist),				\
			       (nevents), kqueue_nonblock ? &ts:NULL);	\
		})

#define KEVENT_POLL(kfd, changelist, nchanges, eventlist, nevents) ({	\
			timespec ts;					\
									\
			kevent((kfd), (changelist), (nchanges),		\
			       (eventlist),				\
			       (nevents), &ts);				\
		})

#define KQUEUE_NONBLOCK_IMPL					\
	void KQUEUE_CLASS::nonblock(bool flag) throw(exception)	\
	{							\
		kqueue_nonblock=flag;				\
	}							\
								\
	bool KQUEUE_CLASS::nonblock() throw(exception)		\
	{							\
		return kqueue_nonblock;			\
								\
	}
