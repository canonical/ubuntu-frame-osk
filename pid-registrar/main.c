#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const char* auth_dir_env = "AUTH_DIR";
char auth_dir[256];
char auth_path[256];
pid_t child_pid;

// Result does not need to be freed
void init_auth_path(pid_t pid)
{
    int length = snprintf(auth_path, sizeof(auth_path), "%s/allow-%d", auth_dir, pid);
    if (length + 1 >= sizeof(auth_path))
    {
        fprintf(stderr, "Path %s too long\n", auth_dir);
        exit(1);
    }
}

void process_sigchld()
{
    init_auth_path(child_pid);
    if (remove(auth_path) < 0)
    {
        fprintf(stderr, "Failed to remove auth file %s", auth_path);
        perror(" ");
    }
    int wstatus = 0;
    waitpid(child_pid, &wstatus, 0);
    int return_code = WEXITSTATUS(wstatus);
    exit(return_code);
}

int main(int argc, char* const* argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No arguments provided\n");
        exit(1);
    }

    const char* auth_dir_value = getenv(auth_dir_env);
    if (!auth_dir_value)
    {
        fprintf(stderr, "%s environment variable not set\n", auth_dir_env);
        exit(1);
    }
    strncpy(auth_dir, auth_dir_value, sizeof(auth_dir));

    signal(SIGCHLD, process_sigchld);

    child_pid = fork();
    switch (child_pid)
    {
    case -1:
        fprintf(stderr, "Failed to fork process\n");
        exit(1);

    case 0:
        init_auth_path(getpid());
        FILE* file = fopen(auth_path, "w");
        if (!file)
        {
            fprintf(stderr, "Failed to create auth file %s", auth_path);
            perror(" ");
        }
        else
        {
            fclose(file);
        }
        execvp(argv[1], argv + 1);
        fprintf(stderr, "Failed to start process %s ...", argv[1]);
        perror(" ");
        exit(1);

    default:
        while(1)
        {
            pause();
        }
    }
}

