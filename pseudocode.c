void FaultHandler(int signo, siginfo_t *info, void *context);

void InitFaultHandler()
{
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags =
        static_cast<int>(SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO);
    act.sa_sigaction = FaultHandler;

    sigaction(SIGHUP, &act, nullptr);
    sigaction(SIGQUIT, &act, nullptr);
    sigaction(SIGABRT, &act, nullptr);
    sigaction(SIGFPE, &act, nullptr);
    sigaction(SIGSEGV, &act, nullptr);
    sigaction(SIGPIPE, &act, nullptr);
    sigaction(SIGBUS, &act, nullptr);
    sigaction(SIGINT, &act, nullptr);
}

void FaultHandler(int signo, siginfo_t *info, void *context)
{
    Q_UNUSED(info);
    // Q_UNUSED(context);

    typedef struct _sig_ucontext {
        unsigned long uc_flags;
        struct ucontext *uc_link;
        stack_t uc_stack;
        struct sigcontext uc_mcontext;
        sigset_t uc_sigmask;
    } sig_ucontext_t;

    qDebug() << __FUNCTION__ << ":" << __LINE__ << ": Signal:" << signo;

    switch (signo) {
        case SIGSEGV:
        case SIGPIPE:
        case SIGBUS:
        case SIGHUP:
        case SIGQUIT:
        case SIGABRT:
        case SIGFPE:
        case SIGINT: {
        } break;
    }

    void *array[50];
    void *caller_address;
    char **messages;
    int size, i;
    sig_ucontext_t *uc;

    uc = (sig_ucontext_t *)context;

    /* Get the address at the time the signal was raised */
#if defined(__i386__)                              // gcc specific
    caller_address = (void *)uc->uc_mcontext.eip;  // EIP: x86 specific
#elif defined(__x86_64__)                          // gcc specific
    caller_address = (void *)uc->uc_mcontext.rip;  // RIP: x86_64 specific
#else
    //#error Unsupported architecture. // TODO: Add support for other arch.
    caller_address = (void *)uc->uc_mcontext.arm_pc;  // ARM
#endif

    fprintf(stderr, "signal %d (%s), address is %p from %p\n", signo,
            strsignal(signo), info->si_addr, (void *)caller_address);

    size = backtrace(array, 50);

    /* overwrite sigaction with caller's address */
    array[1] = caller_address;

    messages = backtrace_symbols(array, size);

    /* skip first stack frame (points here) */
    for (i = 1; i < size && messages != NULL; ++i) {
        fprintf(stderr, "[bt]: (%d) %s\n", i, messages[i]);
    }

    free(messages);

    exit(EXIT_FAILURE);
}

#cmakelists add
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic -funwind-tables -ffunction-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -funwind-tables -ffunction-sections")
#cmakelists
main
{
	
}
